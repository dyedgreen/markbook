// Hello World (I guess)
import {hello} from './world';
import 'preact';

class Greet {

  constructor(name) {
    this.name = "".concat(name);
  }

  hello() {
    hello(this.name);
  }

}

const my_greet = new Greet("Carolina, I love you");
my_greet.hello();

import { h, render } from 'preact';

render((h("h1", {}, "Hello World!")), document.body);
