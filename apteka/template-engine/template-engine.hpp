#pragma once
#include <cc/sdict.hpp>
#include <cc/dict.hpp>
#include <cc/str.hpp>
#include <cc/fs.hpp>

class TemplateInstance {
  Str    content_;
  Path   source_path_;
  size_t max_expanded_size_;

 public:
  explicit TemplateInstance(Str content, Path source_path);

  Str    expand(const SDict<StrView, Str>& values);
  void   expand(StrBuilder& builder, const SDict<StrView, Str>& values);
  size_t get_capacity() const { return max_expanded_size_; }
};

class TemplateEngine {
  Dict<StrHash, TemplateInstance> instances_;

 public:
  TemplateEngine(const Path& base_dir);

  void insert(StrHash name, TemplateInstance instance);
  Str  render(StrView name, const SDict<StrView, Str>& values);
};
