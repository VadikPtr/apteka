#include "content-type.hpp"

StrView ContentType::ext_to_content_type(StrView ext) {
  if (ext.empty()) {
    return application_binary();
  }

  if (ext.compare_ci(".otf") == ComparePos::Equals) {
    return font_otf();
  }
  if (ext.compare_ci(".ttf") == ComparePos::Equals) {
    return font_ttf();
  }
  if (ext.compare_ci(".woff2") == ComparePos::Equals) {
    return font_woff2();
  }
  if (ext.compare_ci(".woff") == ComparePos::Equals) {
    return font_woff();
  }

  if (ext.compare_ci(".js") == ComparePos::Equals || ext.compare_ci(".mjs") == ComparePos::Equals) {
    return application_javascript();
  }
  if (ext.compare_ci(".json") == ComparePos::Equals) {
    return application_json();
  }
  if (ext.compare_ci(".xml") == ComparePos::Equals) {
    return application_xml();
  }
  if (ext.compare_ci(".webmanifest") == ComparePos::Equals) {
    return application_webmanifest();
  }

  if (ext.compare_ci(".gif") == ComparePos::Equals) {
    return image_gif();
  }
  if (ext.compare_ci(".webp") == ComparePos::Equals) {
    return image_webp();
  }
  if (ext.compare_ci(".jpeg") == ComparePos::Equals ||
      ext.compare_ci(".jpg") == ComparePos::Equals) {
    return image_jpeg();
  }
  if (ext.compare_ci(".png") == ComparePos::Equals) {
    return image_png();
  }
  if (ext.compare_ci(".svg") == ComparePos::Equals) {
    return image_svg();
  }
  if (ext.compare_ci(".ico") == ComparePos::Equals) {
    return image_ico();
  }
  if (ext.compare_ci(".bmp") == ComparePos::Equals) {
    return image_bmp();
  }
  if (ext.compare_ci(".heic") == ComparePos::Equals) {
    return image_heic();
  }
  if (ext.compare_ci(".heif") == ComparePos::Equals) {
    return image_heif();
  }
  if (ext.compare_ci(".tiff") == ComparePos::Equals ||
      ext.compare_ci(".tif") == ComparePos::Equals) {
    return image_tiff();
  }

  if (ext.compare_ci(".css") == ComparePos::Equals) {
    return text_css();
  }
  if (ext.compare_ci(".md") == ComparePos::Equals) {
    return text_markdown();
  }
  if (ext.compare_ci(".html") == ComparePos::Equals ||
      ext.compare_ci(".htm") == ComparePos::Equals) {
    return text_html();
  }

  return text_plain();
}
