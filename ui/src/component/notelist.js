// Implements a component that
// displays a list of notes in the
// opened notebook.
//
// TODO: Subscribe to note_updates once implemented

import {h, Component} from "preact";
import {NoteFolder} from "../notebook.js";


export class NoteListComponent extends Component {
  constructor() {
    super();
    this.state.root = new NoteFolder("");
    this.state.closed = new Set();
  }

  updateNotes() {
    NoteFolder.root(root_folder => {
      this.setState({ root: root_folder });
    });
  }

  componentWillMount() {
    this.updateNotes();
  }

  toggleFolder(folder) {
    if (this.state.closed.has(folder.file)) {
      this.state.closed.delete(folder.file);
    } else {
      this.state.closed.add(folder.file);
    }
    this.setState({ closed: this.state.closed });
  }

  renderFolder(folder, onselect) {
    const closed = this.state.closed.has(folder.file);
    const notes = closed ? "" : folder.notes.map(note =>
      <li class="note" onClick={() => onselect(note)}>[NOTE] {note.title}</li>
    );
    const folders = closed ? "" : folder.folders.map(folder => this.renderFolder(folder, onselect));
    return (
      <li class="folder">
        <span onClick={() => this.toggleFolder(folder)}>[FOLDER] {folder.title}</span>
        <ul>
          {notes}
          {folders}
        </ul>
      </li>
    );
  }

  render(props, state) {
    const onselect = typeof props.onSelect === "function" ? props.onSelect : () => {};
    return <ul>{this.renderFolder(state.root, onselect)}</ul>
  }
}
