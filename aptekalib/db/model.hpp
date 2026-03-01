#pragma once
#include <cc/arr.hpp>
#include <cc/str.hpp>

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

struct DbData {
  Arr<Category> categories;
  Arr<Photo>    photos;
};
