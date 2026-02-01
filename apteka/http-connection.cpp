#include "http-connection.hpp"
#include "cc/log.hpp"
#include "common.hpp"
#include "http-req.hpp"
#include "http-server.hpp"
#include "router.hpp"
#include "uv.h"

HttpConnection::HttpConnection(HttpServer& server, Router& router)
    : response_(*this), server_(server), router_(router) {
  // mLogInfo("Connection created!");
  memset(&stream_, 0, sizeof(stream_));
  memset(&write_req_, 0, sizeof(write_req_));
  memset(&response_buf_, 0, sizeof(response_buf_));
}

HttpConnection::~HttpConnection() {
  server_.report_closed(this);
  reset();
}

bool HttpConnection::init(uv_stream_t* socket) {
  mLogDebug("Client connected");
  if (not mUvCheckError(uv_tcp_init(uv_default_loop(), &stream_))) {
    return false;
  }
  stream_open_ = true;
  if (not mUvCheckError(uv_accept(socket, get_stream()))) {
    return false;
  }
  if (not mUvCheckError(uv_tcp_nodelay(&stream_, 1))) {
    return false;
  }
  if (not mUvCheckError(uv_tcp_keepalive(&stream_, 1, 45))) {
    return false;
  }
  stream_.data = this;
  return true;
}

uv_stream_t* HttpConnection::get_stream() {
  return (uv_stream_t*)&stream_;
}

uv_handle_t* HttpConnection::get_handle() {
  return (uv_handle_t*)&stream_;
}

void HttpConnection::close() {
  if (stream_open_) {
    uv_close(get_handle(), close_cb);
  }
}

void HttpConnection::start() {
  mLogDebug("Read start");
  int r = uv_read_start(get_stream(), uv_alloc_buffer, read_cb);
  if (r < 0) {
    mLogWarn("Error read start: ", StrView(uv_err_name(r)));
  }
}

void HttpConnection::send(StrView data) {
  response_buf_ = uv_alloc_buffer(data.size());
  memcpy(response_buf_.base, data.data(), data.size());
  write_req_.data = this;
  uv_write(&write_req_, get_stream(), &response_buf_, 1, write_cb);
}

void HttpConnection::reset() {
  mLogDebug("Reset http connection");
  uv_free_buffer(&response_buf_);
  memset(&write_req_, 0, sizeof(write_req_));
  memset(&response_buf_, 0, sizeof(response_buf_));
  req_parser_.reset();
  response_.reset();
}

void HttpConnection::close_cb(uv_handle_t* handle) {
  HttpConnection* self = (HttpConnection*)handle->data;
  self->stream_open_   = false;
  delete self;
}

void HttpConnection::read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
  HttpConnection* self = (HttpConnection*)stream->data;
  mLogDebug("Read: ", nread);

  if (nread < 0) {
    mLogDebug("Read error (connection closed?)");
    self->close();
  } else if (nread > 0) {
    StrView data = StrView(buf->base, nread);
    if (not self->req_parser_.handle(data)) {
      mLogDebug("Request parsing error");
      self->close();
    } else if (self->req_parser_.is_parsing_done()) {
      mLogDebug("Parsing done");

      HttpReq req = self->req_parser_.get_builder().build();
      mLogDebug("HTTP ", StrView(llhttp_method_name(req.method)), " ", req.url);

      self->router_.handle(req, self->response_);

      // self->response_.send_basic(HTTP_STATUS_OK);

      // self->response_.status(HTTP_STATUS_OK)
      //     .content_type(ContentType::text_plain())
      //     .body(Str("Hello!"))
      //     .send();
    }
  }

  uv_free_buffer(buf);
}

void HttpConnection::write_cb(uv_write_t* req, int status) {
  HttpConnection* self = (HttpConnection*)req->data;
  if (status < 0) {
    mLogWarn("Error write: ", StrView(uv_err_name(status)));
    self->close();
  } else {
    self->reset();
  }
}
