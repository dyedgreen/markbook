#pragma once

#include <pthread.h>
#include "webview.h"
#include "notebook/notebook.h"

#define APP_TITLE "Markbook"
#define APP_DEBUG 1
#define APP_DEFAULT_WIDTH 600
#define APP_DEFAULT_HEIGHT 650


typedef struct View {
  struct webview webview;
  Notebook* nb;
  pthread_t index_thread;
  // TODO: Handle messages...
} View;

// Constructor and destructor methods
// If a notebook is provided, the view takes
// ownership of the pointer.
View* view_init(Notebook* nb);
void view_exit(View* view);

// Start view loop. Returns when view exists.
// This also starts an indexer for the notebook
// the user selects.
void view_run(View* view);
