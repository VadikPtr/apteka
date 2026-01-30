#pragma once
#include "cc/str.hpp"
#include "cc/dict.hpp"
#include "llhttp.h"

struct HttpReq {
  Str                url;
  Dict<StrHash, Str> headers;
  llhttp_method      method;
};

class HttpReqBuilder {
  StrBuilder         url_;
  StrBuilder         header_field_;
  StrBuilder         header_value_;
  Dict<StrHash, Str> headers_;
  llhttp_method      method_ = HTTP_GET;

  static constexpr const size_t g_max_url_size          = 128;
  static constexpr const size_t g_max_header_field_size = 128;
  static constexpr const size_t g_max_header_value_size = 2048;

 public:
  HttpReq build();
  void    set_method(llhttp_method method);
  bool    append_url(StrView val);
  bool    append_header_field(StrView val);
  bool    append_header_value(StrView val);
  void    header_done();
  bool    parse_method(StrView method_name);
};
