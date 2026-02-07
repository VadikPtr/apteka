#pragma once
#include <cc/str-view.hpp>

struct ContentType {
  static StrView font_woff() { return "font/woff"; }
  static StrView font_woff2() { return "font/woff2"; }
  static StrView font_ttf() { return "font/ttf"; }
  static StrView font_otf() { return "font/otf"; }

  static StrView application_javascript() { return "application/javascript"; }
  static StrView application_json() { return "application/json"; }
  static StrView application_x_www_form_urlencoded() { return "application/x-www-form-urlencoded"; }
  static StrView application_xml() { return "application/xml"; }
  static StrView application_webmanifest() { return "application/manifest+json"; }
  static StrView application_binary() { return "application/octet-stream"; }

  static StrView image_gif() { return "image/gif"; }
  static StrView image_webp() { return "image/webp"; }
  static StrView image_jpeg() { return "image/jpeg"; }
  static StrView image_png() { return "image/png"; }
  static StrView image_svg() { return "image/svg+xml"; }
  static StrView image_ico() { return "image/vnd.microsoft.icon"; }
  static StrView image_tiff() { return "image/tiff"; }
  static StrView image_bmp() { return "image/bmp"; }
  static StrView image_heic() { return "image/heic"; }
  static StrView image_heif() { return "image/heif"; }

  static StrView multipart_form_data() { return "multipart/form-data"; }

  static StrView text_css() { return "text/css"; }
  static StrView text_html() { return "text/html"; }
  static StrView text_plain() { return "text/plain"; }
  static StrView text_xml() { return "text/xml"; }
  static StrView text_markdown() { return "text/markdown"; }

  static StrView ext_to_content_type(StrView ext);
};
