#include "tcp_server.h"

#include <glog/logging.h>

#include "hbuf.h"

HTcpServer::HTcpServer(int port, int thread_num) {
  memset(&protorpc_unpack_setting_, 0, sizeof(unpack_setting_t));
  protorpc_unpack_setting_.mode = UNPACK_BY_LENGTH_FIELD;
  protorpc_unpack_setting_.package_max_length = DEFAULT_PACKAGE_MAX_LENGTH;
  protorpc_unpack_setting_.body_offset = RUNTIME_MSG_HEADER_SIZE;
  protorpc_unpack_setting_.length_field_offset =
      PROTORPC_HEAD_LENGTH_FIELD_OFFSET;
  protorpc_unpack_setting_.length_field_bytes =
      PROTORPC_HEAD_LENGTH_FIELD_BYTES;
  protorpc_unpack_setting_.length_field_coding = ENCODE_BY_BIG_ENDIAN;
  setUnpack(&protorpc_unpack_setting_);
  port_ = port;
  setThreadNum(thread_num);
  onMessage = std::bind(&HTcpServer::HandleMessage, this, std::placeholders::_1,
                        std::placeholders::_2);
}

int HTcpServer::Start() {
  int listenfd = createsocket(port_);
  if (listenfd < 0) {
    LOG(ERROR) << "createsocket failed";
    return -1;
  }

  this->start();
  LOG(INFO) << "HTcpServer start success, listen port: " << port_
            << " listenfd: " << listenfd;
  return 0;
}

void HTcpServer::Stop() { stop(); }

void HTcpServer::RegisterOnConnection(
    std::function<void(const TSocketChannelPtr&)> onconnection) {
  onConnection = onconnection;
}

void HTcpServer::HandleMessage(const TSocketChannelPtr& socketChannel,
                               Buffer* buff) {
  RpcMessage rpc_msg;
  memset(&rpc_msg, 0, sizeof(rpc_msg));
  int packlen = rpc_msg.protorpc_unpack(buff->data(), buff->size());
  if (packlen < 0) {
    LOG(ERROR) << "protorpc_unpack failed!";
    return;
  }

  if (message_callback_ != nullptr) {
    message_callback_(socketChannel, rpc_msg);
  }
}

void HTcpServer::SendMessage(const TSocketChannelPtr& socketChannel,
                             RpcMessage& rpc_msg) {
  int packlen = rpc_msg.length_ + RUNTIME_MSG_HEADER_SIZE;
  unsigned char* writebuf = NULL;
  HV_STACK_ALLOC(writebuf, packlen);
  packlen = rpc_msg.protorpc_pack(&rpc_msg, writebuf, packlen);
  if (packlen > 0) {
    socketChannel->write(writebuf, packlen);
  }
  HV_STACK_FREE(writebuf);
}

void HTcpServer::RegisterOnMessage(MsgCallback callback) {
  message_callback_ = callback;
}