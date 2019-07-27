// This component provides the floating
// tool bar.

import {h, Component} from "preact";
import {NoteListComponent} from "./notelist.js"
import {search} from "../message.js";


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
    search(e.target.value, results => {
      this.setState({results, extended: "search"});
    });
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
        let results = state.results.map(r => <span class="search-result" style="margin:10px" onClick={() => { this.hide(); console.log(r.note) }}>{r.value}</span>);
        content = <span class="content">{results}</span>;
        break;
    }

    return (<div class={classList}>
      {title}{content}
      <span class="actions">
        <a class="button icon-folder" onClick={() => this.toggleNotes()}></a>
        <label class="icon-search" for="search"></label>
        <input id="search" name="search" type="text" placeholder="search" value={state.query} onInput={e => this.updateSearch(e)} />
      </span>
    </div>);
  }
}
