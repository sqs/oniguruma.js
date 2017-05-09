#include "./str.h"
#include <string>
#include <iostream>
#include <locale>
#include <codecvt>
#include "nbind/api.h"

OnigString::OnigString(nbind::Buffer utf16Array)
{
	static int idGenerator = 0;
	uniqueId_ = ++idGenerator;

	// Convert UTF-16 encoded buffer to UTF-8 string.
	std::u16string utf16Value = std::u16string(reinterpret_cast<char16_t *>(utf16Array.data()), utf16Array.length() / sizeof(char16_t));
	std::u16string err2 = u"ERR2";
	std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> convert("ERR1", err2);
	//try
	//{
	utf8Value = convert.to_bytes(reinterpret_cast<char16_t *>(utf16Array.data()), reinterpret_cast<char16_t *>(utf16Array.data() + utf16Array.length()));
	//}
	//catch (const std::range_error &e)
	//{
	//		printf("RANGE ERROR\n");
	//}
	utf8_length_ = utf8Value.length();

	printf("buffer len is: %d\n", utf16Array.length());
	printf("UTF-8: %s len %d\n", utf8Value.data(), utf8Value.length());
	printf("UTF-16: %s len %d\n", utf16Value.data(), utf16Value.length());

	hasMultiByteChars = (utf16Value.length() != utf8_length_);

	if (hasMultiByteChars)
	{
		utf16_length_ = utf16Value.length();

		utf16OffsetToUtf8 = new int[utf16_length_ + 1];
		utf16OffsetToUtf8[utf16_length_] = utf8_length_;

		utf8OffsetToUtf16 = new int[utf8_length_ + 1];
		utf8OffsetToUtf16[utf8_length_] = utf16_length_;

		// http://stackoverflow.com/a/148766
		int i8 = 0;
		for (int i16 = 0, len = utf16_length_; i16 < len; i16++)
		{
			char16_t in = utf16Value[i16];

			unsigned int codepoint = in;
			bool wasSurrogatePair = false;

			if (in >= 0xd800 && in <= 0xdbff)
			{
				// Hit a high surrogate, try to look for a matching low surrogate
				if (i16 + 1 < len)
				{
					char16_t next = utf16Value[i16 + 1];
					if (next >= 0xdc00 && next <= 0xdfff)
					{
						// Found the matching low surrogate
						codepoint = (((in - 0xd800) << 10) + 0x10000) | (next - 0xdc00);
						wasSurrogatePair = true;
					}
				}
			}

			utf16OffsetToUtf8[i16] = i8;

			if (codepoint <= 0x7f)
			{
				utf8OffsetToUtf16[i8++] = i16;
			}
			else if (codepoint <= 0x7ff)
			{
				utf8OffsetToUtf16[i8++] = i16;
				utf8OffsetToUtf16[i8++] = i16;
			}
			else if (codepoint <= 0xffff)
			{
				utf8OffsetToUtf16[i8++] = i16;
				utf8OffsetToUtf16[i8++] = i16;
				utf8OffsetToUtf16[i8++] = i16;
			}
			else
			{
				utf8OffsetToUtf16[i8++] = i16;
				utf8OffsetToUtf16[i8++] = i16;
				utf8OffsetToUtf16[i8++] = i16;
				utf8OffsetToUtf16[i8++] = i16;
			}

			if (wasSurrogatePair)
			{
				utf16OffsetToUtf8[i16 + 1] = utf16OffsetToUtf8[i16];
				i16++;
			}
		}
	}
}

OnigString::~OnigString()
{
	if (hasMultiByteChars)
	{
		delete[] utf16OffsetToUtf8;
		delete[] utf8OffsetToUtf16;
	}
}

int OnigString::ConvertUtf8OffsetToUtf16(int utf8Offset)
{
	if (hasMultiByteChars)
	{
		if (utf8Offset < 0)
		{
			return 0;
		}
		if ((size_t)utf8Offset > utf8_length_)
		{
			return utf16_length_;
		}
		return utf8OffsetToUtf16[utf8Offset];
	}
	return utf8Offset;
}

int OnigString::ConvertUtf16OffsetToUtf8(int utf16Offset)
{
	if (hasMultiByteChars)
	{
		if (utf16Offset < 0)
		{
			return 0;
		}
		if ((size_t)utf16Offset > utf16_length_)
		{
			return utf8_length_;
		}
		return utf16OffsetToUtf8[utf16Offset];
	}
	return utf16Offset;
}

#include "nbind/nbind.h"

#ifdef NBIND_CLASS
NBIND_CLASS(OnigString)
{
	construct<nbind::Buffer>();
	method(utf8_value);
	method(utf8_length);
}
#endif // NBIND_CLASS