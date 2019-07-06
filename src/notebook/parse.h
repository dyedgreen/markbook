#pragma once

#include "sds/sds.h"

// Index type for the indexing callback.
// The lower the type, the more important
// the index.
typedef enum IndexType {
  IndexEquation = 0,
  IndexHeading = 1, // If the returned type is in {1,2,3,4,5,6}, it is an nth order heading
  IndexCode = 7,
} IndexType;

// The parse function allows you to collect indexing information while the
// document is rendered to an html string. The indexing information is
// returned as individual elements, where the order indicates the order
// in the document and the type indicates the type if index.
//
// If the callback returns a positive, non-zero integer, the parsing is aborted
// and a null pointer is returned.
sds parse(
  const char* text,
  int (*index_callback)(IndexType /*type*/, const char* /*value*/, size_t /*size*/, void* /* userdata */),
  void* userdata);

// Convenience wrapper that only emits html
sds render_html(const char* text);
