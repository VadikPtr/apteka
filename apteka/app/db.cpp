#include "db.hpp"

void DB::read(const Path& dir) {
  Arr<u8>       categories_data   = (dir / "categories.bin").read_bytes();
  BStreamReader categories_reader = BStreamReader(categories_data);
  u32           categories_count  = categories_reader.read_u32();
  data_.categories                = Arr<Category>(categories_count);
  for (Category& category : data_.categories) {
    category.id   = categories_reader.read_str();
    category.name = categories_reader.read_str();
  }

  Arr<u8>       photos_data   = (dir / "photos.bin").read_bytes();
  BStreamReader photos_reader = BStreamReader(photos_data);
  u32           photos_count  = photos_reader.read_u32();
  data_.photos                = Arr<Photo>(photos_count);
  for (Photo& photo : data_.photos) {
    photo.id = photos_reader.read_str();
    photo.id.to_lower();
    photo.source_name  = photos_reader.read_str();
    photo.date_created = photos_reader.read_str();
    Str category_id    = photos_reader.read_str();
    photo.category     = data_.find_category_by_id(category_id);
  }

  restore_nav_links();
}

const Category* DB::find_category_by_id(StrView id) const {
  for (const Category& category : data_.categories) {
    if (category.id == id) {
      return &category;
    }
  }
  return nullptr;
}

void DB::restore_nav_links() {
  size_t visible_cat_count = 0;
  for (const Category& cat : data_.categories) {
    if (cat.name != "hidden") {
      ++visible_cat_count;
    }
  }
  data_.nav_links = Arr<NavLink>(visible_cat_count);
  size_t link_idx = 0;
  for (const Category& cat : data_.categories) {
    if (cat.name != "hidden") {
      data_.nav_links[link_idx].name = cat.name;
      data_.nav_links[link_idx].link = fmt("/category?id=", cat.id);
      ++link_idx;
    }
  }
}
