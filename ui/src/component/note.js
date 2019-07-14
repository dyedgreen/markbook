// Implements a component that display
// a given note

import {h, Component} from "preact";
import Markup from "preact-markup/src";


function Equation(props) {
  // TODO: Include KaTeX rendering
  return props.type === "display" ?
    <div  class="math" style="color:red">{props.children}</div>:
    <span class="math" style="color:red">{props.children}</span>;
}

export class NoteComponent extends Component {
  constructor() {
    super();
    this.note = undefined;
    this.state.markup = "";
  }

  updateNote(note) {
    if (this.note !== undefined && this.note.file === note.file)
      return;
    this.note = note;
    note.html(html => this.setState({markup: html}));
  }

  render(props, state) {
    if (!props.note)
      return <h2>No Note Selected</h2>;
    this.updateNote(props.note);
    return <Markup markup={state.markup} components={{Equation}}/>
  }
}
