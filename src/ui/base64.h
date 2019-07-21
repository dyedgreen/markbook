#pragma once

// Encode the data given to base64. This allocates
// a char* pointer of the required size, which will
// be owned by the caller.
char* base64_encode(const unsigned char* data, size_t size);

// Return data from file in base64.
char* base64_file(const char* file);
