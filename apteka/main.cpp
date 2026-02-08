#include "http-server/http-server.hpp"
#include "http-server/ip.hpp"
#include "http-server/content-type.hpp"
#include "http-server/router.hpp"
#include "template-engine/template-engine.hpp"
#include "db/db.hpp"
#include <cc/log.hpp>
#include <cc/prog-opts.hpp>
#include <llhttp.h>
#include <uv.h>

namespace {
  struct Arguments {
    Str  host         = Str("127.0.0.1");
    int  port         = 8080;
    bool serve_static = false;

    void parse(int argc, const char** argv) {
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

  struct AppContext {
    TemplateEngine template_engine = TemplateEngine(Path::to_cwd() / "templates");
    DB             db              = DB::read(Path::to_cwd() / "db");
    Arguments      arguments;

    AppContext(int argc, const char** argv) { arguments.parse(argc, argv); }

    void configure(Router& router);
  };

  class ExampleHandler : public IReqHandler {
    AppContext& app_context;

   public:
    ExampleHandler(AppContext& app_context) : app_context(app_context) {}

    void handle(const HttpReq& req, HttpRes& res) override;
  };

  void AppContext::configure(Router& router) {
    router.add(HTTP_GET, "/", new ExampleHandler(*this));

    if (arguments.serve_static) {
      router.serve_static("/static", Path::to_cwd() / "static");
    }
  }

  void ExampleHandler::handle(const HttpReq& req, HttpRes& res) {
    Str body = app_context.template_engine.render(  //
        "index", "title", "Amogus!");
    res.status(HTTP_STATUS_OK)  //
        .content_type(ContentType::text_html())
        .body(move(body))
        .send();
  }
}  // namespace

int main(int argc, const char** argv) try {
  AppContext app_context(argc, argv);
  Router     router;
  app_context.configure(router);
  HttpServer server = HttpServer(router);
  server.listen(SockAddr(app_context.arguments.host.data(), app_context.arguments.port));
  mLogInfo("Listening on http://", app_context.arguments.host, ":", app_context.arguments.port);
  return uv_run(uv_default_loop(), UV_RUN_DEFAULT);
} catch (const Err& err) {
  mLogCrit("Fatal error: ", err.message());
}
