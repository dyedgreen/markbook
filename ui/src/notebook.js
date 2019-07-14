// Classes that simplify dealing with
// notes.

import {list_notes, get_note} from "./message.js";


// Represents an object
// in the file system.
class File {
  constructor(file) {
    this.file = `${file}`;
  }

  // Return display title
  get title() {
    // We follow a few rules to construct
    // titles:
    // _,- => [SPACE]
    // .md => is stripped
    let title = this.file.split("/").reverse()[0];
    title = title.replace(".md", "");
    return title.replace(/(_|-)+/g, " ");
  }

  // Path excluding the file itself
  get path() {
    return this.file.split("/").slice(0, -1).join("/");
  }
}

// Represents a single note
export class Note extends File {
  // Pass note html to callback
  html(callback) {
    get_note(this.file, callback);
  }
}

// Represents a folder containing
// notes.
export class NoteFolder extends File {
  constructor(file) {
    super(file);
    this.notes = [];
    this.folders = [];
  }

  // Retrieve note list and
  // construct folders and notes
  static root(callback) {
    if (typeof callback !== "function")
      return;
    list_notes(files => {
      // Add each note the the correct folder
      let root = new NoteFolder("");
      let folder_map = new Map();
      folder_map.set("", root);

      files.forEach(file => {
        const note = new Note(file);
        let c_path = "";
        // Create folders containing note if they
        // don't exist
        note.path.split("/").forEach(path_part => {
          if (!folder_map.has(c_path + path_part)) {
            const folder = new NoteFolder(c_path + path_part);
            folder_map.set(folder.file, folder);
            folder_map.get(c_path).folders.push(folder);
          }
          c_path += path_part;
        });
        // Add note to correct folder
        folder_map.get(c_path).notes.push(note);
      });
      callback(root);
    });
  }
}
