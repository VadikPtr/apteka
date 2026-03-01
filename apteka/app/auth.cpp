#include "auth.hpp"
#include <cc/error.hpp>

#define SODIUM_LIBRARY_MINIMAL
#include "sodium.h"

void auth::init() {
  if (sodium_init() < 0) {
    throw Err("Failed to init sodium");
  }
}
