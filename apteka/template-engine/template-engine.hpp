#pragma once
#include <cc/sdict.hpp>
#include <cc/dict.hpp>
#include <cc/str.hpp>
#include <cc/fs.hpp>

using TemplateKV = SDict<StrView, StrView>;

class TemplateInstance {
  Str    content_;
  Path   source_path_;
  size_t max_expanded_size_;

 public:
  explicit TemplateInstance(Str content, Path source_path);

  Str    expand(const TemplateKV& values);
  void   expand(StrBuilder& builder, const TemplateKV& values);
  size_t get_capacity() const { return max_expanded_size_; }
};

class TemplateEngine {
  Dict<StrHash, TemplateInstance> instances_;

 public:
  TemplateEngine(const Path& base_dir);

  void insert(StrHash name, TemplateInstance instance);
  Str  render_dict(StrView name, const TemplateKV& values);

  template <typename... Args>
  Str render(StrView name, Args&&... args) {
    TemplateKV dict;
    dict.reserve(sizeof...(Args) / 2);
    fill_dict(dict, std::forward<Args>(args)...);
    return render_dict(name, dict);
  }

 private:
  template <typename... Args>
  static void fill_dict(TemplateKV& dict, Args&&... args);

  static void fill_dict(TemplateKV&) {}

  template <typename Key, typename Value, typename... Rest>
  static void fill_dict(TemplateKV& dict, Key&& key, Value&& value, Rest&&... rest) {
    dict.insert(std::forward<Key>(key), std::forward<Value>(value));
    fill_dict(dict, std::forward<Rest>(rest)...);
  }
};
