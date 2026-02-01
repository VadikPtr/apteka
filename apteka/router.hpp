#pragma once
#include "cc/dict.hpp"
#include "cc/fs.hpp"
#include "http-req.hpp"
#include "llhttp.h"

struct IReqHandler {
  virtual ~IReqHandler()                                = default;
  virtual void handle(const HttpReq& req, HttpRes& res) = 0;
};

struct ReqHandlerKey {
  llhttp_method method;
  StrHash       path;

  u64  hash() const;
  bool operator==(const ReqHandlerKey& o) const;
};

class Router {
  Dict<ReqHandlerKey, IReqHandler*> handlers_;
  IReqHandler*                      not_found_handler_ = nullptr;

 public:
  ~Router();

  void use_not_found_handler(IReqHandler* not_found_handler);
  void add(llhttp_method method, StrView path, IReqHandler* handler);
  void serve_static(StrView     mount_path,
                    const Path& dir);  // NOTE: this is dev handler, not for production
  void handle(const HttpReq& req, HttpRes& res);
};
