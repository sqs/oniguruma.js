#ifndef SRC_ONIG_STR_H_
#define SRC_ONIG_STR_H_

#include <memory>
#include <string>
#include <stddef.h>

using ::std::shared_ptr;

class OnigString
{
public:
  explicit OnigString(std::string value);
  ~OnigString();

  int uniqueId() { return uniqueId_; }

  const char *utf8_value() const { return utf8Value.c_str(); }
  size_t utf8_length() const { return utf8_length_; }

  int ConvertUtf8OffsetToUtf16(int utf8Offset);
  int ConvertUtf16OffsetToUtf8(int utf16Offset);

private:
  int uniqueId_;
  std::string utf8Value;
  size_t utf8_length_;
  bool hasMultiByteChars;

  // - the following members are used only if hasMultiByteChars is true
  size_t utf16_length_;
  int *utf16OffsetToUtf8;
  int *utf8OffsetToUtf16;
};

#include "nbind/nbind.h"

#ifdef NBIND_CLASS
NBIND_CLASS(OnigString)
{
  construct<std::string>();
  method(uniqueId);
  method(utf8_value);
  method(utf8_length);
  method(ConvertUtf8OffsetToUtf16);
  method(ConvertUtf16OffsetToUtf8);
}
#endif

#endif // SRC_ONIG_STR_H_
