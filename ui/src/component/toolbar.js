// This component provides the floating
// tool bar.

import {h, Component} from "preact";
import {NoteListComponent} from "./notelist.js"
import {search} from "../message.js";
import {Note} from "../notebook.js";
import {Equation} from "./note.js"


export class ToolbarComponent extends Component {
  constructor() {
    super();
    this.state.extended = "";
  }

  toggleNotes() {
    if (this.state.extended !== "notes") {
      this.setState({extended: "notes"});
    } else {
      this.setState({extended: ""});
    }
  }

  hide() {
    this.setState({extended: ""});
  }

  updateSearch(e) {
    this.setState({query: e.target.value});
    if (e.target.value === "") {
      this.hide();
    } else {
      search(e.target.value, results => {
        this.setState({results, extended: "search"});
      });
    }
  }

  renderSearchItem(result, onselect) {
    const note = new Note(result.note);
    let content = "";
    switch (result.type) {
      // Equation
      case 0:
        content = Equation({children: result.value});
        break;
      // Code
      case 7:
        content = <code>{result.value}</code>
        break;
      // Heading
      default:
        content = result.value;
        break;
    }
    return <span class="item" onClick={() => { this.hide(); onselect(note); }}>{content}</span>;
  }

  render(props, state) {
    const onselect = typeof props.onSelect === "function" ? props.onSelect : () => {};

    let title = "";
    let content = "";
    const classList = state.extended !== "" ? "toolbar extended": "toolbar";

    switch (state.extended) {
      case "notes":
        title = <h1 class="title">Notes</h1>;
        content = <span class="content"><NoteListComponent onSelect={(...args) => { this.hide(); onselect(...args); }} /></span>;
        break;
      case "search":
        title = <h1 class="title">Search</h1>;
        let results = state.results.map(r => this.renderSearchItem(r, onselect));
        content = <span class="content">{results}</span>;
        break;
    }

    return (<div class={classList}>
      <span>
        {title}{content}
      </span>
      <span class="actions">
        <a class="button icon-folder" onClick={() => this.toggleNotes()}></a>
        <label class="icon-search" for="search"></label>
        <input id="search" name="search" type="text" placeholder="search" value={state.query} onInput={e => this.updateSearch(e)} />
      </span>
    </div>);
  }
}
