#pragma once
#include <cc/arr.hpp>
#include <cc/str.hpp>
#include <cc/fs.hpp>
#include <cc/bstream.hpp>
#include "cc/log.hpp"

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

  static DB read(const Path& dir) {
    Arr<u8>       categories_data   = (dir / "categories.bin").read_bytes();
    BStreamReader categories_reader = BStreamReader(categories_data);
    u32           categories_count  = categories_reader.read_u32();
    Arr<Category> categories_arr    = Arr<Category>(categories_count);
    for (Category& category : categories_arr) {
      category.id   = categories_reader.read_str();
      category.name = categories_reader.read_str();
    }
    Categories categories = Categories(categories_arr);

    Arr<u8>       photos_data   = (dir / "photos.bin").read_bytes();
    BStreamReader photos_reader = BStreamReader(photos_data);
    u32           photos_count  = photos_reader.read_u32();
    Arr<Photo>    photos_arr    = Arr<Photo>(photos_count);
    for (Photo& photo : photos_arr) {
      photo.id           = categories_reader.read_str();
      photo.source_name  = categories_reader.read_str();
      photo.date_created = categories_reader.read_str();
      Str category_id    = categories_reader.read_str();
      photo.category     = categories.find_by_id(category_id);
    }
    Photos photos = Photos(photos_arr);

    return DB(move(categories), move(photos));
  }

  Categories& get_categories();
  Photos&     get_photos();
};
