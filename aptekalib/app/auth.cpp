#include "app/auth.hpp"
#include <cc/error.hpp>
#include <cc/log.hpp>
#include <sodium/core.h>
#include <sodium/crypto_pwhash.h>

Auth::Auth() {
  if (sodium_init() < 0) {
    throw Err("Failed to init sodium");
  }
}

void Auth::init(StrView admin_password) {
  admin_password_ = admin_password;
  admin_password_.null_terminate();
}

bool Auth::check_hash_matches(StrView password) const {
  int r = crypto_pwhash_str_verify(admin_password_.data(), password.data(), password.size());
  return r == 0;
}

Str Auth::hash_password(Str password) const {
  char hash[crypto_pwhash_STRBYTES];

  int r = crypto_pwhash_str(hash, password.data(), password.size(),
                            crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE);
  if (r != 0) {
    mLogCrit("Failed to hash password");
  }

  return Str(hash);
}
