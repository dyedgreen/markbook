import {h, render, Component} from 'preact';
import {list_notes, get_note, subscribe} from './message.js';

let note_list = [];

class NoteList extends Component {

  constructor() {
    super();
    this.state.notes = [];
    // Subscribe to index changes
    subscribe("update_notes", () => list_notes(notes => {
      this.setState({ notes: notes })
    }));
  }

  componentDidMount() {
    list_notes(notes => {
      this.setState({ notes: notes });
    });
  }

  clickHandler(...args) {
    console.log(args);
  }

  render() {
    const notes = this.state.notes.map(note => <li onClick={this.clickHandler}>{note}</li>);
    return <ul>{ notes }</ul>
  }

}

render((
  <div>
    <NoteList />
    <div id="foo">
      <span>Hello, world!</span>
      <button onClick={ e => list_notes(list => {console.log(list); note_list=list;}) }>List Notes</button>
      <button onClick={ e => get_note(note_list[0], note => console.log(note)) }>Get Note</button>
    </div>
  </div>
), document.body);
