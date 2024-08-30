#include "rpc_message.h"

#include <vector>
#include <algorithm>
#include "hendian.h"

RpcMessage::RpcMessage() {
  header_ = 0xABCDDCBA;
  request_id_ = 0;
  command_ = 0;
  type_ = 0;
  length_ = 0;
  payload_ = "";
}

int RpcMessage::ParseFromString(const std::string& message) {
  int ret = 0;
  if (message.size() < RUNTIME_MSG_HEADER_SIZE) {
    return -1;
  }
  const char* p = message.c_str();
  header_ = ntohl(*reinterpret_cast<const int*>(p));
  request_id_ = ntohl(*reinterpret_cast<const int*>(p + 4));
  command_ = ntohs(*reinterpret_cast<const unsigned short*>(p + 8));
  type_ = ntohs(*reinterpret_cast<const unsigned short*>(p + 10));
  length_ = ntohl(*reinterpret_cast<const int*>(p + 12));

  if (length_ < JSON_STRING_MINIMUM_SIZE ||
      header_ != RUNTIME_MSG_HEADER_PRIFIX) {
    return -2;
  }
  payload_ = message.substr(RUNTIME_MSG_HEADER_SIZE, length_);

  return ret;
}

std::string RpcMessage::SerializeAsString() {
  char buf[RUNTIME_MSG_HEADER_SIZE];
  int* p = reinterpret_cast<int*>(buf);
  *p = htonl(header_);
  p = reinterpret_cast<int*>(buf + 4);
  *p = htonl(request_id_);
  unsigned short* q = reinterpret_cast<unsigned short*>(buf + 8);
  *q = htons(command_);
  q = reinterpret_cast<unsigned short*>(buf + 10);
  *q = htons(type_);
  p = reinterpret_cast<int*>(buf + 12);
  *p = htonl(length_);

  return std::string(buf, RUNTIME_MSG_HEADER_SIZE) + payload_;
}

int RpcMessage::protorpc_pack(const RpcMessage* msg, void* buf, int len) {
  if (!msg || !buf || !len) return -1;
  unsigned int packlen = protorpc_package_length(msg);
  if (len < packlen) {
    return -2;
  }
  unsigned char* p = (unsigned char*)buf;
  *p++ = (msg->header_ >> 24) & 0xFF;
  *p++ = (msg->header_ >> 16) & 0xFF;
  *p++ = (msg->header_ >> 8) & 0xFF;
  *p++ = msg->header_ & 0xFF;

  *p++ = (msg->request_id_ >> 24) & 0xFF;
  *p++ = (msg->request_id_ >> 16) & 0xFF;
  *p++ = (msg->request_id_ >> 8) & 0xFF;
  *p++ = msg->request_id_ & 0xFF;

  *p++ = (msg->command_ >> 8) & 0xFF;
  *p++ = msg->command_ & 0xFF;

  *p++ = (msg->type_ >> 8) & 0xFF;
  *p++ = msg->type_ & 0xFF;

  *p++ = (msg->length_ >> 24) & 0xFF;
  *p++ = (msg->length_ >> 16) & 0xFF;
  *p++ = (msg->length_ >> 8) & 0xFF;
  *p++ = msg->length_ & 0xFF;

  if (msg->length_) {
    memcpy(p, msg->payload_.c_str(), msg->length_);
  }
  return packlen;
}

int RpcMessage::protorpc_unpack(const void* buf, int len) {
  if (!buf || !len) return -1;
  if (len < RUNTIME_MSG_HEADER_SIZE) return -2;
  const char* p = (const char*)buf;
  this->header_ = ntohl(*reinterpret_cast<const int*>(p));
  this->request_id_ = ntohl(*reinterpret_cast<const int*>(p + 4));
  this->command_ = ntohs(*reinterpret_cast<const unsigned short*>(p + 8));
  this->type_ = ntohs(*reinterpret_cast<const unsigned short*>(p + 10));
  this->length_ = ntohl(*reinterpret_cast<const int*>(p + 12));
  int packlen = protorpc_package_length(this);
  if (len < packlen) {
    return -3;
  }

  if (len > RUNTIME_MSG_HEADER_SIZE) {
    this->payload_.assign(p + RUNTIME_MSG_HEADER_SIZE, this->length_);
  }

  return packlen;
}

int RpcMessage::protorpc_package_length(const RpcMessage* msg) {
  return RUNTIME_MSG_HEADER_SIZE + msg->length_;
}