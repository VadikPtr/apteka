#pragma once
#include <uv.h>
#include <cc/str-view.hpp>

struct SockAddr {
  sockaddr addr;
  StrView  host;
  int      port;

  SockAddr(const char* host, int port);
  explicit SockAddr(const sockaddr* a_addr);

  bool is_empty() const;
};
