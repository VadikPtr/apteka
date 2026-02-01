#include "http-server.hpp"
#include "http-connection.hpp"
#include "common.hpp"
#include "cc/log.hpp"
#include "uv.h"

static constexpr int g_backlog = 1024 * 16;

HttpServer::HttpServer(Router& router) : router_(router) {
  memset(&socket_, 0, sizeof(socket_));
  socket_.data = this;
  mUvCheckCrit(uv_tcp_init(uv_default_loop(), &socket_));
}

void HttpServer::listen(const SockAddr& addr) {
  mUvCheckCrit(uv_tcp_bind(&socket_, &addr.addr, 0));
  // mUvCheckCrit(uv_tcp_keepalive(&socket_, 1, 45));
  mUvCheckCrit(uv_tcp_simultaneous_accepts(&socket_, 1));
  mUvCheckCrit(uv_listen(get_stream(), g_backlog, connection_cb));
}

void HttpServer::report_closed(class HttpConnection* con) {
  --connections_num_;
  mLogDebug("Connection destroyed! Currently active: ", connections_num_);
}

uv_stream_t* HttpServer::get_stream() {
  return (uv_stream_t*)&socket_;
}

void HttpServer::connection_cb(uv_stream_t* socket, int status) {
  HttpServer* self = (HttpServer*)socket->data;

  if (status == -1) {
    mLogWarn("Connection callback got error");
    return;
  }

  HttpConnection* con = new HttpConnection(*self, self->router_);
  ++self->connections_num_;
  mLogDebug("New connection! Currently active: ", self->connections_num_);

  if (not con->init(socket)) {
    delete con;
    return;
  }

  con->start();
}
