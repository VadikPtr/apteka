#include "http-server.hpp"
#include "http-connection.hpp"
#include "common.hpp"
#include "cc/log.hpp"
#include "uv.h"

HttpServer::HttpServer() {
  memset(&socket_, 0, sizeof(socket_));
  socket_.data = this;
  mUvCheckCrit(uv_tcp_init(uv_default_loop(), &socket_));
}

void HttpServer::listen(const SockAddr& addr) {
  mUvCheckCrit(uv_tcp_bind(&socket_, &addr.addr, 0));
  mUvCheckCrit(uv_listen(get_stream(), 1024, connection_cb));
}

uv_stream_t* HttpServer::get_stream() {
  return (uv_stream_t*)&socket_;
}

void HttpServer::connection_cb(uv_stream_t* server, int status) {
  if (status == -1) {
    mLogWarn("Connection callback got error");
    return;
  }

  HttpConnection* con = new HttpConnection();
  if (not con->init()) {
    delete con;
    return;
  }

  if (not mUvCheckError(uv_accept(server, con->get_stream()))) {
    con->close();
    return;
  }

  con->start();
}
