#pragma once

#include "sqlite/sqlite3.h"

typedef struct Notebook {
  sqlite3* index_db; // Connection to (root)/.markbook
  char* root;        // Root directory
} Notebook;

Notebook* open_notebook(const char* root);
void close_notebook(Notebook* nb);

char* nb_file_path(Notebook* nb, const char* file);
int nb_note_id(Notebook* nb, const char* file);

// int reindex_notebook(Notebook* nb); (should refresh index for each file if outdated...)
// TODO: search, retrieve item, retrieve item list, etc...
