#include "app/auth.hpp"
#include <cc/error.hpp>
#include <cc/log.hpp>

int main(int argc, const char** argv) try {
  if (argc != 2) {
    throw Err("Usage: ./hash password");
  }
  Str  password = Str(argv[1]);
  Auth auth     = Auth();
  Str  hash     = auth.hash_password(password);
  mLogInfo("Password hash: ", hash);
  return 0;
} catch (const Err& err) {
  mLogCrit("Fatal error: ", err.message());
}
