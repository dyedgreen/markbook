#pragma once

#include "sds/sds.h"
#include "webview.h"
#include "notebook/notebook.h"

#define MESSAGE_TYPE_LIST_NOTES   'a'
#define MESSAGE_TYPE_GET_NOTE     'b'
#define MESSAGE_TYPE_SEARCH       'c'
#define MESSAGE_TYPE_GET_ROOT     'd'
#define MESSAGE_TYPE_OPEN         'e'

typedef enum {
  MessageTypeListNotes = 0,
  MessageTypeGetNote,
  MessageTypeSearch,
  MessageTypeGetRoot,
  MessageTypeOpen,
  MessageTypeUnknown,
} MessageType;

typedef struct MessageContext {
  MessageType type;
  sds id;                      // Unique message id
  sds detail;                  // Detail data. This is a sds string (but may contain any data)
  struct MessageContext* next;
} MessageContext;

typedef struct MessageQueue {
  int fragment_count;       // Fragments received for current message
  MessageContext* current;  // Currently being assembled
  MessageContext* first;    // Next to be processed
  struct webview* webview;  // Webview used for responding to messages
  Notebook* nb;             // Notebook used for fulfilling queries
} MessageQueue;

// Initialize a new message queue
MessageQueue* message_init_queue(struct webview*);
// Destroy message queue and all contained contexts
void close_message_queue(MessageQueue*);

// Collect a single message fragment
void message_digest_fragment(MessageQueue*, char* /*fragment*/);
// Respond to the message
void message_respond(MessageQueue*);
