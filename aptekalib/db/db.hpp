#pragma once
#include "views.hpp"
#include "model.hpp"
#include <cc/fs.hpp>

class Db {
  DbData  data_;
  DbViews views_ = DbViews(data_);

 public:
  void read(const Path& dir);

  const DbData&  data() const { return data_; }
  const DbViews& views() const { return views_; }

  Category* find_category_by_id(StrView id);
};
