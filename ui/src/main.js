import {h, render, Component} from "preact";
import {ToolbarComponent} from './component/toolbar.js';
import {NoteComponent} from "./component/note.js";


class App extends Component {
  constructor() {
    super();
    this.state.note = undefined;
  }

  selectNote(note) {
    this.setState({note: note});
  }

  render(props, state) {
    return (<div class="app">
      <ToolbarComponent onSelect={(note) => {this.selectNote(note)}} />
      <NoteComponent note={state.note} />
    </div>);
  }
}

render(<App />, document.body);
