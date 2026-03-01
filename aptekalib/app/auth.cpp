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

Str Auth::hash_password(Str password) const {
  unsigned char hash[crypto_pwhash_STRBYTES];

  int r = crypto_pwhash_str(reinterpret_cast<char*>(hash), password.data(), password.size(),
                            crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE);
  if (r != 0) {
    mLogCrit("Failed to hash password");
  }

  return Str(reinterpret_cast<char*>(hash));
}

bool Auth::check_hash_matches(StrView hashed_password, StrView password) const {
  int r = crypto_pwhash_str_verify(hashed_password.data(), password.data(), password.size());
  return r == 0;
}
