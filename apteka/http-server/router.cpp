#include "router.hpp"
#include "cc/fs.hpp"
#include "content-type.hpp"
#include <cc/log.hpp>
#include <llhttp.h>

namespace {
  void hash_combine(u64& seed, u64 value) {
    seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 13) + (seed >> 7);
  }

  class StaticHandler {
    Path base_dir_;
    Str  mount_path_;

   public:
    StaticHandler(Path base_dir, Str mount_path)
        : base_dir_(move(base_dir)), mount_path_(move(mount_path)) {}

    bool try_handle(const HttpReq& req, HttpRes& res) {
      if (req.url.find("..") != StrView::npos or  //
          not req.url.starts_with(mount_path_)) {
        return false;
      }
      StrView location = req.url.sub(mount_path_.size() + 1);
      if (location.empty()) {
        return false;
      }
      Path path = base_dir_ / location;
      if (FsType type = path.type(); type != FsType::File) {
        mLogDebug("Can't serve ", path, " (", type, "): not a file");
      }
      StrView content_type = ContentType::ext_to_content_type(path.ext());
      size_t  sz           = path.file_size();
      Arr<u8> bytes        = Arr<u8>(sz);
      File    file         = File(path, "rb");
      if (not file.try_open(path, "rb")) {
        mLogDebug("Serve static failed: cannot open file ", path);
        return false;
      }
      if (not file.try_read_bytes(bytes)) {
        mLogDebug("Serve static failed: cannot read file ", path);
        return false;
      }
      res.status(HTTP_STATUS_OK)  //
          .content_type(content_type)
          .body(move(bytes))
          .send();
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

void Router::serve_static(Str mount_path, Path dir) {
  static_handler_ = new StaticHandler(move(dir), move(mount_path));
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
  if (static_handler_ and static_handler_->try_handle(req, res)) {
    return;
  }
  if (not_found_handler_) {
    not_found_handler_->handle(req, res);
    return;
  }
  res.send_basic(HTTP_STATUS_NOT_FOUND);
}
