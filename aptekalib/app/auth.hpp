#pragma once
#include <cc/str.hpp>

class Auth {
  Str admin_password_;

 public:
  Auth();
  void init(StrView admin_password);
  bool check_hash_matches(StrView password) const;
  Str  hash_password(Str password) const;
};
