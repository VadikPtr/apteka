#include "http-req.hpp"
#include "http-connection.hpp"
#include "cc/fmt.hpp"
#include "cc/log.hpp"

void HttpReq::reset() {
  url     = Str();
  headers = Dict<StrHash, Str>();
}

HttpRes::HttpRes(HttpConnection* http_connection) : http_connection_(http_connection) {}

HttpRes& HttpRes::status(llhttp_status value) {
  status_ = value;
  return *this;
}

HttpRes& HttpRes::content_type(StrView value) {
  content_type_ = value;
  return *this;
}

HttpRes& HttpRes::body(Str data) {
  body_is_text_ = true;
  body_text_    = move(data);
  return *this;
}

HttpRes& HttpRes::body(Arr<u8> data) {
  body_is_text_ = false;
  body_binary_  = move(data);
  return *this;
}

void HttpRes::send() {
  StrView send_body =
      body_is_text_ ? body_text_ : StrView((char*)body_binary_.data(), body_binary_.size());

  // clang-format off
  fmt(buffer_,
    "HTTP/1.1 ", int(status_), " ", StrView(llhttp_status_name(status_)), "\r\n"
    "Content-Type: ", content_type_, "\r\n"
    "Content-Length: ", send_body.size(), "\r\n"
    "Connection: Keep-Alive\r\n"
    // "Keep-Alive: timeout=5, max=30\r\n" // in seconds
    "\r\n",
    send_body);
  // clang-format on

  mLogDebug("Write: ", buffer_.view().size());
  http_connection_->send(buffer_.view());
}

void HttpRes::reset() {
  buffer_.reset();
  body_text_   = Str();
  body_binary_ = ArrView<u8>();
}
