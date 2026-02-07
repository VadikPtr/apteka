#include "router.hpp"
#include "content-type.hpp"
#include <cc/log.hpp>
#include <llhttp.h>

namespace {
  void hash_combine(u64& seed, u64 value) {
    seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 13) + (seed >> 7);
  }

  struct FileServeHandler : public IReqHandler {
    StrView content_type;
    Path    path;

    FileServeHandler(StrView content_type, Path path)
        : content_type(content_type), path(move(path)) {}

    void handle(const HttpReq&, HttpRes& res) override {
      Arr<u8> bytes = read_file();
      res.status(HTTP_STATUS_OK).content_type(content_type).body(bytes).send();
    }

    Arr<u8> read_file() {
      size_t  sz   = path.file_size();
      Arr<u8> res  = Arr<u8>(sz);
      File    file = File(path, "rb");
      if (not file.try_open(path, "rb")) {
        mLogCrit("Serve static failed: cannot open file ", path);
      }
      if (not file.try_read_bytes(res)) {
        mLogCrit("Serve static failed: cannot read file ", path);
      }
      return res;
    }
  };

  struct StaticVisitor : public IFileVisitor {
    const Path& base_dir;
    Router&     router;
    StrView     mount_path;

    StaticVisitor(const Path& base_dir, Router& router, StrView mount_path)
        : base_dir(base_dir), router(router), mount_path(mount_path) {}

    bool visit(const Path& path, FsType type) override {
      if (type == FsType::File) {
        Path         relative     = path.relative_to(base_dir);
        Str          serve_path   = mount_path + "/" + relative;
        StrView      content_type = ContentType::ext_to_content_type(path.ext());
        IReqHandler* handler      = new FileServeHandler(content_type, path);
        mLogDebug("Serving ", serve_path, " (", content_type, ") with ", path);
        router.add(HTTP_GET, serve_path, handler);
      }
      return true;
    }
  };
}  // namespace

u64 ReqHandlerKey::hash() const {
  u64 value = 0;
  hash_combine(value, path.hash());
  hash_combine(value, method);
  return value;
}

bool ReqHandlerKey::operator==(const ReqHandlerKey& o) const {
  return path == o.path && method == o.method;
}

Router::~Router() {
  delete not_found_handler_;
  for (auto it = handlers_.begin(); it != handlers_.end(); ++it) {
    delete it.value();
  }
}

void Router::use_not_found_handler(IReqHandler* not_found_handler) {
  delete not_found_handler_;
  not_found_handler_ = not_found_handler;
}

void Router::add(llhttp_method method, StrView path, IReqHandler* handler) {
  ReqHandlerKey key = ReqHandlerKey{
      .method = method,
      .path   = path,
  };
  if (auto it = handlers_.find(key); it != handlers_.end()) {
    mLogCrit("Failed to add route ", StrView(llhttp_method_name(method)), " ", path,
             ": Already exists");
  }
  handlers_.insert(move(key), move(handler));
}

void Router::serve_static(StrView mount_path, const Path& dir) {
  StaticVisitor visitor = StaticVisitor(dir, *this, mount_path);
  dir.visit_dir(visitor, FsDirMode::Recursive);
}

void Router::handle(const HttpReq& req, HttpRes& res) {
  ReqHandlerKey key = ReqHandlerKey{
      .method = req.method,
      .path   = req.url,
  };
  if (auto it = handlers_.find(key); it != handlers_.end()) {
    it.value()->handle(req, res);
    return;
  }
  if (not_found_handler_) {
    not_found_handler_->handle(req, res);
    return;
  }
  res.send_basic(HTTP_STATUS_NOT_FOUND);
}
