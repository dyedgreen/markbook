#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "app/entry_points.h"
#include "ui/view.h"

int main() {
  // Start application threads
  pthread_t app_ui;

  View* v = view_init();
  if (v == NULL || pthread_create(&app_ui, NULL, thread_ui, v) != 0) {
    printf("Could not initialize ui thread.\n");
    return 1;
  }

  // TODO: Move ui into main thread ...

  // Keep busy
  while(1) {
    sleep(10);
  }

  // View is cleared by ui thread

  return 0;
}
