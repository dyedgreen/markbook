#pragma once

#include "sds/sds.h"
#include "webview.h"

#define MESSAGE_TYPE_LIST_NOTES 'a'
#define MESSAGE_TYPE_GET_NOTE   'b'
#define MESSAGE_TYPE_SEARCH     'c'

typedef enum {
  MessageTypeListNotes,
  MessageTypeGetNote,
  MessageTypeSearch,
  MessageTypeUnknown,
} MessageType;

typedef struct MessageContext {
  MessageType type;
  sds detail;               // Detail data. This is a sds string (but may contain any data)
  struct MessageContext* next;
} MessageContext;

typedef struct MessageQueue {
  int fragment_count;       // Fragments received for current message
  MessageContext* current;  // Currently being assembled
  MessageContext* first;    // Next to be processed
  struct webview* webview;  // Webview used for responding to messages
} MessageQueue;

// Initialize a new message queue
MessageQueue* message_init_queue(struct webview*);
// Destroy message queue and all contained contexts
void close_message_queue(MessageQueue*);

// Collect a single message fragment
void message_digest_fragment(MessageQueue*, char* /*fragment*/);
// Respond to the message
void message_respond(MessageQueue*);
