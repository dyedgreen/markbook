#include <unistd.h>
#include "app/entry_points.h"
#define WEBVIEW_IMPLEMENTATION
#include "view.h"

// Constructor and destructor methods
View* view_init() {
  View* view = malloc(sizeof(View));

  // Configure window
  view->webview.title = APP_TITLE;
  view->webview.url = "https://tilman.xyz"; // FIXME!
  view->webview.width = APP_DEFAULT_WIDTH;
  view->webview.height = APP_DEFAULT_HEIGHT;
  view->webview.debug = APP_DEBUG;
  view->webview.resizable = 1;

  webview_init(&view->webview);
  webview_set_color(&view->webview, 255, 255, 255, 255);
  // webview.external_invoke_cb = *callback; TODO: Implement messaging protocol

  // Set notebook placeholder.
  // This is populated with the
  // notebook path chosen by the
  // user when the ui is run.
  view->nb = NULL;

  return view;
}

void view_exit(View* view) {
  if (view == NULL) return;
  // Webview exit kills the thread it runs in...
  close_notebook(view->nb);
  free(view);
}

void view_run(View* view) {
  while (webview_loop(&view->webview, 0) == 0) {
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
      if (view->nb == NULL || pthread_create(&view->index_thread, NULL, thread_indexer, view->nb) != 0) {
        // Abort
        close_notebook(view->nb);
        webview_exit(&view->webview);
        return;
      }
    }

    // Go easy on the CPU
    usleep(10000);

    // TODO: Should messages be handled in the ui thread
    //       or should they be handled in a separate thread?
  }
}
