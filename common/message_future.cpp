#include "message_future.h"

#include "message_future.h"

#include "timeout_exception.h"

MessageFuture::MessageFuture() { start_ = GetCurrentTime(); }
MessageFuture::~MessageFuture() {}

int64_t MessageFuture::GetCurrentTime() {
  auto now = std::chrono::system_clock::now();
  auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
  return now_ms.time_since_epoch().count();
}

bool MessageFuture::IsTimeout() {
  std::lock_guard<std::mutex> guard(mutex_);
  return GetCurrentTime() - start_ > timeout_;
}

RpcMessage MessageFuture::Get(int64_t timeout) {
  std::future<RpcMessage> future = promise_.get_future();
  std::future_status status =
      future.wait_for(std::chrono::milliseconds(timeout));
  if (status == std::future_status::ready) {
    return future.get();
  } else if (status == std::future_status::timeout) {
    // throw timeout exception
    throw TimeoutException("timeout exception");
  }
}