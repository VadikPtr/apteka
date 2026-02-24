#pragma once
#include "app/template-engine.hpp"
#include "app/db.hpp"
#include "http-server/http-req.hpp"
#include <cc/sarr.hpp>


template <typename T, size_t capacity>
struct StaticFilter {
  const T*       data_arr[capacity] = {nullptr};
  SArr<const T*> data               = SArr<const T*>(data_arr);

  template <typename TFunc>
  void filter(const Arr<T>& source, TFunc&& func) {
    for (size_t i = 0; i < source.size(); i++) {
      const T& element = source[i];
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

template <typename T>
class DynamicFilter {
  Arr<const T*> data_;

 public:
  ArrView<const T*> view;

  template <typename TFunc>
  void filter(const Arr<T>& source, TFunc&& func) {
    for (size_t i = 0; i < source.size(); i++) {
      const T& element = source[i];

      if (func(element)) {
        size_t size     = view.size();
        size_t capacity = data_.size();

        if (size + 1 >= capacity) {
          capacity = mMax(capacity * 2, 32);
          data_.resize(capacity);
        }

        if constexpr (std::is_pointer_v<decltype(element)>) {
          data_[size] = element;
        } else {
          data_[size] = &element;
        }

        view = data_.sub(0, size + 1);
      }
    }
  }
};

struct PageRender {
  TemplateEngine& template_engine;
  DB&             db;

  Str build_main() const {
    TemplateInstance& index_tmpl = template_engine.get("index"_sh);

    Str photos = build_photos([](const Photo& photo) { return photo.category->name != "hidden"; });
    Str navigation = build_navigation();
    return index_tmpl.render(  //
        TemplateKV()
            .insert("title"_sh, "")
            .insert("navigation"_sh, navigation)
            .insert("photos"_sh, photos));
  }

  Str build_category(const Category* category) const {
    TemplateInstance& index_tmpl = template_engine.get("index"_sh);
    Str               photos =
        build_photos([category](const Photo& photo) { return photo.category->id == category->id; });
    Str navigation = build_navigation();
    return index_tmpl.render(  //
        TemplateKV()
            .insert("title"_sh, category->name)
            .insert("navigation"_sh, navigation)
            .insert("photos"_sh, photos));
  }

  Str build_navigation() const {
    return template_engine.get("nav"_sh).render_array(
        db.get().nav_links, [](const NavLink& nav_link, TemplateKV& kv) mutable {
          bool active = nav_link.link == g_http_context.req->full_url;
          kv.insert("classes"_sh, active ? "active" : "")  //
              .insert("link"_sh, nav_link.link)
              .insert("name"_sh, nav_link.name);
        });
  }

  template <typename TFilter>
  Str build_photos(TFilter&& filter) const {
    DynamicFilter<Photo> photos_filtered;
    photos_filtered.filter(db.get().photos, filter);
    return template_engine.get("photo"_sh)
        .render_array(photos_filtered.view, [](const Photo* photo, TemplateKV& kv) {
          kv.insert("id"_sh, photo->id)
              .insert("source_name"_sh, photo->source_name)
              .insert("date_created"_sh, photo->date_created)
              .insert("category"_sh, photo->category->name);
        });
  }
};
