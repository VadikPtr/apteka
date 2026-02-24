#pragma once
#include <cc/arr.hpp>
#include <cc/str.hpp>
#include <cc/fs.hpp>
#include <cc/bstream.hpp>

struct Category {
  Str id;
  Str name;
};

struct Photo {
  Str       id;
  Str       source_name;
  Str       date_created;
  Category* category;
};

struct NavLink {
  Str name;
  Str link;
};

struct DbData {
  Arr<Category> categories;
  Arr<Photo>    photos;
  Arr<NavLink>  nav_links;

  Category* find_category_by_id(StrView id) {
    for (Category& category : categories) {
      if (category.id == id) {
        return &category;
      }
    }
    return nullptr;
  }
};

class DB {
  DbData data_;

 public:
  void read(const Path& dir);

  const DbData& get() const { return data_; }

 private:
  void restore_nav_links();
};
