#include "cc/log.hpp"
#include "http-server.hpp"
#include "ip.hpp"
#include "llhttp.h"
#include "mime.hpp"
#include "uv.h"
#include "router.hpp"

#define mBindAddr "127.0.0.1"
// #define mBindAddr "0.0.0.0"

namespace {
  class ExampleHandler : public IReqHandler {
    void handle(const HttpReq& req, HttpRes& res) override {
      res.status(HTTP_STATUS_OK)
          .content_type(ContentType::text_plain())
          .body(Str("Hello, wrold!"))
          .send();
    }
  };
}  // namespace

int main() {
  Router router;
  router.add(HTTP_GET, "/", new ExampleHandler());

  HttpServer server(router);
  server.listen(SockAddr(mBindAddr, 8080));
  mLogInfo("Listening on http://" mBindAddr ":8080");

  return uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
