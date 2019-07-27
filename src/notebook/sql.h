#pragma once

// This header contains all SQL code used for
// the notebook index.

#define NB_SQL_INIT ""\
  "CREATE TABLE IF NOT EXISTS notes ("\
  "id       INTEGER PRIMARY KEY AUTOINCREMENT,"\
  "ready    INTEGER NOT NULL,"\
  "updated  INTEGER NOT NULL,"\
  "file     TEXT UNIQUE NOT NULL,"\
  "html     TEXT);"\
\
  "CREATE TABLE IF NOT EXISTS search ("\
  "id       INTEGER PRIMARY KEY AUTOINCREMENT,"\
  "note     INTEGER NOT NULL,"\
  "type     INTEGER NOT NULL,"\
  "value    TEXT NOT NULL,"\
  "FOREIGN KEY (note) REFERENCES notes(id));"

#define NB_SQL_CREATE_NOTE ""\
  "INSERT INTO notes (ready, updated, file) VALUES (0, 0, ?) ON CONFLICT(file) DO UPDATE SET ready = 0;"

#define NB_SQL_NOTE_ID ""\
  "SELECT id FROM notes WHERE file = ?;"

#define NB_SQL_NOTE_UPDATED ""\
  "SELECT updated FROM notes WHERE file = ?;"

#define NB_SQL_NOTE_FINISH ""\
  "UPDATE notes SET ready = ?, updated = ?, html = ? WHERE id = ?;"

#define NB_SQL_INSERT_SEARCH ""\
  "INSERT INTO search (note, type, value) VALUES (?, ?, ?);"

#define NB_SQL_PURGE_SEARCH ""\
  "DELETE FROM search WHERE note = ?"

#define NB_SQL_LIST_NOTES ""\
  "SELECT file FROM notes WHERE ready = 1 ORDER BY file;"

#define NB_SQL_GET_NOTE ""\
  "SELECT html FROM notes WHERE file = ? AND ready = 1 LIMIT 1;"

// NOTE: We have to use sqlite3_mprintf(), since we want to interpolate *inside* the string!
#define NB_SQL_SEARCH ""\
  "SELECT type, (SELECT file FROM notes WHERE id=note LIMIT 1), value FROM search WHERE value LIKE \"%%%q%%\";"
