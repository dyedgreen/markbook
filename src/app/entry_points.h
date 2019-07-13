#pragma once

// This header defines entry points for the applications
// threads.

// Constants
#define APP_INDEX_WAIT 60 // seconds

// Entry point for thread handling indexing of
// files in directory.
void* thread_indexer(void* notebook);
