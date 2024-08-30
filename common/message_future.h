#pragma once

#include <chrono>
#include <future>
#include <memory>

#include "rpc_message.h"

class MessageFuture {
 public:
  MessageFuture();
  ~MessageFuture();

  bool IsTimeout();
  RpcMessage Get(int64_t timeout);
  int64_t GetCurrentTime();

 public:
  std::mutex mutex_;
  RpcMessage request_;
  int64_t timeout_;  // timeout in milliseconds
  int64_t start_;    // start time
  std::promise<RpcMessage> promise_;
};