#pragma once
#include "ip.hpp"
#include "uv.h"

class HttpServer {
  uv_tcp_t socket_;
  size_t   connections_num_ = 0;

 public:
  HttpServer();

  void listen(const SockAddr& addr);
  void report_closed(class HttpConnection* con);

 private:
  uv_stream_t* get_stream();

  static void connection_cb(uv_stream_t* server, int status);
};
