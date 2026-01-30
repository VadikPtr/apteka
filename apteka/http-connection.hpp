#pragma once
#include "uv.h"
#include "req-parser.hpp"

class HttpConnection {
  uv_tcp_t  stream_;
  ReqParser req_parser_;

 public:
  bool         init();
  uv_stream_t* get_stream();
  uv_handle_t* get_handle();
  void         close();
  void         start();

 private:
  static void close_cb(uv_handle_t* handle);
  static void read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
};
