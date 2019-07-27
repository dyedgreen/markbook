// Communication to C part

// Messages are serialized, i.e. the worker
// in C will respond to messages in the order
// they were sent.
let message_queue = new Map();

// Make respond function available to C
window.external.c_response_handle = function(id, raw_message) {
  console.log("Got response", raw_message, id);
  handlers.get(id.split("-")[0])(raw_message, message_queue.get(id));
  message_queue.delete(id);
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

function handle_search(raw_message, callback) {
  let results = [];
  let rows = raw_message.split("\n");
  for (let i = 0; i+2 < rows.length; i += 3) {
    results.push({
      type: +rows[i],
      note: rows[i+1],
      value: rows[i+2],
    });
  }
  callback(results);
}

function handle_discard() {
  // no op
}

const types = new Map();
types.set("list_notes", "a");
types.set("get_note", "b");
types.set("search", "c");
types.set("get_root", "d");
types.set("open", "e");

const handlers = new Map();
handlers.set("a", handle_string_list);
handlers.set("b", handle_verbatim);
handlers.set("c", handle_search);
handlers.set("d", handle_verbatim);
handlers.set("e", handle_discard);

const subscribers = new Map();
types.forEach((key, val) => subscribers.set(val, []));


// Message functions

function send_message(callback, type, ...fragments) {
  const id = `${type}-${Math.random().toString(36).substring(7)}`;
  // Queue callback
  message_queue.set(id, typeof callback === "function" ? callback : () => {});
  // Send message
  window.external.invoke(id);
  fragments.forEach(fragment => window.external.invoke(`${fragment}`));
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

// Search the node index
export function search(query, callback) {
  if (typeof query !== "string") throw "No query given.";
  send_message(callback, types.get("search"), query);
}

// Retrieve the notebook root folder
export function get_root(callback) {
  send_message(callback, types.get("get_root"));
}

export function open(url) {
  send_message(null, types.get("open"), `${url}`);
}
