// Implements a component that displays
// a given note

import {h, Component} from "preact";
import Markup from "preact-markup/src";
import {open} from "../message.js";


function Equation(props) {
  const html = katex.renderToString("".concat(props.children), {
    throwOnError: false,
    displayMode: props.type === "display",
  });

  return <Markup markup={html} />;
}

function A(props) {
  // We need to handle opening links ourselves
  // TODO: Recognize: Other notes, YouTube, Twitter, ...
  return <span class="link" onClick={() => open(props.href)}> {props.children} </span>
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
      return <div class="empty icon-file"></div>;
    this.updateNote(props.note);
    return <Markup class="markdown" markup={state.markup} components={{Equation, A}} />
  }
}
