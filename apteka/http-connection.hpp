#pragma once
#include "uv.h"
#include "req-parser.hpp"
#include "http-req.hpp"

class HttpConnection {
  uv_tcp_t   stream_;
  uv_write_t write_req_;
  uv_buf_t   response_buf_;
  ReqParser  req_parser_;
  HttpRes    response_;
  bool       closed_ = true;

 public:
  HttpConnection();
  ~HttpConnection();

  bool         init(uv_stream_t* server);
  uv_stream_t* get_stream();
  uv_handle_t* get_handle();
  void         close();
  void         start();
  void         send(StrView data);

 private:
  void reset();

  static void close_cb(uv_handle_t* handle);
  static void read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
  static void write_cb(uv_write_t* req, int status);
};
