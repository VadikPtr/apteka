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
        TemplateInstance instance = TemplateInstance(path.read_text(), path);
        mLogDebug("Template instance: ", relative);
        engine.insert(relative.view(), instance);
      }
      return true;
    }
  };
}  // namespace

TemplateInstance::TemplateInstance(Str content, Path source_path)
    : content_(move(content)),
      source_path_(move(source_path)),
      max_expanded_size_(content_.size()) {}

Str TemplateInstance::expand(const SDict<StrView, Str>& values) {
  StrBuilder builder;
  builder.ensure_capacity(max_expanded_size_);
  expand(builder, values);
  return builder.to_string();
}

void TemplateInstance::expand(StrBuilder& builder, const SDict<StrView, Str>& values) {
#ifdef _DEBUG
  content_ = source_path_.read_text();
#endif

  size_t pos           = 0;
  size_t expanded_size = 0;

  while (pos < content_.size()) {
    mLogDebug(pos);
    StrView view = content_.sub(pos);

    size_t open_pos = view.find("{{");
    if (open_pos == StrView::npos) {
      expanded_size += view.size();
      builder.append(view);
      break;
    }

    StrView before_open = view.sub(0, open_pos);
    expanded_size += before_open.size();
    builder.append(before_open);

    size_t close_pos = view.find("}}");
    if (close_pos == StrView::npos) {
      mLogWarn("Closing term of template was not found!");
      break;
    }

    StrView    key   = view.sub(open_pos + 2, close_pos - open_pos - 2).trim();
    const Str* value = values.find_non_sorted(key);

    if (not value) {
      mLogDebug("No substitution found for key ", key);
    } else {
      expanded_size += value->size();
      builder.append(*value);
    }

    pos += close_pos + 2;
  }

  max_expanded_size_ = mMax(max_expanded_size_, expanded_size);
}

TemplateEngine::TemplateEngine(const Path& base_dir) {
  TemplateVisitor visitor = TemplateVisitor(base_dir, *this);
  base_dir.visit_dir(visitor, FsDirMode::Recursive);
}

void TemplateEngine::insert(StrHash name, TemplateInstance instance) {
  instances_.insert(move(name), move(instance));
}

Str TemplateEngine::render(StrView name, const SDict<StrView, Str>& values) {
  auto it = instances_.find(name);
  if (it == instances_.end()) {
    mLogWarn("Template instance ", name, " not found!");
    return Str(name);
  }
  return it.value().expand(values);
}
