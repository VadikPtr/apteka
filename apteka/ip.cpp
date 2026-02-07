#include "ip.hpp"
#include <cc/arr-view.hpp>
#include <cc/common.hpp>
#include <uv.h>
#ifdef _WIN32
  #include <winsock2.h>
#endif

SockAddr::SockAddr(const char* host, int port) {
  sockaddr_in addr_in;
  uv_ip4_addr(host, port, &addr_in);
  memcpy(&addr, &addr_in, sizeof(addr));
}

SockAddr::SockAddr(const sockaddr* a_addr) {
  if (a_addr) {
    addr = *a_addr;
  } else {
    memset(&addr, 0, sizeof(addr));
  }
}

bool SockAddr::is_empty() const {
  static_assert(sizeof(sockaddr) == sizeof(u32) * 4);
  const u32* base = reinterpret_cast<const u32*>(&addr);

  for (u32 v : ArrView<const u32>(base, 4)) {
    if (v != 0) {
      return false;
    }
  }
  return true;
}
