#include "ui/view.h"
#include "entry_points.h"


void* thread_ui(void* view) {
  View* v = (View*) view;
  view_run(v);
  view_exit(v);
}
