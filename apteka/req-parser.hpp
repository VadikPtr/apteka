#pragma once
#include "cc/str-view.hpp"
#include "http-req.hpp"
#include "llhttp.h"

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
  void    reset();
  HttpReq build();
  void    set_method(llhttp_method method);
  bool    append_url(StrView val);
  bool    append_header_field(StrView val);
  bool    append_header_value(StrView val);
  void    header_done();
  bool    parse_method(StrView method_name);
};

class ReqParser {
  llhttp_t       parser = llhttp_t{};
  HttpReqBuilder req_builder_;
  bool           done_ = false;

 public:
  ReqParser();

  bool handle(StrView data);
  void reset();

  HttpReqBuilder& get_builder() { return req_builder_; }
  bool            is_parsing_done() { return done_; }
  void            set_parsing_done() { done_ = true; }
};
