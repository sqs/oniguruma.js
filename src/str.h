#ifndef SRC_ONIG_STR_H_
#define SRC_ONIG_STR_H_

#include <memory>
#include <string>
#include <stddef.h>
#include "nbind/api.h"
#include "nbind/BindDefiner.h"

using ::std::shared_ptr;

class OnigString
{
public:
  explicit OnigString(std::string utf8Value, int x);
  OnigString(const OnigString *other)
      : uniqueId_(other->uniqueId_), utf8Value(other->utf8Value), utf8_length_(other->utf8_length_),
        utf16_length_(other->utf16_length_), utf16OffsetToUtf8(other->utf16OffsetToUtf8), utf8OffsetToUtf16(other->utf8OffsetToUtf16){};
  ~OnigString();

  int uniqueId() { return uniqueId_; }

  const char *utf8_value() const { return utf8Value.c_str(); }
  size_t utf8_length() const { return utf8_length_; }

  int ConvertUtf8OffsetToUtf16(int utf8Offset);
  int ConvertUtf16OffsetToUtf8(int utf16Offset);

  void toJS(nbind::cbOutput output)
  {
    output(utf8Value, 0);
  }

private:
  std::string utf8Value;
  int uniqueId_;
  size_t utf8_length_;
  bool hasMultiByteChars;

  // - the following members are used only if hasMultiByteChars is true
  size_t utf16_length_;
  int *utf16OffsetToUtf8;
  int *utf8OffsetToUtf16;
};

#endif // SRC_ONIG_STR_H_
