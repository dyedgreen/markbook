#include <unistd.h>
#include "notebook/index.h"
#include "entry_points.h"


void* thread_indexer(void* notebook) {
  Notebook* nb = (Notebook*) notebook;

  // TODO: Use file-system apis to monitor for changes
  while(1) {
    nb_index(nb);
    sleep(APP_INDEX_WAIT);
  }
}
