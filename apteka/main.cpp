#include "cc/log.hpp"
#include "cc/prog-opts.hpp"
#include "http-server.hpp"
#include "ip.hpp"
#include "llhttp.h"
#include "mime.hpp"
#include "uv.h"
#include "router.hpp"

namespace {
  class ExampleHandler : public IReqHandler {
    void handle(const HttpReq& req, HttpRes& res) override {
      res.status(HTTP_STATUS_OK)
          .content_type(ContentType::text_plain())
          .body(Str("Hello, wrold!"))
          .send();
    }
  };

  struct Arguments {
    Str  host;
    int  port;
    bool serve_static = false;

    void parse(int argc, const char** argv) {
      // defaults:
      host = "127.0.0.1";
      port = 8080;

      ProgOpts::add(ProgOpts::ArgumentStr{
          .long_name     = "host",
          .short_name    = 'H',
          .help_argument = "Bind IP address. Default: 127.0.0.1",
          .value         = host,
          .flags         = ProgOpts::Optional,
      });
      ProgOpts::add(ProgOpts::ArgumentS32{
          .long_name     = "port",
          .short_name    = 'p',
          .help_argument = "Bind port number. Default: 8080",
          .value         = port,
          .flags         = ProgOpts::Optional,
      });
      ProgOpts::add(ProgOpts::Flag{
          .long_name     = "serve-static",
          .short_name    = 's',
          .help_argument = "Serve static",
          .value         = serve_static,
          .flags         = ProgOpts::Optional,
      });
      ProgOpts::parse(argc, argv);

      host.null_terminate();
    }
  };
}  // namespace

int main(int argc, const char** argv) {
  Arguments arguments;
  arguments.parse(argc, argv);

  Router router;
  router.add(HTTP_GET, "/", new ExampleHandler());

  if (arguments.serve_static) {
    router.serve_static("/static", Path::to_cwd() / "static");
  }

  HttpServer server = HttpServer(router);
  server.listen(SockAddr(arguments.host.data(), arguments.port));
  mLogInfo("Listening on http://", arguments.host, ":", arguments.port);

  return uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
