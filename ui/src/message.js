// Communication to C part

// Messages are serialized, i.e. the worker
// in C will respond to messages in the order
// they were sent.
let message_queue = {
  callbacks: [],
  types: [],
};

// Make respond function available to C
window.external.c_response_handle = function(raw_message) {
  console.log("Got response", raw_message, message_queue.types[0]);
  handlers.get(message_queue.types.shift())(raw_message, message_queue.callbacks.shift())
}

window.external.c_notify_handle = function(type) {
  console.log("Got notification of type", type);
  // TODO: This is not yet implemented on the C side of things
  subscribers.get(type).forEach(callback => callback());
}


// Message handlers

function handle_string_list(raw_message, callback) {
  let notes = raw_message.split("\n");
  callback(notes);
}

function handle_verbatim(raw_message, callback) {
  callback(raw_message);
}


const types = new Map();
types.set("list_notes", "a");
types.set("get_note", "b");
types.set("search", "c");
types.set("get_root", "d")
types.set("update_notes", "e")

const handlers = new Map();
handlers.set("a", handle_string_list);
handlers.set("b", handle_verbatim);
handlers.set("d", handle_verbatim);

const subscribers = new Map();
subscribers.set("a", []);
subscribers.set("b", []);
subscribers.set("c", []);
subscribers.set("d", []);


// Message functions

function send_message(callback, type, ...fragments) {
  window.external.invoke(type);
  fragments.forEach(fragment => window.external.invoke(`${fragment}`));
  message_queue.callbacks.push(typeof callback === "function" ? callback : () => {});
  message_queue.types.push(type);
}

// Retrieve a list with all available notes
export function list_notes(callback) {
  send_message(callback, types.get("list_notes"));
}

// Retrieve the rendered HTML for a given note
export function get_note(note, callback) {
  if (typeof note !== "string") throw "No note given.";
  send_message(callback, types.get("get_note"), note);
}

// Retrieve the notebook root folder
export function get_root(callback) {
  send_message(callback, types.get("get_root"));
}

// Register for notifications

export function subscribe(type, callback) {
  subscribers.get(types.get(type)).push(callback);
}
