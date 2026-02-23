#pragma once
#include "app.hpp"
#include "cc/str.hpp"
#include "db.hpp"
#include "http-server/content-type.hpp"
#include <cc/sarr.hpp>
#include <cstring>
#include <type_traits>

class ExampleHandler : public IReqHandler {
  AppContext& app_context;

 public:
  ExampleHandler(AppContext& app_context) : app_context(app_context) {}

  void handle(const HttpReq& req, HttpRes& res) override {
    TemplateInstance& index_tmpl = app_context.template_engine.get("index"_sh);

    Str photos     = build_photos();
    Str navigation = build_navigation();

    Str body = index_tmpl.render(  //
        TemplateKV()               //
            .insert("title"_sh, "Amogus!")
            .insert("navigation"_sh, navigation)
            .insert("photos"_sh, photos));

    res.status(HTTP_STATUS_OK)  //
        .content_type(ContentType::text_html())
        .body(move(body))
        .send();
  }

  Str build_navigation() const {
    StaticFilter<Category, 12> cats_filtered;
    cats_filtered.filter(app_context.db.get_categories().get_all(),
                         [](const Category& cat) { return cat.name != "hidden"; });

    StrBuilder cat_links[12];
    for (size_t i = 0; i < cats_filtered.data.size(); ++i) {
      fmt(cat_links[i], "/category/", cats_filtered.data[i]->id);
    }

    return app_context.template_engine.get("nav"_sh).render_array(
        cats_filtered.data, [cat_idx = 0, &cat_links](const Category* cat, TemplateKV& kv) mutable {
          kv.insert("classes"_sh, "")  //
              .insert("link"_sh, cat_links[cat_idx++].view())
              .insert("name"_sh, cat->name);
        });
  }

  Str build_photos() const {
    StaticFilter<Photo, 30> photos_filtered;
    photos_filtered.filter(app_context.db.get_photos().get_all(),
                           [](const Photo& photo) { return photo.category->name != "hidden"; });

    return app_context.template_engine.get("photo"_sh)
        .render_array(photos_filtered.data, [](const Photo* photo, TemplateKV& kv) {
          kv.insert("id"_sh, photo->id)
              .insert("source_name"_sh, photo->source_name)
              .insert("date_created"_sh, photo->date_created)
              .insert("category"_sh, photo->category->name);
        });
  }

  template <typename T, size_t capacity>
  struct StaticFilter {
    T*       data_arr[capacity] = {nullptr};
    SArr<T*> data               = SArr<T*>(data_arr);

    template <typename TFunc>
    void filter(Arr<T>& source, TFunc&& func) {
      for (size_t i = 0; i < source.size(); i++) {
        T& element = source[i];
        if (func(element)) {
          if constexpr (std::is_pointer_v<decltype(element)>) {
            data.push(element);
          } else {
            data.push(&element);
          }
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
