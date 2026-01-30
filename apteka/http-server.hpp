#pragma once
#include "ip.hpp"
#include "uv.h"

class HttpServer {
  uv_tcp_t socket_;

 public:
  HttpServer();

  void listen(const SockAddr& addr);

 private:
  uv_stream_t* get_stream();

  static void connection_cb(uv_stream_t* server, int status);
};
