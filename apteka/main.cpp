#include "cc/log.hpp"
#include "http-server.hpp"
#include "ip.hpp"
#include "uv.h"
#include "llhttp.h"

int main() {
  HttpServer server;
  server.listen(SockAddr("127.0.0.1", 8080));
  mLogInfo("Listening on http://127.0.0.1:8080");
  return uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
