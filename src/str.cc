#include "./str.h"
#include <string>
#include <iostream>
#include <locale>
#include <codecvt>
#include "nbind/api.h"

OnigString::OnigString(nbind::Buffer ucs2Array)
{
	static int idGenerator = 0;
	uniqueId_ = ++idGenerator;

	// Convert UCS-2 encoded buffer from JavaScript to a UTF-8 string.
	//
	// We don't need to keep the UCS-2 buffer around, because we pass the UTF-8 string to
	// Oniguruma. (See https://github.com/atom/node-oniguruma/pull/63#issuecomment-295466190
	// for why we can't just keep it in UCS-2 or UTF-16.)
	char16_t *ucs2Value = reinterpret_cast<char16_t *>(ucs2Array.data());
	size_t ucs2Size = ucs2Array.length() / sizeof(char16_t);

	// Replace lone surrogates with the replacement character, because if any of them exist in
	// the input string to codecvt_utf8, the output will be an empty UTF-8 string. This mimics
	// the behavior of the v8::String::Utf8Value conversion.
	for (int i = 0, len = ucs2Size; i < len; i++)
	{
		char16_t in = ucs2Value[i];

		unsigned int codepoint = in;
		bool wasSurrogatePair = false;

		if (in >= 0xd800 && in <= 0xdbff)
		{
			// Hit a high surrogate. Try to look for a low surrogate immediately following.
			if (i + 1 < len)
			{
				char16_t next = ucs2Value[i + 1];
				if (next >= 0xdc00 && next <= 0xdfff)
				{
					// Found a low surrogate. Skip past the surrogate pair.
					i++;
					continue;
				}
			}

			// No matching low surrogate. Replace with replacement character.
			wprintf(L"hit LONE HIGH surrogate %x at i=%d\n", in, i);
			ucs2Value[i] = 0xfffd;
		}
		else if (in >= 0xdc00 && in <= 0xdfff)
		{
			// Hit a low surrogate, and the preceding character was not a high surrogate
			// (otherwise we would've skipped past this character). Replace with replacement
			// character.
			wprintf(L"hit LONE LOW surrogate %x at i=%d\n", in, i);
			ucs2Value[i] = 0xfffd;
		}
	}

	// Perform the conversion. The ucs2Array points to the same underlying data we may have
	// modified in the preceding loop.
	std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> convert;
	utf8Value = convert.to_bytes(
		reinterpret_cast<char16_t *>(ucs2Array.data()),
		reinterpret_cast<char16_t *>(ucs2Array.data() + ucs2Array.length()));
	utf8_length_ = utf8Value.length();

	printf("buffer len is: %d\n", ucs2Array.length());
	printf("UTF-8: len %d:", utf8Value.length());
	for (int i = 0; i < utf8Value.length(); i++)
	{
		printf(" %02hhx", utf8Value[i]);
	}
	printf("\n");
	wprintf(L"UTF-16: len %d: %04x %04x %04x\n", ucs2Size, ucs2Value[0], ucs2Value[1], ucs2Value[2]);

	hasMultiByteChars = (ucs2Size != utf8_length_);

	if (hasMultiByteChars)
	{
		utf16_length_ = ucs2Size;

		utf16OffsetToUtf8 = new int[utf16_length_ + 1];
		utf16OffsetToUtf8[utf16_length_] = utf8_length_;

		utf8OffsetToUtf16 = new int[utf8_length_ + 1];
		utf8OffsetToUtf16[utf8_length_] = utf16_length_;

		// http://stackoverflow.com/a/148766
		int i8 = 0;
		for (int i16 = 0, len = utf16_length_; i16 < len; i16++)
		{
			char16_t in = ucs2Value[i16];

			unsigned int codepoint = in;
			bool wasSurrogatePair = false;

			if (in >= 0xd800 && in <= 0xdbff)
			{
				// Hit a high surrogate, try to look for a matching low surrogate
				if (i16 + 1 < len)
				{
					char16_t next = ucs2Value[i16 + 1];
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