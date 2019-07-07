#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include "parse.h"
#include "sql.h"
#include "index.h"


typedef struct IndexData {
  Notebook* nb;
  int note_id;
} IndexData;

// Helper function, callback for indexing operation
int h_index_callback(IndexType type, const char* value, size_t size, void* userdata) {
  IndexData* dt = (IndexData*) userdata;
  sqlite3_stmt* query;
  // TODO: Handle errors (?)
  sqlite3_prepare_v2(dt->nb->index_db, NB_SQL_INSERT_SEARCH, -1, &query, NULL);
  sqlite3_bind_int(query, 1, dt->note_id);
  sqlite3_bind_int(query, 2, type);
  sqlite3_bind_text(query, 3, value, size/sizeof(char), NULL);
  sqlite3_step(query);
  sqlite3_finalize(query);
  return 0;
}

// Adds or updates the index for a file in
// the markbook database
int nb_index_file(Notebook* nb, const char* file) {
  // Variables that are cleaned up at the end
  int succ = 0;
  FILE* doc;
  char* contents = NULL;
  sds html = NULL;

  // Read file contents
  char* doc_path = nb_file_path(nb, file);
  doc = fopen(doc_path, "r");
  free(doc_path);

  if (fseek(doc, 0, SEEK_END) != 0) goto abort;
  int s_doc = ftell(doc);
  if (fseek(doc, 0, SEEK_SET) != 0) goto abort;
  contents = malloc((s_doc+1)*sizeof(char));
  if (s_doc*sizeof(char) != fread(contents, sizeof(char), s_doc, doc)) goto abort;
  contents[s_doc] = '\0';

  // Index contents to db
  sqlite3_stmt* query;

  // Create note or set existing note to not ready
  if (SQLITE_OK != sqlite3_prepare_v2(nb->index_db, NB_SQL_CREATE_NOTE, -1, &query, NULL)) goto abort;
  if (SQLITE_OK != sqlite3_bind_text(query, 1, file, -1, NULL)) goto abort;
  if (SQLITE_DONE != sqlite3_step(query)) goto abort;
  sqlite3_finalize(query);

  // Obtain note id
  IndexData dt = {nb, nb_note_id(nb, file)};
  if (dt.note_id == 0) goto abort;

  // Delete old search index entries for note
  if (SQLITE_OK != sqlite3_prepare_v2(nb->index_db, NB_SQL_PURGE_SEARCH, -1, &query, NULL)) goto abort;
  if (SQLITE_OK != sqlite3_bind_int(query, 1, dt.note_id)) goto abort;
  if (SQLITE_DONE != sqlite3_step(query)) goto abort;
  sqlite3_finalize(query);

  // Call index callback
  html = parse(contents, h_index_callback, &dt);

  // Update note data and set note to ready
  if (SQLITE_OK != sqlite3_prepare_v2(nb->index_db, NB_SQL_NOTE_FINISH, -1, &query, NULL)) goto abort;
  if (SQLITE_OK != sqlite3_bind_int(query, 1, 1)) goto abort;
  if (SQLITE_OK != sqlite3_bind_int(query, 2, time(NULL))) goto abort;
  if (SQLITE_OK != sqlite3_bind_text(query, 3, html, sdslen(html), NULL)) goto abort;
  if (SQLITE_OK != sqlite3_bind_int(query, 4, dt.note_id)) goto abort;
  if (SQLITE_DONE != sqlite3_step(query)) goto abort;
  sqlite3_finalize(query);

  // This was a great success!
  succ = 1;

  // Cleanup
  abort:;
  fclose(doc);
  if (contents != NULL) free(contents);
  sdsfree(html);
  return succ;
}

// Updates index if the index is not present in the
// database or if it is outdated.
int nb_index_if_outdated(Notebook* nb, const char* file) {
  // Acquire last updated from index
  sqlite3_stmt* query;
  int updated = 0;
  if (SQLITE_OK != sqlite3_prepare_v2(nb->index_db, NB_SQL_NOTE_UPDATED, -1, &query, NULL)) return 0;
  if (SQLITE_OK != sqlite3_bind_text(query, 1, file, -1, NULL)) return 0;
  if (SQLITE_ROW == sqlite3_step(query)) {
    updated = sqlite3_column_int(query, 0);
  }
  sqlite3_finalize(query);

  // Load last updated from file and updated if newer than index
  char* file_path = nb_file_path(nb, file);
  struct stat attr;
  stat(file_path, &attr);
  free(file_path);
  if (attr.st_mtime > updated) return nb_index_file(nb, file);
  return 1;
}

// Helper function, recursively index directory
void h_index_rec(Notebook* nb, const char* dir) {
  DIR* d = opendir(dir);
  if (d == NULL) return;

  struct dirent* entry;
  struct stat attr;
  sds path_buf = sdscat(sdsnew(dir), "/");
  int buf_base_len = sdslen(path_buf);
  int root_len = strlen(nb->root) + 1; // including '/'

  while((entry = readdir(d))) {
    // Ignore system files
    if (entry->d_name[0] == '.') continue;
    path_buf = sdscat(path_buf, entry->d_name);

    stat(path_buf, &attr);

    // Descend into folders
    if (S_ISDIR(attr.st_mode)) {
      h_index_rec(nb, path_buf);
    }

    // Index markdown files
    if (
      S_ISREG(attr.st_mode) &&
      sdslen(path_buf) >= 4 &&
      path_buf[sdslen(path_buf) - 3] == '.' &&
      path_buf[sdslen(path_buf) - 2] == 'm' &&
      path_buf[sdslen(path_buf) - 1] == 'd'
    ) {
      nb_index_if_outdated(nb, &path_buf[root_len]);
    }

    // Reset buffer
    sdssetlen(path_buf, buf_base_len);
  }

  sdsfree(path_buf);
}

// Walk the directory and index any markdown files
// found inside by calling index_if_outdated.
int nb_index(Notebook* nb) {
  h_index_rec(nb, nb->root);
  return 1; // TODO: Implement error checking
}
