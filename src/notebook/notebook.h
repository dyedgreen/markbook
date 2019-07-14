#pragma once

#include "sqlite/sqlite3.h"
#include "sds/sds.h"

typedef struct Notebook {
  sqlite3* index_db; // Connection to (root)/.markbook
  char* root;        // Root directory
} Notebook;

Notebook* open_notebook(const char* root);
void close_notebook(Notebook* nb);

char* nb_file_path(Notebook* nb, const char* file);
int nb_note_id(Notebook* nb, const char* file);

// API for consumption by JS part

// Returns newline separated list of file names
sds nb_api_list_notes(Notebook* nb);
// Returns rendered HTML
sds nb_api_get_note(Notebook* nb, const char* file);
// Returns search results
// TODO (!)
