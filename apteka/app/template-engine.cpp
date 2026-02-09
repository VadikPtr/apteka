#include "template-engine.hpp"
#include <cc/log.hpp>

namespace {
  struct TemplateVisitor : public IFileVisitor {
    const Path&     base_dir;
    TemplateEngine& engine;

    TemplateVisitor(const Path& base_dir, TemplateEngine& engine)
        : base_dir(base_dir), engine(engine) {}

    bool visit(const Path& path, FsType type) override {
      if (type == FsType::File) {
        Path             relative = path.relative_to(base_dir).with_ext("");
        TemplateInstance instance = TemplateInstance(path);
        mLogDebug("Template instance: ", relative);
        engine.insert(relative.view(), instance);
      }
      return true;
    }
  };

  List<TemplateToken> parse_template(StrView content) {
    List<TemplateToken> tokens;
    size_t              pos           = 0;
    size_t              expanded_size = 0;

    while (pos < content.size()) {
      StrView view = content.sub(pos);

      size_t open_pos = view.find("{{");
      if (open_pos == StrView::npos) {
        expanded_size += view.size();
        tokens.push_back(TemplateToken{
            .text = Str(view),
            .type = TemplateTokenType::Text,
        });
        break;
      }

      StrView before_open = view.sub(0, open_pos);
      expanded_size += before_open.size();
      tokens.push_back(TemplateToken{
          .text = Str(before_open),
          .type = TemplateTokenType::Text,
      });

      size_t close_pos = view.find("}}");
      if (close_pos == StrView::npos) {
        mLogWarn("Closing term of template was not found!");
        break;
      }

      StrView key = view.sub(open_pos + 2, close_pos - open_pos - 2).trim();
      tokens.push_back(TemplateToken{
          .key  = StrHash(key),
          .type = TemplateTokenType::Substitution,
      });

      pos += close_pos + 2;
    }

    return tokens;
  }
}  // namespace

TemplateKV& TemplateKV::insert(StrHash hash, StrView value) {
  assert(size < g_max_template_params);
  keys[size]   = hash;
  values[size] = value;
  ++size;
  return *this;
}

const StrView* TemplateKV::find(StrHash needle) const {
  for (size_t i = 0; i < size; i++) {
    if (keys[i] == needle) {
      return &values[i];
    }
  }
  return nullptr;
}

TemplateInstance::TemplateInstance(const Path& source_path) : source_path_(source_path) {
  parse_template();
}

Str TemplateInstance::render(const TemplateKV& values) {
  StrBuilder builder;
  render(builder, values);
  return builder.to_string();
}

void TemplateInstance::render(StrBuilder& builder, const TemplateKV& values) {
#ifdef _DEBUG
  parse_template();
#endif

  builder.ensure_capacity(builder.view().size() + max_rendered_size_);
  size_t render_begin = builder.view().size();

  for (const TemplateToken& token : tokens_) {
    if (token.type == TemplateTokenType::Text) {
      builder.append(token.text);
    } else if (token.type == TemplateTokenType::Substitution) {
      const StrView* value = values.find(token.key);
      if (value) {
        builder.append(*value);
      } else {
        mLogWarn("No substitution found for key ", token.key.hash());
      }
    } else {
      mLogCrit("Token type is unknown");
    }
  }

  size_t current_render_size = builder.view().size() - render_begin;
  max_rendered_size_         = mMax(max_rendered_size_, current_render_size);
}

void TemplateInstance::parse_template() {
  Str content        = source_path_.read_text();
  tokens_            = ::parse_template(content).into_arr();
  max_rendered_size_ = content.size();
}

TemplateEngine::TemplateEngine(const Path& base_dir) {
  TemplateVisitor visitor = TemplateVisitor(base_dir, *this);
  base_dir.visit_dir(visitor, FsDirMode::Recursive);
}

TemplateInstance& TemplateEngine::get(StrHash name) {
  auto it = instances_.find(name);
  assert(it != instances_.end());
  return it.value();
}

void TemplateEngine::insert(StrHash name, TemplateInstance instance) {
  instances_.insert(move(name), move(instance));
}
