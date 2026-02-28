#pragma once
#include "app.hpp"
#include "http-server/content-type.hpp"
#include <llhttp.h>
#include <cc/str.hpp>
#include <cc/sarr.hpp>

class MainHandler : public IReqHandler {
  AppContext& app_context;

 public:
  MainHandler(AppContext& app_context) : app_context(app_context) {}

  void handle(HttpReq& req, HttpRes& res) override {
    res.status(HTTP_STATUS_OK)  //
        .content_type(ContentType::text_html())
        .body(app_context.page_render.build_main())
        .send();
  }
};

class CategoryHandler : public IReqHandler {
  AppContext& app_context;

 public:
  CategoryHandler(AppContext& app_context) : app_context(app_context) {}

  void handle(HttpReq& req, HttpRes& res) override {
    auto id = req.query.find("id"_sh);
    if (id == req.query.end()) {
      return res.send_basic(HTTP_STATUS_BAD_REQUEST);
    }

    const Category* category = app_context.db.find_category_by_id(id.value());
    if (not category) {
      return res.send_basic(HTTP_STATUS_NOT_FOUND);
    }

    res.status(HTTP_STATUS_OK)  //
        .content_type(ContentType::text_html())
        .body(app_context.page_render.build_category(category))
        .send();
  }
};

class RedirectHandler : public IReqHandler {
  Str location_;

 public:
  RedirectHandler(Str location) : location_(move(location)) {}

  void handle(HttpReq&, HttpRes& res) override { res.send_permanent_redirect(location_); }
};
