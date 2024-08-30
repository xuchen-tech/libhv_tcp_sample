#include "tcp_client.h"

#include <glog/logging.h>

#include "timeout_exception.h"

HTcpClient::HTcpClient(std::string host, int port) : host_(host), port_(port) {
  onMessage = std::bind(&HTcpClient::HandleMessage, this, std::placeholders::_1,
                        std::placeholders::_2);
  onConnection =
      std::bind(&HTcpClient::HandleConnection, this, std::placeholders::_1);
}

int HTcpClient::Start() {
  int ret = 0;
  if (ret = createsocket(port_, host_.c_str()) < 0) {
    return ret;
  };
  connect_state_ = kConnecting;
  start();
  return ret;
}

void HTcpClient::HandleConnection(const TSocketChannelPtr& socketChannel) {
  if (socketChannel->isConnected()) {
    connect_state_ = kConnected;
    socketChannel_ = socketChannel;
  } else {
    connect_state_ = kDisconnectd;
  }
  if (connection_callback_ != nullptr) {
    connection_callback_(socketChannel);
  }
}

void HTcpClient::HandleMessage(const TSocketChannelPtr& socketChannel,
                               Buffer* buffer) {
  RpcMessage rpc_msg;
  memset(&rpc_msg, 0, sizeof(rpc_msg));
  int packlen = rpc_msg.protorpc_unpack(buffer->data(), buffer->size());
  if (packlen < 0) {
    LOG(ERROR) << "protorpc_unpack failed!";
    return;
  }

  std::lock_guard<std::mutex> lock(mutex_);
  auto it = map_.find(rpc_msg.request_id_);
  if (it != map_.end()) {
    it->second->promise_.set_value(rpc_msg);
    map_.erase(it);
  } else {
    if (message_callback_ != nullptr) {
      message_callback_(socketChannel, rpc_msg);
    }
  }
}

void HTcpClient::Stop() { stop(); }

void HTcpClient::RegisterOnConnection(
    std::function<void(const TSocketChannelPtr&)> onconnection) {
  connection_callback_ = onconnection;
}

void HTcpClient::RegisterOnMessage(MsgCallback callback) {
  message_callback_ = callback;
}

void HTcpClient::SendMessage(RpcMessage& rpc_msg) {
  if (connect_state_ != kConnected) {
    LOG(ERROR) << "HTcpClient::SendMessage() - not connected!";
    return;
  }
  int packlen = rpc_msg.length_ + RUNTIME_MSG_HEADER_SIZE;
  unsigned char* writebuf = NULL;
  HV_STACK_ALLOC(writebuf, packlen);
  packlen = rpc_msg.protorpc_pack(&rpc_msg, writebuf, packlen);
  if (packlen > 0) {
    socketChannel_->write(writebuf, packlen);
  }
  HV_STACK_FREE(writebuf);
}

RpcMessage HTcpClient::SendSyncMessage(RpcMessage& rpc_msg,
                                       const int64_t timeout) {
  if (connect_state_ != kConnected) {
    LOG(ERROR) << "HTcpClient::SendMessage() - not connected!";
    return RpcMessage();
  }
  std::shared_ptr<MessageFuture> message_future =
      std::make_shared<MessageFuture>();
  message_future->timeout_ = timeout;
  message_future->request_ = rpc_msg;
  rpc_msg.request_id_ = counter_.GetAndIncrement();
  {
    std::lock_guard<std::mutex> lock(mutex_);
    map_.insert(std::make_pair(rpc_msg.request_id_, message_future));
  }
  SendMessage(rpc_msg);
  try {
    return message_future->Get(timeout);
  } catch (TimeoutException& e) {
    LOG(ERROR) << "HTcpClient::SendSyncMessage() - timeout exception, timeout: "
               << timeout;
    // remove the request from the map
    std::lock_guard<std::mutex> lock(mutex_);
    size_t erased_count = map_.erase(rpc_msg.request_id_);
    LOG(INFO) << "HTcpClient::SendSyncMessage() - erased_count: "
              << erased_count;
    return RpcMessage();
  }
}