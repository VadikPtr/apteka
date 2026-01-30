#include "req-parser.hpp"
#include "cc/log.hpp"
#include "llhttp.h"

namespace {
  llhttp_settings_t get_http_parser_settings() {
    llhttp_settings_t settings = llhttp_settings_t{};
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
      HttpReq req = self->get_builder().build();
      mLogDebug("HTTP ", StrView(llhttp_method_name(req.method)), " ", req.url);
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

  llhttp_settings_t g_parser_settings{get_http_parser_settings()};
}  // namespace

ReqParser::ReqParser() {
  llhttp_init(&parser, HTTP_REQUEST, &g_parser_settings);
  parser.data = this;
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
