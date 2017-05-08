#ifndef SRC_ONIG_STR_H_
#define SRC_ONIG_STR_H_

#include <memory>
#include <string>
#include <stddef.h>

using ::std::shared_ptr;

class OnigString
{
public:
  // nbind passes JavaScript strings as utf8 std::string.
  explicit OnigString(std::string utf8Value);
  OnigString(const OnigString *other)
      : utf8Value(other->utf8Value), uniqueId_(other->uniqueId_), utf8_length_(other->utf8_length_),
        hasMultiByteChars(other->hasMultiByteChars),
        utf16_length_(other->utf16_length_), utf16OffsetToUtf8(other->utf16OffsetToUtf8), utf8OffsetToUtf16(other->utf8OffsetToUtf16)
  {
    printf("OnigString const pointer copy constructor called\n");
  };
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
