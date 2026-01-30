#pragma once
#include "cc/str-view.hpp"
#include "cc/log.hpp"
#include "uv.h"


#define mUvCheckCrit(...)                                              \
  if (int r_uv_check_ = __VA_ARGS__; r_uv_check_) {                    \
    mLogCrit("UV error " #__VA_ARGS__ " returned ", r_uv_check_, ": ", \
             StrView(uv_strerror(r_uv_check_)));                       \
  }

#define mUvCheckError(...) uv_check_error(__VA_ARGS__, #__VA_ARGS__)

bool     uv_check_error(int code, StrView message);
uv_buf_t uv_alloc_buffer(size_t size);
void     uv_alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void     uv_free_buffer_data(void* data);
void     uv_free_buffer(const uv_buf_t* buf);
