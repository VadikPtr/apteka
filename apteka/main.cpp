#include "cc/log.hpp"
#include "http-server.hpp"
#include "ip.hpp"
#include "uv.h"
#include "llhttp.h"

#define mBindAddr "127.0.0.1"
// #define mBindAddr "0.0.0.0"

int main() {
  HttpServer server;
  server.listen(SockAddr(mBindAddr, 8080));
  mLogInfo("Listening on http://" mBindAddr ":8080");
  return uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
