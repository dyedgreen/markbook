#include <unistd.h>
#include "worker/entrypoints.h"
#include "bundle.h"
#define WEBVIEW_IMPLEMENTATION
#include "view.h"
#include "message.h" // TODO: Fix ifdef in webview so I can have these in the proper order
#include "debug.h"


// Currently, because of they way the webview callbacks work,
// we can only support a single queue :(
// (This is okay for now, since we only need a single window.)
MessageQueue* global_message_queue = NULL;

// Message handler callback
void message_callback(struct webview* webview, void* arg) {
  message_digest_fragment(global_message_queue, (char*)arg);
  // We are not responsible for the arg pointer
}


// Constructor and destructor methods
View* view_init(Notebook* nb) {
  // If the message queue is taken, there is nothing we can do
  if (global_message_queue != NULL)
    return NULL;

  View* view = malloc(sizeof(View));
  if (view == NULL)
    return NULL;

  // Init message queue
  global_message_queue = message_init_queue(&view->webview);
  if (global_message_queue == NULL) {
    free(view);
    return NULL;
  }

  // Configure window
  view->webview.title = APP_TITLE;
  view->webview.url = html_bundle;
  view->webview.width = APP_DEFAULT_WIDTH;
  view->webview.height = APP_DEFAULT_HEIGHT;
  view->webview.debug = APP_DEBUG;
  view->webview.resizable = 1;

  webview_init(&view->webview);
  webview_set_color(&view->webview, 255, 255, 255, 255);
  view->webview.external_invoke_cb = *message_callback;

  // Set notebook. This may be
  // a NULL pointer, in which
  // case the user will be prompted
  // to select a notebook.
  view->nb = nb;

  return view;
}

void view_exit(View* view) {
  if (view == NULL) return;
  close_message_queue(global_message_queue);
  // webview_exit kills the thread it runs in...
  webview_terminate(&view->webview);
  close_notebook(view->nb);
  free(view);
}

void view_run(View* view) {
  // Acquire the folder to view from user
  if (view->nb == NULL) {
    char* nb_folder = malloc(sizeof(char)*2024);
    webview_dialog(
      &view->webview,
      WEBVIEW_DIALOG_TYPE_OPEN,
      WEBVIEW_DIALOG_FLAG_DIRECTORY,
      "Open Notebook",
      "Please select a folder containing your markdown notes.",
      nb_folder, sizeof(char)*2024);
    view->nb = open_notebook(nb_folder);
    free(nb_folder);
  }

  // Start indexing thread
  if (view->nb == NULL || pthread_create(&view->index_thread, NULL, thread_indexer, view->nb) != 0) {
    // Abort
    close_notebook(view->nb);
    webview_exit(&view->webview);
    return;
  }
  // Register notebook with message queue
  global_message_queue->nb = view->nb;

  // Load ui bundle
  DEBUG_PRINT("Loading UI bundle...\n");
  webview_loop(&view->webview, 1);
  webview_loop(&view->webview, 1); // Need two such that the webview is properly initialized
  webview_inject_css(&view->webview, css_bundle);
  webview_eval(&view->webview, js_bundle);

  while (webview_loop(&view->webview, 1) == 0) {
    // Go easy on the CPU
    usleep(10000);

    // TODO: Should messages be handled in the ui thread
    //       or should they be handled in a separate thread?
    message_respond(global_message_queue);
  }
}
