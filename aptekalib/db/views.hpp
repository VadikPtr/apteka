#pragma once
#include "model.hpp"

struct NavLink {
  Str name;
  Str link;
};

class DbViews {
  DbData&      db_data_;
  Arr<NavLink> nav_links_all_;
  Arr<NavLink> nav_links_pub_;

 public:
  explicit DbViews(DbData& db_data) : db_data_(db_data) {}

  void restore();

  ArrView<const NavLink> nav_links(bool show_hidden) const;
  Arr<const Photo*>      photos_by_category(const Category* category) const;
  Arr<const Photo*>      main_photos(bool show_hidden) const;

 private:
  void restore_nav_links(bool with_hidden, Arr<NavLink>& output);
};
