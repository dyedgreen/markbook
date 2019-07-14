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
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/katex@0.10.2/dist/katex.min.css" integrity="sha384-yFRtMMDnQtDRO8rLpMIKrtPCD5jdktao2TV19YiZYWMDkUR5GQZR/NOVTdquEx1j" crossorigin="anonymous" />
    <script defer src="https://cdn.jsdelivr.net/npm/katex@0.10.2/dist/katex.min.js" integrity="sha384-9Nhn55MVVN0/4OFx7EE5kpFBPsEMZxKTCnA+4fqDmg12eCTqGi6+BB2LjY8brQxJ" crossorigin="anonymous"></script>
    <h1>Hello World! Here are the notes:</h1>
    <App />
  </div>
), document.body);
