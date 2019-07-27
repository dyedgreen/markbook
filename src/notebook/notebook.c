#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "notebook.h"
#include "sql.h"
#include "debug.h"

#define INDEX_DB_FILE "markbook.db" // TODO: change to .markbook or similar for release!

int init_db(sqlite3* index_db) {
  return SQLITE_OK == sqlite3_exec(index_db, NB_SQL_INIT, NULL, NULL, NULL);
}

// Create notebook for root and init
// index database if not present.
Notebook* open_notebook(const char* root) {
  Notebook* nb = malloc(sizeof(Notebook));

  // Create index_db filename
  int s_root = strlen(root);
  int s_file = strlen(INDEX_DB_FILE);
  nb->root = malloc((s_root + s_file + 2) * sizeof(char)); // \0 + one slash
  strcpy(nb->root, root);
  nb->root[s_root] = '/';
  strcpy(&nb->root[s_root+1], INDEX_DB_FILE);

  // Open SQLite DB
  nb->index_db = NULL;
  if(SQLITE_OK != sqlite3_open((const char*)nb->root, &nb->index_db)) {
    // Failed to open database
    close_notebook(nb);
    return NULL;
  }

  // Initialize database
  if(!init_db(nb->index_db)) {
    // Failed to initialize database
    close_notebook(nb);
    return NULL;
  }

  // Remove db file name from root (by inserting a \0 char)
  nb->root[s_root] = '\0';

  return nb;
}

// Free notebook struct
void close_notebook(Notebook* nb) {
  if (nb != NULL) {
    sqlite3_close(nb->index_db);
    free(nb->root);
    free(nb);
  }
}

// Obtain file path from notebook and file
// path relative to root.
// The resulting pointer should be freed by
// the user.
char* nb_file_path(Notebook* nb, const char* file) {
  int s_file = strlen(file);
  int s_root = strlen(nb->root);
  char* path = malloc((s_file+s_root+2)*sizeof(char));
  strcpy(path, nb->root);
  path[s_root] = '/';
  strcpy(&path[s_root+1], file);
  return path;
}

// Obtain database id of given note.
// Returns 0 if note not found.
int nb_note_id(Notebook* nb, const char* file) {
  sqlite3_stmt* query;
  if (SQLITE_OK != sqlite3_prepare_v2(nb->index_db, NB_SQL_NOTE_ID, -1, &query, NULL)) return 0;
  if (SQLITE_OK != sqlite3_bind_text(query, 1, file, -1, NULL)) return 0;
  if (SQLITE_ROW != sqlite3_step(query)) return 0;
  int id = sqlite3_column_int(query, 0);
  sqlite3_finalize(query);
  return id;
}

// Returns newline separated list of file names
// The returned pointer is owned by the caller.
sds nb_api_list_notes(Notebook* nb) {
  sqlite3_stmt* query;
  if (SQLITE_OK != sqlite3_prepare_v2(nb->index_db, NB_SQL_LIST_NOTES, -1, &query, NULL)) return NULL;

  sds list = sdsempty();
  while (SQLITE_ROW == sqlite3_step(query)) {
    if (sdslen(list) > 0)
      list = sdscat(list, "\n");
    list = sdscat(list, (const char*)sqlite3_column_text(query, 0));
  }
  sqlite3_finalize(query);
  return list;
}

// Returns rendered HTML as sds string.
// The returned pointer is owned by the caller.
sds nb_api_get_note(Notebook* nb, const char* file) {
  sqlite3_stmt* query;
  if (SQLITE_OK != sqlite3_prepare_v2(nb->index_db, NB_SQL_GET_NOTE, -1, &query, NULL)) return NULL;
  if (SQLITE_OK != sqlite3_bind_text(query, 1, file, -1, NULL)) return NULL;
  if (SQLITE_ROW != sqlite3_step(query)) return NULL;

  sds html = sdsempty();
  html = sdscat(html, (const char*)sqlite3_column_text(query, 0));
  sqlite3_finalize(query);
  return html;
}

// Returns search results from database. Each
// result is returned on three lines (type, note, and value)
sds nb_api_search(Notebook* nb, const char* search_term) {
  sqlite3_stmt* query;
  char* query_str = sqlite3_mprintf(NB_SQL_SEARCH, search_term);
  if (SQLITE_OK != sqlite3_prepare_v2(nb->index_db, query_str, -1, &query, NULL)) {
    sqlite3_free(query_str);
    return NULL;
  }
  sqlite3_free(query_str);

  sds list = sdsempty();
  while (SQLITE_ROW == sqlite3_step(query)) {
    if (sdslen(list) > 0)
      list = sdscat(list, "\n");
    // Append result
    list = sdscatfmt(
      list,
      "%i\n%s\n%s",
      sqlite3_column_int(query, 0),
      (const char*)sqlite3_column_text(query, 1),
      (const char*)sqlite3_column_text(query, 2));
  }
  sqlite3_finalize(query);
  return list;
}
