#pragma once
#include <map>
#include <TcpServer.h>
#include "rpc_message.h"
#include "message_future.h"

using namespace hv;

typedef std::function<void(const TcpServer::TSocketChannelPtr&, RpcMessage& data)> MsgCallback;
class HTcpServer : public TcpServer {
 public:
  HTcpServer(int port, int thread_num);
  int Start();
  void Stop();
  void RegisterOnConnection(std::function<void(const TSocketChannelPtr&)> onconnection);
  void RegisterOnMessage(MsgCallback callback);
  void SendMessage(const TSocketChannelPtr& socketChannel, RpcMessage& rpc_msg);
 private:
  void HandleMessage(const TSocketChannelPtr& socketChannel, Buffer* buffer);
 private:
  unpack_setting_t protorpc_unpack_setting_;
  std::map<int32_t, std::shared_ptr<MessageFuture>> map_;
  int port_;
  MsgCallback message_callback_;
};