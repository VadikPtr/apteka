#include "req-parser.hpp"
#include "cc/log.hpp"
#include "llhttp.h"

namespace {
  llhttp_settings_t get_http_parser_settings() {
    llhttp_settings_t settings;
    llhttp_settings_init(&settings);

    settings.on_url = [](llhttp_t* http, const char* at, size_t length) -> int {
      ReqParser* self = (ReqParser*)http->data;
      return self->get_builder().append_url(StrView(at, length)) ? 0 : -1;
    };

    settings.on_header_field = [](llhttp_t* http, const char* at, size_t length) -> int {
      ReqParser* self = (ReqParser*)http->data;
      return self->get_builder().append_header_field(StrView(at, length)) ? 0 : -1;
    };

    settings.on_header_value = [](llhttp_t* http, const char* at, size_t length) -> int {
      ReqParser* self = (ReqParser*)http->data;
      return self->get_builder().append_header_value(StrView(at, length)) ? 0 : -1;
    };

    settings.on_body = [](llhttp_t* http, const char* at, size_t length) -> int {
      // auto reader = reinterpret_cast<HttpRequestParser*>(http->data);
      // reader->body_parser_.add_buffer(at, length);
      // g_log->debug("[llhttp] on body {} bytes", length);
      return 0;
    };

    settings.on_message_complete = [](llhttp_t* http) -> int {
      ReqParser* self = (ReqParser*)http->data;
      self->get_builder().set_method((llhttp_method_t)http->method);
      self->set_parsing_done();
      return 0;
    };

    settings.on_header_value_complete = [](llhttp_t* http) -> int {
      ReqParser* self = (ReqParser*)http->data;
      self->get_builder().header_done();
      return 0;
    };

    return settings;
  };

  llhttp_settings_t g_parser_settings = get_http_parser_settings();
}  // namespace

ReqParser::ReqParser() {
  llhttp_init(&parser, HTTP_REQUEST, &g_parser_settings);
  parser.data = this;
}

void HttpReqBuilder::reset() {
  url_.reset();
  header_field_.reset();
  header_value_.reset();
  headers_ = Dict<StrHash, Str>();
}

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
  // mLogDebug("Header [", key, "]: ", val);
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

bool ReqParser::handle(StrView data) {
  llhttp_errno_t err = llhttp_execute(&parser, data.data(), data.size());
  if (err != HPE_OK) {
    mLogWarn("ReqParser parse error: ", StrView(llhttp_errno_name(err)), " ",
             StrView(parser.reason));
    return false;
  }
  return true;
}

void ReqParser::reset() {
  llhttp_reset(&parser);
  req_builder_.reset();
  done_ = false;
}
