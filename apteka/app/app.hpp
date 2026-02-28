#pragma once
#include "app/template-engine.hpp"
#include "app/page-render.hpp"
#include "db/db.hpp"
#include "http-server/router.hpp"

struct Arguments {
  Str  host         = Str("127.0.0.1");
  int  port         = 8080;
  bool serve_static = false;

  void parse(int argc, const char** argv);
};

struct AppContext {
  Arguments      arguments;
  TemplateEngine template_engine;
  Db             db;
  PageRender     page_render = PageRender(template_engine, db);

  AppContext(int argc, const char** argv) {
    arguments.parse(argc, argv);
    template_engine.parse(Path::to_cwd() / "templates");
    db.read(Path::to_cwd() / "db");
  }

  void configure(Router& router);
};
