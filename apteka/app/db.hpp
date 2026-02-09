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

class Categories {
  Arr<Category> data_;

 public:
  Categories(Arr<Category> data) : data_(move(data)) {}

  Arr<Category>& get_all() { return data_; }

  Category* find_by_id(StrView id) {
    for (Category& category : data_) {
      if (category.id == id) {
        return &category;
      }
    }
    return nullptr;
  }
};

class Photos {
  Arr<Photo> data_;

 public:
  Photos(Arr<Photo> data) : data_(move(data)) {}

  Arr<Photo>& get_all() { return data_; }
};

class DB {
  Categories categories_;
  Photos     photos_;

 public:
  DB(Categories categories, Photos photos) : categories_(move(categories)), photos_(move(photos)) {}

  static DB read(const Path& dir);

  Categories& get_categories() { return categories_; }
  Photos&     get_photos() { return photos_; }
};
