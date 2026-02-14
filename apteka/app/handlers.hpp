#pragma once
#include "app.hpp"
#include "http-server/content-type.hpp"
#include <cc/sarr.hpp>

class ExampleHandler : public IReqHandler {
  AppContext& app_context;

 public:
  ExampleHandler(AppContext& app_context) : app_context(app_context) {}

  void handle(const HttpReq& req, HttpRes& res) override {
    TemplateInstance& photo_tmpl = app_context.template_engine.get("photo"_sh);
    TemplateInstance& index_tmpl = app_context.template_engine.get("index"_sh);

    Arr<Photo>&     db_photos = app_context.db.get_photos().get_all();
    PhotoFilter<30> photos_filtered;
    photos_filtered.filter(app_context.db.get_photos().get_all(),
                           [](const Photo& photo) { return photo.category->name != "hidden"; });

    Str photos =
        photo_tmpl.render_array(photos_filtered.data, [](const Photo* photo, TemplateKV& kv) {
          kv.insert("id"_sh, photo->id)
              .insert("source_name"_sh, photo->source_name)
              .insert("date_created"_sh, photo->date_created)
              .insert("category"_sh, photo->category->name);
        });

    Str body = index_tmpl.render(  //
        TemplateKV()               //
            .insert("title"_sh, "Amogus!")
            .insert("photos"_sh, move(photos)));

    res.status(HTTP_STATUS_OK)  //
        .content_type(ContentType::text_html())
        .body(move(body))
        .send();
  }

  template <size_t capacity>
  struct PhotoFilter {
    Photo*       data_arr[capacity] = {nullptr};
    SArr<Photo*> data               = SArr<Photo*>(data_arr);

    template <typename TFunc>
    void filter(Arr<Photo>& source, TFunc&& func) {
      for (size_t i = 0; i < source.size(); i++) {
        Photo& photo = source[i];
        if (func(photo)) {
          data.push(&photo);
          if (data.size() == data.capacity()) {
            break;
          }
        }
      }
    }
  };
};

class RedirectHandler : public IReqHandler {
  Str location_;

 public:
  RedirectHandler(Str location) : location_(move(location)) {}

  void handle(const HttpReq&, HttpRes& res) override { res.send_permanent_redirect(location_); }
};
