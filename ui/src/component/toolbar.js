// This component provides the floating
// tool bar.

import {h, Component} from "preact";
import {NoteListComponent} from "./notelist.js"


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

  render(props, state) {
    const onselect = typeof props.onSelect === "function" ? props.onSelect : () => {};

    let title = "";
    let content = "";
    const classList = state.extended !== "" ? "toolbar extended": "toolbar";

    switch (state.extended) {
      case "notes":
        title = <h1 class="title">Notes</h1>;
        content = <span class="content"><NoteListComponent onSelect={onselect} /></span>;
        break;
    }

    return (<div class={classList}>
      {title}{content}
      <span class="actions">
        <a class="button icon-folder" onClick={() => this.toggleNotes()}></a>
        <label class="icon-search" for="search"></label>
        <input id="search" name="search" type="text" placeholder="search" />
      </span>
    </div>);
  }
}
