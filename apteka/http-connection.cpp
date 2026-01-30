#include "http-connection.hpp"
#include "common.hpp"

bool HttpConnection::init() {
  mLogDebug("Client connected");
  memset(&stream_, 0, sizeof(stream_));
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
      // WRITE ANSWER
    }
  }

  uv_free_buffer(buf);
}
