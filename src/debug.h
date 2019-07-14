#pragma once

#ifdef DEBUG
#include <stdio.h>
#define DEBUG_PRINT(...) printf("DEBUG: %s:%d:%s(): ", __FILE__, __LINE__, __func__);\
                         printf(__VA_ARGS__);
#else
#define DEBUG_PRINT(...)
#endif
