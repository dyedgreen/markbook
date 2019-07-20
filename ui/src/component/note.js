// Implements a component that display
// a given note

import {h, Component} from "preact";
import Markup from "preact-markup/src";


function Equation(props) {
  // TODO: Include KaTeX rendering properly (currently it is hot-loaded >.<)
  const html = katex.renderToString("".concat(props.children), {
    throwOnError: false,
    displayMode: props.type === "display",
  });

  return props.type === "display" ?
    <div  class="math" style="color:red"><Markup markup={html} /></div>:
    <span class="math" style="color:red"><Markup markup={html} /></span>;
}

function A(props) {
  // We need to handle opening links manually
  return <span class="link" style="color:blue" onClick={() => {alert("Trying to open: "+props.href)}}>{props.children}</span>
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
    console.log(state);
    return <Markup markup={state.markup} components={{Equation, A}} />
  }
}