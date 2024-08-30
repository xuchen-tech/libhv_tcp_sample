#include <glog/logging.h>

#include <iostream>

#include "positive_atomic_counter.h"
#include "tcp_client.h"

PositiveAtomicCounter counter;
HTcpClient client("127.0.0.1", 9080);

static void onConnection(const TcpClient::TSocketChannelPtr& channel) {
  if (channel->isConnected()) {
    LOG(INFO) << "connected";
    RpcMessage msg;
    msg.command_ = 100;
    msg.request_id_ = counter.GetAndIncrement();
    msg.payload_ = "hello";
    msg.length_ = msg.payload_.size();
    // client.SendMessage(msg);
    std::thread([channel, &msg] {
      LOG(INFO) << "SendSyncMessage Start";
      RpcMessage ret = client.SendSyncMessage(msg, 10000);
      LOG(INFO) << "SendSyncMessage END" << " request_id: " << ret.request_id_
                << " payload: " << ret.payload_;

    }).detach();
  } else {
    LOG(INFO) << "disconnected";
  }
}

static void onMessage(const TcpClient::TSocketChannelPtr& channel,
                      RpcMessage& data) {
  LOG(INFO) << "onMessage" << " request_id: " << data.request_id_
            << " payload: " << data.payload_;
}

int main() {
  client.RegisterOnConnection(onConnection);
  client.RegisterOnMessage(onMessage);

  client.Start();

  while (true) hv_sleep(1);
  return 0;
}