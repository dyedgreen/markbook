#pragma once

#include "notebook.h"


int nb_index_file(Notebook* nb, const char* file);
int nb_index_if_outdated(Notebook* nb, const char* file);

int nb_index(Notebook* nb);
