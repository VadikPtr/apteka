#include "common.hpp"
#include <cstdlib>

#ifdef _WIN32
  #define realpath(N, R) _fullpath((R), (N), PATH_MAX)
  #define aligned_alloc(alignment, size) _aligned_malloc(size, alignment)
  #define aligned_free(x) _aligned_free(x)
#else
  // NOTE: this is weird behavior of macOS: when aligned_alloc for 4 bytes, it just
  // returns NULL...
  #define aligned_alloc(alignment, size)                \
    ([alignment_ = alignment, size_ = size]() {         \
      void* mem = malloc(size_);                        \
      mCheck(((uintptr_t)mem & (alignment_ - 1)) == 0); \
      return mem;                                       \
    })()

  #define aligned_free(x) free(x)
#endif

bool uv_check_error(int code, StrView message) {
  if (code) {
    mLogWarn("UV error ", message, " returned ", code, ": ", StrView(uv_strerror(code)));
    return false;
  }
  return true;
}

uv_buf_t uv_alloc_buffer(size_t size) {
  char* allocated_memory = (char*)aligned_alloc(4, size);
  return uv_buf_init(allocated_memory, (unsigned int)size);
}

void uv_alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
  *buf = uv_alloc_buffer(suggested_size);
}

void uv_free_buffer_data(void* data) {
  if (data) {
    aligned_free(data);
  }
}

void uv_free_buffer(const uv_buf_t* buf) {
  if (buf) {
    uv_free_buffer_data(buf->base);
  }
}
