#pragma once
#include "app/template-engine.hpp"
#include "app/page-render.hpp"
#include "app/auth.hpp"
#include "db/db.hpp"
#include "http-server/router.hpp"

struct Arguments {
  Str  host         = Str("127.0.0.1");
  int  port         = 8080;
  bool serve_static = false;

  void parse(int argc, const char** argv);
};

struct AppConfig {
  Str admin_password_hash;
};

struct AppContext {
  Arguments      arguments;
  TemplateEngine template_engine;
  Db             db;
  Auth           auth;
  PageRender     page_render = PageRender(template_engine, db);

  AppContext(int argc, const char** argv);

  void configure(Router& router);
};
