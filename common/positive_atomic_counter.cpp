#include "positive_atomic_counter.h"

int PositiveAtomicCounter::IncrementAndGet() {
  int value = value_.load();
  int next = (value + 1) & kMask;
  while (!value_.compare_exchange_weak(value, next)) {
    next = (value + 1) & kMask;
  }
  return next;
}

int PositiveAtomicCounter::GetAndIncrement() {
  int value = value_.load();
  int next = (value + 1) & kMask;
  while (!value_.compare_exchange_weak(value, next)) {
    next = (value + 1) & kMask;
  }
  return value;
}