#include "unicode-utils.h"
#include <clocale>
#include <stdlib.h>
#include <string>

int UnicodeUtils::characters_in_bytes(const char* string, int bytes) {
  if (bytes == 0)
    return 0;

  mblen(NULL, 0);
  setlocale(LC_CTYPE, "en_US.UTF-8");

  int characters = 0;
  while (bytes > 0) {
    int characterLength = mblen(string, bytes);
    if (characterLength < 1)
      break;
    characters++;
    bytes -= characterLength;
    string += characterLength;
  }
  return characters;
}

int UnicodeUtils::bytes_in_characters(const char* string, int characters) {
  if (characters == 0)
    return 0;

  mblen(NULL, 0);
  setlocale(LC_CTYPE, "en_US.UTF-8");

  int bytes = 0;
  int length = strlen(string);
  while (length > 0) {
    int characterLength = mblen(string, length);
    if (characterLength < 1)
      break;

    bytes += characterLength;
    if (--characters == 0)
      break;

    length -= characterLength;
    string += characterLength;
  }
  return bytes;
}