#include <stdio.h>
#include "notebook/notebook.h"
#include "notebook/index.h"

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

  // Index a file
  if (!nb_index(nb)) {
    printf("index failed\n");
  }

  // Clean up
  close_notebook(nb);

  return 0;
}
