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
    this.state.root = null;
    this.state.closed = new Set();
  }

  updateNotes() {
    NoteFolder.root(root_folder => {
      this.setState({root: root_folder});
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
    if (folder === null)
      return [];

    let items = [];

    // Render notes
    folder.notes.forEach(note => {
      items.push(
        <li class="note icon-file" onClick={() => onselect(note)}>{note.title}</li>
      );
    });

    // Render folders
    folder.folders.forEach(sub_folder => {
      const closed = this.state.closed.has(sub_folder.file);
      const classList = "folder" + (closed ? " collapsed" : "");
      const sub_items = closed ? [] : this.renderFolder(sub_folder, onselect);
      items.push(
        <li class={classList}>
          <span class="icon-folder" onClick={() => this.toggleFolder(sub_folder)}>{sub_folder.title}</span>
          <ul>{sub_items}</ul>
        </li>
      );
    });

    return items;
  }

  render(props, state) {
    const onselect = typeof props.onSelect === "function" ? props.onSelect : () => {};
    return <ul class="note-list">{this.renderFolder(state.root, onselect)}</ul>;
  }
}
