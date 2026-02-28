#include "views.hpp"
#include <cc/fmt.hpp>
#include "db/model.hpp"

namespace {
  template <typename TOut, typename TIn, typename TFunc>
  Arr<TOut> filter(TIn& source, TFunc&& func) {
    size_t result_size = 0;
    for (size_t i = 0; i < source.size(); i++) {
      if (func(source[i])) {
        ++result_size;
      }
    }

    Arr<TOut> result    = Arr<TOut>(result_size);
    size_t    out_index = 0;

    for (size_t i = 0; i < source.size(); i++) {
      if (not func(source[i])) {
        continue;
      }
      if constexpr (std::is_pointer_v<TOut> && not std::is_pointer_v<TIn>) {
        result[out_index] = &source[i];
      } else {
        result[out_index] = source[i];
      }
      ++out_index;
    }

    return result;
  }

  template <typename TOut, typename TIn, typename TFunc>
  Arr<TOut> map(const TIn& source, TFunc&& mapper) {
    Arr<TOut> result = Arr<TOut>(source.size());
    for (size_t i = 0; i < source.size(); i++) {
      result[i] = mapper(source[i]);
    }
    return result;
  }
}  // namespace


void DbViews::restore() {
  restore_nav_links(true, nav_links_all_);
  restore_nav_links(false, nav_links_pub_);
}

void DbViews::restore_nav_links(bool with_hidden, Arr<NavLink>& output) {
  Arr<const Category*> cats = filter<const Category*>(
      db_data_.categories,
      [with_hidden](const Category& cat) { return with_hidden || cat.name != "hidden"; });

  output = map<NavLink>(cats, [](const Category* cat) {
    return NavLink{
        .name = cat->name,
        .link = fmt("/category?id=", cat->id),
    };
  });
}

ArrView<const NavLink> DbViews::nav_links(bool show_hidden) const {
  if (show_hidden) {
    return ArrView<const NavLink>(nav_links_all_.data(), nav_links_all_.size());
  } else {
    return ArrView<const NavLink>(nav_links_pub_.data(), nav_links_pub_.size());
  }
}

Arr<const Photo*> DbViews::photos_by_category(const Category* category) const {
  return filter<const Photo*>(db_data_.photos, [category](const Photo& photo) {
    return photo.category->id == category->id;
  });
}

Arr<const Photo*> DbViews::main_photos(bool show_hidden) const {
  return filter<const Photo*>(db_data_.photos, [show_hidden](const Photo& photo) {
    return show_hidden || photo.category->name != "hidden";
  });
}
