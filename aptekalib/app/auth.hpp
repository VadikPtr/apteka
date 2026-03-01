#pragma once
#include <cc/str.hpp>

class Auth {
 public:
  Auth();

  Str hash_password(Str password) const;

  // requires hashed_password to be null terminated
  bool check_hash_matches(StrView hashed_password, StrView password) const;
};
