import {h, render, Component} from "preact";
import {NoteListComponent} from './component/notelist.js';
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
    return (<div>
      <NoteListComponent onSelect={(note) => {this.selectNote(note)}} />
      <NoteComponent note={state.note} />
    </div>);
  }
}

render((
  <div>
    <h1>Hello World! Here are the notes:</h1>
    <App />
  </div>
), document.body);
