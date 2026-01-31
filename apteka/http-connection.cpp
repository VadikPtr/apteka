#include "http-connection.hpp"
#include "cc/log.hpp"
#include "common.hpp"
#include "http-req.hpp"
#include "llhttp.h"
#include "mime.hpp"
#include "uv.h"

HttpConnection::HttpConnection() : response_(this) {
  memset(&response_buf_, 0, sizeof(response_buf_));
  memset(&write_req_, 0, sizeof(write_req_));
  memset(&stream_, 0, sizeof(stream_));
}

HttpConnection::~HttpConnection() {
  uv_free_buffer(&response_buf_);
}

bool HttpConnection::init() {
  mLogDebug("Client connected");

  if (not mUvCheckError(uv_tcp_init(uv_default_loop(), &stream_))) {
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
  uv_close(get_handle(), HttpConnection::close_cb);
}

void HttpConnection::start() {
  uv_read_start(get_stream(), uv_alloc_buffer, read_cb);
}

void HttpConnection::send(StrView data) {
  response_buf_ = uv_alloc_buffer(data.size());
  memcpy(response_buf_.base, data.data(), data.size());
  write_req_.data = this;
  uv_write(&write_req_, get_stream(), &response_buf_, 1, write_cb);
}

void HttpConnection::close_cb(uv_handle_t* handle) {
  HttpConnection* self = (HttpConnection*)handle->data;
  delete self;
}

void HttpConnection::read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
  HttpConnection* self = (HttpConnection*)stream->data;

  if (nread < 0) {
    mLogDebug("Read error (connection closed?)");
    self->close();
  } else {
    StrView data = StrView(buf->base, nread);
    if (not self->req_parser_.handle(data)) {
      mLogDebug("Request parsing error");
      self->close();
    }
    if (self->req_parser_.is_parsing_done()) {
      mLogDebug("Parsing done");
      self->response_.status(HTTP_STATUS_OK)
          .content_type(ContentType::text_plain())
          .body(Str("Hello!"))
          .send();
    }
  }

  uv_free_buffer(buf);
}

void HttpConnection::write_cb(uv_write_t* req, int status) {
  HttpConnection* self = (HttpConnection*)req->data;
  if (status < 0) {
    mLogDebug("Error write: ", StrView(uv_err_name(status)));
  }
  delete req;
  self->close();
}
