#pragma once

#include <string>

#define RUNTIME_MSG_HEADER_SIZE (16)
#define JSON_STRING_MINIMUM_SIZE (2)
#define RUNTIME_MSG_HEADER_PRIFIX (0xABCDDCBA)
#define PROTORPC_HEAD_LENGTH_FIELD_OFFSET (12)
#define PROTORPC_HEAD_LENGTH_FIELD_BYTES (4)

class RpcMessage {
 public:
  RpcMessage();
  int ParseFromString(const std::string& message);
  std::string SerializeAsString();
  int protorpc_package_length(const RpcMessage* msg);
  int protorpc_pack(const RpcMessage* msg, void* buf, int len);
  int protorpc_unpack(const void* buf, int len);
 public:
  int header_ = 0xABCDDCBA;
  int request_id_;
  unsigned short command_;
  unsigned short type_;
  int length_;
  std::string payload_;
};