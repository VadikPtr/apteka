#include "db.hpp"
#include <cc/bstream.hpp>

void Db::read(const Path& dir) {
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
    photo.category     = find_category_by_id(category_id);
  }

  views_.restore();
}

Category* Db::find_category_by_id(StrView id) {
  for (Category& category : data_.categories) {
    if (category.id == id) {
      return &category;
    }
  }
  return nullptr;
}
