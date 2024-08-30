#pragma once
#include <EventLoopThread.h>

#include <thread>

#include "TcpClient.h"
#include "message_future.h"
#include "positive_atomic_counter.h"
#include "rpc_message.h"

using namespace hv;

typedef enum {
  kConnecting,
  kConnected,
  kDisconnectd,
} ConnectionState;

typedef std::function<void(const TcpClient::TSocketChannelPtr&,
                           RpcMessage& data)>
    MsgCallback;
typedef std::function<void(const TcpClient::TSocketChannelPtr&)>
    ConnectionCallback;
class HTcpClient : public TcpClient {
 public:
  HTcpClient(std::string host, int port);
  int Start();
  void Stop();
  void RegisterOnConnection(
      std::function<void(const TSocketChannelPtr&)> onconnection);

  void RegisterOnMessage(MsgCallback callback);
  void SendMessage(RpcMessage& rpc_msg);
  RpcMessage SendSyncMessage(RpcMessage& rpc_msg, const int64_t timeout);

 private:
  void HandleConnection(const TSocketChannelPtr& socketChannel);
  void HandleMessage(const TSocketChannelPtr& socketChannel, Buffer* buffer);

 private:
  int port_;
  std::string host_;
  TSocketChannelPtr socketChannel_;
  MsgCallback message_callback_;
  ConnectionCallback connection_callback_;
  std::mutex mutex_;
  std::map<int, std::shared_ptr<MessageFuture>> map_;
  PositiveAtomicCounter counter_;
  ConnectionState connect_state_;
};