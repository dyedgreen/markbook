import {h, render} from "preact";
import {NoteListComponent} from './component/notelist.js';


render((
  <div>
    <h1>Hello World! Here are the notes:</h1>
    <NoteListComponent onSelect={(note) => {alert(note.file)}} />
  </div>
), document.body);
