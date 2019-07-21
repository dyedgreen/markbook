#include <stdlib.h>
#include "base64.h"

const char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Based on: https://nachtimwald.com/2017/11/18/base64-encode-and-decode-in-c/
char* base64_encode(const unsigned char* data, size_t size) {
  // Determine the size of the resulting string
  size_t size_res = size;
  if (size % 3 != 0)
    size_res += 3 - (size % 3);
  size_res = (size_res/3)*4;

  // Allocate memory for result
  char* encoded = malloc(size_res+1);
  if (encoded == NULL)
    return NULL;
  encoded[size_res] = '\0';

  // Transcribe data to base 64
  size_t i;
  size_t j;
  size_t v;
  for (i = 0, j=0; i < size; i+=3, j+=3) {
    v = data[i];
    v = i+1 < size ? v << 8 | data[i+1] : v << 8;
    v = i+2 < size ? v << 8 | data[i+2] : v << 8;
    encoded[j]   = b64chars[(v >> 18) & 0x3F];
    encoded[j+1] = b64chars[(v >> 12) & 0x3F];
    if (i+1 < size) {
      encoded[j+2] = b64chars[(v >> 6) & 0x3F];
    } else {
      encoded[j+2] = '=';
    }
    if (i+2 < size) {
      encoded[j+3] = b64chars[v & 0x3F];
    } else {
      encoded[j+3] = '=';
    }
  }

  return encoded;
}

char* base64_file(const char* file) {
  // TODO
  return NULL;
}
