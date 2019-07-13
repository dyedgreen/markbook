import {h, render} from 'preact';
import {list_notes, get_note} from './message.js';

render((
    <div id="foo">
        <span>Hello, world!</span>
        <button onClick={ e => list_notes(list => console.log(list)) }>List Notes</button>
        <button onClick={ e => get_note("hello.md", note => console.log(note)) }>Get Note</button>
    </div>
), document.body);
