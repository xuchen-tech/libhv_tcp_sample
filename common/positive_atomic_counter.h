#pragma once

#include <atomic>

static const int kMask = 0x7FFFFFFF;

class PositiveAtomicCounter {
 public:
  PositiveAtomicCounter() : value_(0) {}
  int IncrementAndGet();
  int GetAndIncrement();

 private:
  std::atomic<int> value_;
};