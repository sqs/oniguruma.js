#include "./str.h"
#include <string>
#include <iostream>
#include <locale>
#include <codecvt>
#include "nbind/api.h"

// TODO(sqs): do these utf8 conversions using nan or the v8 API directly when running in v8, for perf and safety.

// Adapted from https://github.com/kripken/emscripten/blob/6dc4ac5f9e4d8484e273e4dcc554f809738cedd6/src/preamble.js#L671.
static size_t lengthBytesUTF8(const char16_t const *utf16Data, size_t len)
{
	size_t utf8Len = 0;
	for (size_t i = 0; i < len; i++)
	{
		// Gotcha: charCodeAt returns a 16-bit word that is a UTF-16 encoded code unit, not a Unicode code point of the character! So decode UTF16->UTF32->UTF8.
		// See http://unicode.org/faq/utf_bom.html#utf16-3
		int u = utf16Data[i]; // possibly a lead surrogate
		if (u >= 0xd800 && u <= 0xdbff)
		{
			// Hit a high surrogate. Try to look for a low surrogate immediately following.
			if (i + 1 < len)
			{
				char16_t next = utf16Data[i + 1];
				if (next >= 0xdc00 && next <= 0xdfff)
				{
					// Found a low surrogate.
					u = 0x10000 + ((u & 0x3FF) << 10) | (utf16Data[++i] & 0x3FF);
				}
			}
		}

		if (u <= 0x7F)
		{
			++utf8Len;
		}
		else if (u <= 0x7FF)
		{
			utf8Len += 2;
		}
		else if (u <= 0xFFFF)
		{
			utf8Len += 3;
		}
		else if (u <= 0x1FFFFF)
		{
			utf8Len += 4;
		}
		else if (u <= 0x3FFFFFF)
		{
			utf8Len += 5;
		}
		else
		{
			utf8Len += 6;
		}
	}
	return utf8Len;
}

// Adapted from https://github.com/kripken/emscripten/blob/6dc4ac5f9e4d8484e273e4dcc554f809738cedd6/src/preamble.js#L603.
size_t stringToUTF8Array(const char16_t *str, size_t utf16Len, std::string &outU8Array)
{
	if (!(utf16Len > 0)) // Parameter len is not optional. Negative values, 0, null, undefined and false each don't write out any bytes.
		return 0;

	size_t outIdx = 0;
	size_t endIdx = outU8Array.capacity(); // -1 for string null terminator.
	for (size_t i = 0; i < utf16Len; ++i)
	{
		// Gotcha: charCodeAt returns a 16-bit word that is a UTF-16 encoded code unit, not a Unicode code point of the character! So decode UTF16->UTF32->UTF8.
		// See http://unicode.org/faq/utf_bom.html#utf16-3
		// For UTF8 byte structure, see http://en.wikipedia.org/wiki/UTF-8#Description and https://www.ietf.org/rfc/rfc2279.txt and https://tools.ietf.org/html/rfc3629
		int u = str[i]; // possibly a lead surrogate
		if (u >= 0xd800 && u <= 0xdbff)
		{
			// Hit a high surrogate. Try to look for a low surrogate immediately following.
			if (i + 1 < utf16Len)
			{
				char16_t next = str[i + 1];
				if (next >= 0xdc00 && next <= 0xdfff)
				{
					// Found a low surrogate.
					u = 0x10000 + ((u & 0x3FF) << 10) | (str[++i] & 0x3FF);
				}
			}
		}

		if (u <= 0x7F)
		{
			if (outIdx >= endIdx)
				break;
			outU8Array[outIdx++] = u;
		}
		else if (u <= 0x7FF)
		{
			if (outIdx + 1 >= endIdx)
				break;
			outU8Array[outIdx++] = 0xC0 | (u >> 6);
			outU8Array[outIdx++] = 0x80 | (u & 63);
		}
		else if (u <= 0xFFFF)
		{
			if (outIdx + 2 >= endIdx)
				break;
			outU8Array[outIdx++] = 0xE0 | (u >> 12);
			outU8Array[outIdx++] = 0x80 | ((u >> 6) & 63);
			outU8Array[outIdx++] = 0x80 | (u & 63);
		}
		else if (u <= 0x1FFFFF)
		{
			if (outIdx + 3 >= endIdx)
				break;
			outU8Array[outIdx++] = 0xF0 | (u >> 18);
			outU8Array[outIdx++] = 0x80 | ((u >> 12) & 63);
			outU8Array[outIdx++] = 0x80 | ((u >> 6) & 63);
			outU8Array[outIdx++] = 0x80 | (u & 63);
		}
		else if (u <= 0x3FFFFFF)
		{
			if (outIdx + 4 >= endIdx)
				break;
			outU8Array[outIdx++] = 0xF8 | (u >> 24);
			outU8Array[outIdx++] = 0x80 | ((u >> 18) & 63);
			outU8Array[outIdx++] = 0x80 | ((u >> 12) & 63);
			outU8Array[outIdx++] = 0x80 | ((u >> 6) & 63);
			outU8Array[outIdx++] = 0x80 | (u & 63);
		}
		else
		{
			if (outIdx + 5 >= endIdx)
				break;
			outU8Array[outIdx++] = 0xFC | (u >> 30);
			outU8Array[outIdx++] = 0x80 | ((u >> 24) & 63);
			outU8Array[outIdx++] = 0x80 | ((u >> 18) & 63);
			outU8Array[outIdx++] = 0x80 | ((u >> 12) & 63);
			outU8Array[outIdx++] = 0x80 | ((u >> 6) & 63);
			outU8Array[outIdx++] = 0x80 | (u & 63);
		}
	}
	return outIdx;
}

OnigString::OnigString(nbind::Buffer utf16Array)
{
	static int idGenerator = 0;
	uniqueId_ = ++idGenerator;

	// Convert UTF-16 encoded buffer from JavaScript to a UTF-8 string.
	//
	// We don't need to keep the UTF-16 buffer around, because we pass the UTF-16 string to
	// Oniguruma. (See https://github.com/atom/node-oniguruma/pull/63#issuecomment-295466190
	// for why we can't just keep it in UCS-2/UTF-16.)
	const char16_t *utf16Value = reinterpret_cast<const char16_t *>(utf16Array.data());
	size_t utf16Size = utf16Array.length() / sizeof(char16_t);
	utf8_length_ = lengthBytesUTF8(utf16Value, utf16Size);
	hasMultiByteChars = (utf16Size != utf8_length_);
	utf8Value.reserve(utf8_length_);
	stringToUTF8Array(utf16Value, utf16Size, utf8Value);

	// Cache character offset conversion between UTF-8 and UTF-16; see
	// https://github.com/atom/node-oniguruma/pull/46.
	if (hasMultiByteChars)
	{
		utf16_length_ = utf16Size;

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
	construct<nbind::Buffer>(nbind::Strict());
	method(utf8_value);
	method(utf8_length);
}
#endif // NBIND_CLASS