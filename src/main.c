#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#define WEBVIEW_IMPLEMENTATION // THIS IS VERY MUCH NECCESSARY!
#include "webview.h"
#include "notebook/notebook.h"
#include "app/entry_points.h"

int main(int argc, const char** argv) {
  if (argc < 2) {
    printf("Give me a folder!\n");
    return 1;
  }

  printf("Opening folder %s \n", argv[1]);
  Notebook* nb = open_notebook(argv[1]);

  if (nb == NULL) {
    printf("Fuck...\n");
    return 0;
  }

  // Start application threads
  pthread_t app_t_indexer;

  if (pthread_create(&app_t_indexer, NULL, thread_indexer, nb)) {
    printf("Could not initialize indexing thread.\n");
    return 1;
  }

  // TODO: Move into entry points
  struct webview webview = {
      .title = "Markbook",
      .url = "https://tilman.xyz",
      .width = 800,
      .height = 600,
      .debug = 1,
      .resizable = 1,
  };

  webview_init(&webview);
  webview_set_color(&webview, 255, 255, 255, 255);
  // webview.external_invoke_cb = *callback;
  while (webview_loop(&webview, 0) == 0) {
    // NEEDED!!!
    usleep(10000);
  }

  // TODO: Start ui thread
  // TODO: Terminate other threads when ui loop terminates

  // Clean up (NOTE: May not be reached if app is quit... Should be investigated)
  close_notebook(nb);
  webview_exit(&webview);

  return 0;
}
