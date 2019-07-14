#include "notebook/notebook.h"
#include "ui/view.h"
#include "debug.h"

int main(int argc, char** argv) {
  // If notebook root is provided via command line
  Notebook* nb = NULL;
  if (argc >= 2) {
    DEBUG_PRINT("Opening command line supplied notebook %s.\n", argv[1]);
    nb = open_notebook(argv[1]);
    if (nb == NULL)
      return 1;
  }
  // Initialize view
  View* v = view_init(nb);
  view_run(v);
  // Cleanup
  view_exit(v);
  return 0;
}
