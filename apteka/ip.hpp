#pragma once
#include <uv.h>

struct SockAddr {
  sockaddr addr;

  SockAddr(const char* host, int port);
  explicit SockAddr(const sockaddr* a_addr);

  bool is_empty() const;
};
