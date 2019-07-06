#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "parse.h"
#include "sql.h"
#include "index.h"


typedef struct IndexData {
  Notebook* nb;
  int note_id;
} IndexData;

int index_callback(IndexType type, const char* value, size_t size, void* userdata) {
  IndexData* dt = (IndexData*) userdata;
  sqlite3_stmt* query;
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
int index_file(Notebook* nb, const char* file) {
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
  html = parse(contents, index_callback, &dt);

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
