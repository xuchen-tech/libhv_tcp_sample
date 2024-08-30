#include <iostream>
#include <string>
#include <glog/logging.h>
#include "tcp_server.h"

HTcpServer server(9080, 4);

static void onConnection(const TcpServer::TSocketChannelPtr& channel) {
  if (channel->isConnected()) {
    LOG(INFO) << "connected";
  } else {
    LOG(INFO) << "disconnected";
  }
}

static void onMessage(const TcpServer::TSocketChannelPtr& channel, RpcMessage& data) {
  LOG(INFO) << "onMessage" << " request_id: " << data.request_id_ << " payload: " << data.payload_;
  // server.SendMessage(channel, data);
  // LOG(INFO) << "SendMessage END";
}

int main() {
  
  server.RegisterOnConnection(onConnection);
  server.RegisterOnMessage(onMessage);

  server.Start();

  while (true) hv_sleep(1);

  return 0;
}