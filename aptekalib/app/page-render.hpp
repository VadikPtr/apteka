#pragma once
#include "app/template-engine.hpp"
#include "db/db.hpp"
#include "http-server/http-req.hpp"
#include <cc/sarr.hpp>
#include <ctime>


struct PageRender {
  TemplateEngine& template_engine;
  Db&             db;

  Str build_main() const {
    TemplateInstance& index_tmpl = template_engine.get("index"_sh);
    Str               photos     = build_photos(db.views().main_photos(false));
    Str               navigation = build_navigation();
    return index_tmpl.render(  //
        TemplateKV()
            .insert("title"_sh, "Фотографии Вадима")
            .insert("year"_sh, year())
            .insert("navigation"_sh, navigation)
            .insert("photos"_sh, photos));
  }

  Str build_category(const Category* category) const {
    TemplateInstance& index_tmpl = template_engine.get("index"_sh);
    Str               photos     = build_photos(db.views().photos_by_category(category));
    Str               navigation = build_navigation();
    return index_tmpl.render(  //
        TemplateKV()
            .insert("title"_sh, category->name)
            .insert("year"_sh, year())
            .insert("navigation"_sh, navigation)
            .insert("photos"_sh, photos));
  }

  Str build_navigation() const {
    return template_engine.get("nav"_sh).render_array(
        db.views().nav_links(false), [](const NavLink& nav_link, TemplateKV& kv) mutable {
          bool active = nav_link.link == g_http_context.req->full_url;
          kv.insert("classes"_sh, active ? "active" : "")  //
              .insert("link"_sh, nav_link.link)
              .insert("name"_sh, nav_link.name);
        });
  }

  Str build_photos(Arr<const Photo*> view) const {
    return template_engine.get("photo"_sh)
        .render_array(view, [](const Photo* photo, TemplateKV& kv) {
          kv.insert("id"_sh, photo->id)
              .insert("source_name"_sh, photo->source_name)
              .insert("date_created"_sh, photo->date_created)
              .insert("category"_sh, photo->category->name);
        });
  }

 private:
  static StrView year() {
    std::time_t       t          = std::time(nullptr);
    std::tm*          local_time = std::localtime(&t);
    static StrBuilder builder    = StrBuilder();
    builder.reset();
    fmt(builder, local_time->tm_year + 1900);
    return builder.view();
  }
};
