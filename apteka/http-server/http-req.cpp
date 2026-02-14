#include "http-req.hpp"
#include "http-connection.hpp"
#include "content-type.hpp"
#include "llhttp.h"
#include <cc/str.hpp>
#include <cc/fmt.hpp>

void HttpReq::reset() {
  url     = Str();
  headers = Dict<StrHash, Str>();
}

HttpRes::HttpRes(HttpConnection& http_connection) : http_connection_(http_connection) {}

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

  http_connection_.send(buffer_.view());
}

void HttpRes::send_basic(llhttp_status status) {
  StrBuilder send_body;
  fmt(send_body, int(status), " ", StrView(llhttp_status_name(status)));

  // clang-format off
  fmt(buffer_,
    "HTTP/1.1 ", send_body.view(), "\r\n"
    "Content-Type: ", ContentType::text_plain(), "\r\n"
    "Content-Length: ", send_body.view().size(), "\r\n"
    "Connection: Keep-Alive\r\n"
    "\r\n",
    send_body.view());
  // clang-format on

  http_connection_.send(buffer_.view());
}

void HttpRes::send_permanent_redirect(StrView location) {
  StrBuilder send_body;
  fmt(send_body, int(HTTP_STATUS_PERMANENT_REDIRECT), " ",
      StrView(llhttp_status_name(HTTP_STATUS_PERMANENT_REDIRECT)));

  // clang-format off
  fmt(buffer_,
    "HTTP/1.1 ", send_body.view(), "\r\n"
    "Content-Length: 0\r\n"
    "Location: ", location ,"\r\n"
    "Connection: Keep-Alive\r\n"
    "\r\n");
  // clang-format on

  http_connection_.send(buffer_.view());
}

void HttpRes::reset() {
  buffer_.reset();
  body_text_   = Str();
  body_binary_ = ArrView<u8>();
}
