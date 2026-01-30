#pragma once
#include "cc/str-view.hpp"
#include "http-req.hpp"
#include "llhttp.h"

class ReqParser {
  llhttp_t       parser = llhttp_t{};
  HttpReqBuilder req_builder_;
  bool           done_ = false;

 public:
  ReqParser();

  bool handle(StrView data);

  HttpReqBuilder& get_builder() { return req_builder_; }
  bool            is_parsing_done() { return done_; }
  void            set_parsing_done() { done_ = true; }
};
