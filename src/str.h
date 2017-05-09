#ifndef SRC_ONIG_STR_H_
#define SRC_ONIG_STR_H_

#include <memory>
#include <string>
#include <stddef.h>
#include "nbind/api.h"

using ::std::shared_ptr;

class OnigString
{
public:
  // Use nbind::Buffer of a Uint16Array from JS to avoid incurring unnecessary UTF-16 to UTF-8
  // conversion.
  explicit OnigString(nbind::Buffer utf16Array);
  OnigString(const OnigString *other) = delete;
  ~OnigString();

  int uniqueId() { return uniqueId_; }

  const char *utf8_value() const { return utf8Value.c_str(); }
  size_t utf8_length() const { return utf8_length_; }

  int ConvertUtf8OffsetToUtf16(int utf8Offset);
  int ConvertUtf16OffsetToUtf8(int utf16Offset);

private:
  std::string utf8Value;
  int uniqueId_;
  size_t utf8_length_;
  bool hasMultiByteChars;

  // - the following members are used only if hasMultiByteChars is true
  size_t utf16_length_;
  int *utf16OffsetToUtf8;
  int *utf8OffsetToUtf16;

  OnigString(const OnigString &) = delete;            // Disallow copying
  OnigString &operator=(const OnigString &) = delete; // Disallow copying
};

#endif // SRC_ONIG_STR_H_
