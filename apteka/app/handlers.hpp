#pragma once
#include "app.hpp"
#include "http-server/content-type.hpp"

class ExampleHandler : public IReqHandler {
  AppContext& app_context;

 public:
  ExampleHandler(AppContext& app_context) : app_context(app_context) {}

  void handle(const HttpReq& req, HttpRes& res) override {
    TemplateInstance& photo_tmpl = app_context.template_engine.get("photo"_sh);
    TemplateInstance& index_tmpl = app_context.template_engine.get("index"_sh);

    Arr<Photo>& photos = app_context.db.get_photos().get_all();

    Str body = index_tmpl.render(
        TemplateKV()
            .insert("title"_sh, "Amogus!")
            .insert("photos"_sh,
                    photo_tmpl.render_array(photos, [](const Photo& photo, TemplateKV& kv) {
                      kv.insert("id"_sh, photo.id)
                          .insert("source_name"_sh, photo.source_name)
                          .insert("date_created"_sh, photo.date_created)
                          .insert("category"_sh, photo.category->name);
                    })));

    res.status(HTTP_STATUS_OK)  //
        .content_type(ContentType::text_html())
        .body(move(body))
        .send();
  }
};
