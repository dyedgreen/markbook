#pragma once

#include "notebook.h"


int index_file(Notebook* nb, const char* file);
int index_if_outdated(Notebook* nb, const char* file);

int index(Notebook* nb);

// TODO:
// index_file
// walk_dir
// ...
