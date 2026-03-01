#include "app.hpp"
#include "handlers.hpp"
#include "auth.hpp"
#include <cc/prog-opts.hpp>

void Arguments::parse(int argc, const char** argv) {
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

AppContext::AppContext(int argc, const char** argv) {
  arguments.parse(argc, argv);
  template_engine.parse(Path::to_cwd() / "templates");
  db.read(Path::to_cwd() / "db");
  auth::init();
}

void AppContext::configure(Router& router) {
  router.add(HTTP_GET, "/favicon.ico", new RedirectHandler("/static/favicon.ico"_s));
  router.add(HTTP_GET, "/", new MainHandler(*this));
  router.add(HTTP_GET, "/category", new CategoryHandler(*this));

  if (arguments.serve_static) {
    router.serve_static("/static"_s, Path::to_cwd() / "static");
  }
}
