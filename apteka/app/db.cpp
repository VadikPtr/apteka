#include "db.hpp"

DB DB::read(const Path& dir) {
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
    photo.id           = photos_reader.read_str();
    photo.source_name  = photos_reader.read_str();
    photo.date_created = photos_reader.read_str();
    Str category_id    = photos_reader.read_str();
    photo.category     = categories.find_by_id(category_id);
  }
  Photos photos = Photos(photos_arr);

  return DB(move(categories), move(photos));
}
