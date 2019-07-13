#include "ui/view.h"

int main() {
  // Initialize view
  View* v = view_init(NULL);
  view_run(v);
  // Cleanup
  view_exit(v);
  return 0;
}
