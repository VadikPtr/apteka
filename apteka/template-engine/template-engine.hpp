#pragma once
#include <cc/dict.hpp>
#include <cc/str.hpp>
#include <cc/fs.hpp>

inline constexpr size_t g_max_template_params = 16;

struct TemplateKV {
  StrHash keys[g_max_template_params];
  StrView values[g_max_template_params];
  size_t  size = 0;

  TemplateKV&    insert(StrHash hash, StrView value);
  const StrView* find(StrHash needle) const;
};

enum class TemplateTokenType {
  Text,
  Substitution,
};

struct TemplateToken {
  Str               text;
  StrHash           key;
  TemplateTokenType type;
};

class TemplateInstance {
  Path               source_path_;
  size_t             max_rendered_size_;
  Arr<TemplateToken> tokens_;

 public:
  explicit TemplateInstance(const Path& source_path);

  Str  render(const TemplateKV& values);
  void render(StrBuilder& builder, const TemplateKV& values);

  template <typename TArr, typename TFunc>
  Str render_array(const TArr& arr, TFunc&& func) {
    StrBuilder builder;
    builder.ensure_capacity(max_rendered_size_ * arr.size());
    for (auto& item : arr) {
      TemplateKV kv;
      func(item, kv);
      render(builder, kv);
    }
    return builder.to_string();
  }

 private:
  void parse_template();
};

class TemplateEngine {
  Dict<StrHash, TemplateInstance> instances_;

 public:
  TemplateEngine(const Path& base_dir);

  TemplateInstance& get(StrHash name);
  void              insert(StrHash name, TemplateInstance instance);
};
