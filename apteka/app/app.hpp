#pragma once
#include "app/template-engine.hpp"
#include "app/db.hpp"
#include "app/page-render.hpp"
#include "http-server/router.hpp"

struct Arguments {
  Str  host         = Str("127.0.0.1");
  int  port         = 8080;
  bool serve_static = false;

  void parse(int argc, const char** argv);
};

struct AppContext {
  TemplateEngine template_engine = TemplateEngine(Path::to_cwd() / "templates");
  DB             db;
  PageRender     page_render = PageRender(template_engine, db);
  Arguments      arguments;

  AppContext(int argc, const char** argv) {
    arguments.parse(argc, argv);
    db.read(Path::to_cwd() / "db");
  }

  void configure(Router& router);
};
