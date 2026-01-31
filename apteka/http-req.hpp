#pragma once
#include "cc/arr.hpp"
#include "cc/str.hpp"
#include "cc/dict.hpp"
#include "llhttp.h"

class HttpConnection;

struct HttpReq {
  Str                url;
  Dict<StrHash, Str> headers;
  llhttp_method      method;
};

class HttpRes {
  StrBuilder      buffer_;
  HttpConnection* http_connection_;
  StrView         content_type_;
  Str             body_text_;
  Arr<u8>         body_binary_;
  llhttp_status   status_;
  bool            body_is_text_;

 public:
  HttpRes(HttpConnection* http_connection);

  HttpRes& status(llhttp_status value);
  HttpRes& content_type(StrView value);
  HttpRes& body(Str data);
  HttpRes& body(Arr<u8> data);

  void send();
};
