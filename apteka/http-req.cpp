#include "http-req.hpp"
#include "cc/log.hpp"

HttpReq HttpReqBuilder::build() {
  HttpReq req;
  req.url     = url_.to_string();
  req.headers = move(headers_);
  req.method  = method_;
  return req;
}

void HttpReqBuilder::set_method(llhttp_method method) {
  method_ = method;
}

bool HttpReqBuilder::append_url(StrView val) {
  if (url_.view().size() + val.size() > g_max_url_size) {
    return false;
  }
  url_.append(val);
  return true;
}

bool HttpReqBuilder::append_header_field(StrView val) {
  if (header_field_.view().size() + val.size() > g_max_header_field_size) {
    return false;
  }
  header_field_.append(val);
  return true;
}

bool HttpReqBuilder::append_header_value(StrView val) {
  if (header_value_.view().size() + val.size() > g_max_header_value_size) {
    return false;
  }
  header_value_.append(val);
  return true;
}

void HttpReqBuilder::header_done() {
  StrView key = header_field_.view().to_lower();
  StrView val = header_value_.view();
  mLogDebug("Header [", key, "]: ", val);
  headers_.insert(StrHash(key), Str(val));
  header_field_.reset();
  header_value_.reset();
}

bool HttpReqBuilder::parse_method(StrView method_name) {
  mLogDebug("Method: ", method_name);

#define mExpandMethodCheck(code, name, ...) \
  if (method_name == #name) {               \
    method_ = HTTP_##name;                  \
    return true;                            \
  }

  HTTP_METHOD_MAP(mExpandMethodCheck);

#undef mExpandMethodCheck

  mLogDebug("Unknown method: ", method_name);
  return false;
}
