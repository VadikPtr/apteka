#include "router.hpp"
#include "cc/log.hpp"
#include "llhttp.h"

namespace {
  void hash_combine(u64& seed, u64 value) {
    seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 13) + (seed >> 7);
  }
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
    mLogCrit("Failed to add route ", StrView(llhttp_method_name(method)), " ", path, ": Already exists");
  }
  handlers_.insert(move(key), move(handler));
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
