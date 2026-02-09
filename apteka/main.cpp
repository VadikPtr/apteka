#include "app/app.hpp"
#include "http-server/http-server.hpp"
#include "http-server/ip.hpp"
#include "http-server/router.hpp"
#include <cc/log.hpp>

int main(int argc, const char** argv) try {
  AppContext app_context(argc, argv);
  Router     router;
  app_context.configure(router);
  HttpServer server = HttpServer(router);
  SockAddr   addr   = SockAddr(app_context.arguments.host.data(), app_context.arguments.port);
  return server.listen(addr);
} catch (const Err& err) {
  mLogCrit("Fatal error: ", err.message());
}
