#include "debug.h"
#include "message.h"


// Append the finished current context to the queue
void append_current(MessageQueue* queue) {
  MessageContext* last = queue->first;
  while (last != NULL && last->next != NULL)
    last = last->next;
  if (last == NULL) {
    queue->first = queue->current;
  } else {
    last->next = queue->current;
  }
  queue->current = NULL;
}

// Remove first message from queue
MessageContext* pop_first(MessageQueue* queue) {
  MessageContext* ctx = queue->first;
  if (queue->first != NULL) {
    queue->first = queue->first->next;
  }
  return ctx;
}

// Send a message to JS
void send_message(MessageQueue* queue, const char* msg) {
  // JS encode msg
  size_t enc_size = webview_js_encode(msg, NULL, 0);
  char* msg_enc = malloc(enc_size);
  webview_js_encode(msg, msg_enc, enc_size);
  // Send message to JS land
  sds js_command = sdsempty();
  js_command = sdscatfmt(js_command, "window.external.c_response_handle(\"%s\");", msg_enc);
  webview_eval(queue->webview, js_command);
  sdsfree(js_command);
  free(msg_enc);
}

// Obtain a fresh context
MessageContext* empty_context(char type) {
  MessageContext* ctx = malloc(sizeof(MessageContext));
  ctx->detail = sdsempty();
  ctx->next = NULL;

  // Determine type
  ctx->type = (MessageType)(type - 'a');
  if (ctx->type > MessageTypeUnknown || ctx->type < 0)
    ctx->type = MessageTypeUnknown;

  return ctx;
}

// Free a context and it's detail data
void destroy_context(MessageContext* ctx) {
  // The detail field is not allowed to contain pointers owned by the queue
  // so we may free it here
  if (ctx == NULL) return;
  sdsfree(ctx->detail);
  free(ctx);
}


// Public Interface

MessageQueue* message_init_queue(struct webview* webview) {
  MessageQueue* queue = malloc(sizeof(MessageQueue));
  if (queue == NULL)
    return NULL;

  queue->fragment_count = 0;
  queue->current = NULL;
  queue->first = NULL;
  queue->webview = webview;
  queue->nb = NULL;
  return queue;
}

void close_message_queue(MessageQueue* queue) {
  destroy_context(queue->current);
  while (queue->first != NULL) 
    destroy_context(pop_first(queue));
}

void message_digest_fragment(MessageQueue* queue, char* fragment) {
  if (queue->current == NULL) {
    // We are a new message, create it's context
    queue->current = empty_context(fragment[0]);
    queue->fragment_count = 0;
    DEBUG_PRINT("Revived new message of type %i.\n", queue->current->type);
  }

  switch (queue->current->type) {
    case MessageTypeListNotes:
    case MessageTypeGetRoot:
      // Expect no fragments
      append_current(queue);
      break;
    case MessageTypeGetNote:
    case MessageTypeSearch:
      // Expect one fragment: the (note file | query string)
      // Detail is simply this one string.
      if (queue->fragment_count == 1) {
        queue->current->detail = sdscat(queue->current->detail, fragment);
        append_current(queue);
      }
      break;
    default:
      // Message not recognized, discard
      destroy_context(queue->current);
      queue->current = NULL;
      break;
  }

  // Increase fragment count
  queue->fragment_count ++;
}

void message_respond(MessageQueue* queue) {
  // Obtain first complete message
  MessageContext* ctx = pop_first(queue);
  if (ctx != NULL) {
    switch (ctx->type) {
      case MessageTypeListNotes:
        DEBUG_PRINT("Wanted to list notes.\n");
        if (queue->nb != NULL) {
          DEBUG_PRINT("Notebook available, listing notes.\n");
          sds note_list = nb_api_list_notes(queue->nb);
          if (note_list != NULL) {
            send_message(queue, note_list);
            sdsfree(note_list);
          } else {
            DEBUG_PRINT("Error listing notes.\n");
            send_message(queue, "");
          }
        }
        break;
      case MessageTypeGetNote:
        DEBUG_PRINT("Wanted to get note: %s.\n", ctx->detail);
        if (queue->nb != NULL) {
          DEBUG_PRINT("Notebook available, getting note.\n");
          sds note_html = nb_api_get_note(queue->nb, ctx->detail);
          if (note_html != NULL) {
            send_message(queue, note_html);
            sdsfree(note_html);
          } else {
            DEBUG_PRINT("Error loading note.\n");
            send_message(queue, "");
          }
        }
        break;
      case MessageTypeSearch:
        DEBUG_PRINT("Wanted to search for query: %s.\n", ctx->detail);
        break;
      case MessageTypeGetRoot:
        DEBUG_PRINT("Wanted to get notebook root.\n");
        if (queue->nb != NULL) {
          DEBUG_PRINT("Notebook available, returning root.\n");
            send_message(queue, queue->nb->root);
        }
        break;
      default:
        // Message not recognized
        break;
    }
    // Discard used context
    destroy_context(ctx);
  }
}
