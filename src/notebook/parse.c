#include <string.h>
#include "parse.h"



/* Parse State */

typedef struct ParseState {
  char mask;       // bit mask with state
  int quote_depth; // How many quotes are open
  sds indx_buf;    // buffer for storing strings to index
  sds html;        // buffer for resulting html
} ParseState;

// Bit mask values
static char m_image_open = 1<<0;
static char m_link_open  = 1<<1;
static char m_quote_open = 1<<2;


/* Render Helpers */

// Escape " character
sds cat_attr(sds html, const char* str, size_t size) {
  size_t len = 0;
  size_t off = 0;
  for (size_t i = 0; i < size; i ++, len += sizeof(char)) {
    if (str[i] == '"') {
      // Copy and add quote
      html = sdscatlen(html, str+off, len);
      html = sdscat(html, "&quot;");
      off = len+sizeof(char);
      len = 0;
    }
  }
  return sdscatlen(html, str+off, len);
}

// <pre><code (lang="detail->lang")>
sds cat_code(sds html, void* detail) {
  MD_BLOCK_CODE_DETAIL* d = (MD_BLOCK_CODE_DETAIL*) detail;
  if (d->lang.size == 0) {
    return sdscat(html, "<pre><code>");
  } else {
    html = sdscat(html, "<pre><code lang=\"");
    html = cat_attr(html, d->lang.text, d->lang.size);
    return sdscat(html, "\">");
  }
}

// <a href="detail->href" (title="detail->title")>
sds cat_a(sds html, void* detail) {
  MD_SPAN_A_DETAIL* d = (MD_SPAN_A_DETAIL*) detail;
  html = sdscat(html, "<a href=\"");
  html = cat_attr(html, d->href.text, d->href.size);
  if (d->title.size > 0) {
    html = sdscat(html, "\" title=\"");
    html = cat_attr(html, d->title.text, d->title.size);
  }
  return sdscat(html, "\">");
}

// <img src="detail->src" (title="detail->title") alt=" (... fill in subsequently and close tag ...)
sds cat_img(sds html, void* detail) {
  MD_SPAN_IMG_DETAIL* d = (MD_SPAN_IMG_DETAIL*) detail;
  html = sdscat(html, "<img src=\"");
  html = cat_attr(html, d->src.text, d->src.size);
  if (d->title.size > 0) {
    html = sdscat(html, "\" title=\"");
    html = cat_attr(html, d->title.text, d->title.size);
  }
  return sdscat(html, "\" alt=\"");
}


/* MD4C Callbacks */

static int cb_enter_block(MD_BLOCKTYPE type, void* detail, void* userdata) {

  ParseState* s = (ParseState*)userdata;

  // Ignore tags if an image is open
  if (s->mask & m_image_open) {
    return 0;
  }

  switch(type) {
    case MD_BLOCK_DOC:      /* nothing */ break;
    case MD_BLOCK_QUOTE:
      s->html = sdscat(s->html, "<blockquote>");
      s->mask |= m_quote_open;
      s->quote_depth ++;
      break;
    case MD_BLOCK_UL:       s->html = sdscat(s->html, "<ul>"); break;
    case MD_BLOCK_OL:       s->html = sdscatfmt(s->html, "<ol start=\"%u\">", ((MD_BLOCK_OL_DETAIL*) detail)->start); break;
    case MD_BLOCK_LI:       s->html = sdscat(s->html, "<li>"); break;
    case MD_BLOCK_HR:       s->html = sdscat(s->html, "<hr>"); break;
    case MD_BLOCK_H:        s->html = sdscatfmt(s->html, "<h%u>", ((MD_BLOCK_H_DETAIL*) detail)->level); break;
    case MD_BLOCK_CODE:     s->html = sdscatfmt(s->html, "<pre><code>"); break;
    case MD_BLOCK_P:        s->html = sdscat(s->html, "<p>"); break;
    default:                /* nothing, we don't have tables or html blocks */ break;
  }

  return 0;
}
static int cb_leave_block(MD_BLOCKTYPE type, void* detail, void* userdata) {

  ParseState* s = (ParseState*)userdata;

  // Ignore tags if an image is open
  if (s->mask & m_image_open) {
    return 0;
  }

  switch(type) {
    case MD_BLOCK_DOC:      /* nothing */ break;
    case MD_BLOCK_QUOTE:
      s->html = sdscat(s->html, "</blockquote>");
      s->quote_depth --;
      if (s->quote_depth == 0) {
        s->mask ^= m_quote_open;
      }
      break;
    case MD_BLOCK_UL:       s->html = sdscat(s->html, "</ul>"); break;
    case MD_BLOCK_OL:       s->html = sdscat(s->html, "</ol>"); break;
    case MD_BLOCK_LI:       s->html = sdscat(s->html, "</li>"); break;
    case MD_BLOCK_HR:       /* nothing */ break;
    case MD_BLOCK_H:        s->html = sdscatfmt(s->html, "</h%u>", ((MD_BLOCK_H_DETAIL*) detail)->level); break;
    case MD_BLOCK_CODE:     s->html = sdscat(s->html, "</code></pre>"); break;
    case MD_BLOCK_P:        s->html = sdscat(s->html, "</p>"); break;
    default:                /* nothing, we don't have tables or html blocks */ break;
  }

  return 0;
}

static int cb_enter_span(MD_SPANTYPE type, void* detail, void* userdata) {

  ParseState* s = (ParseState*)userdata;

  // Ignore formatting inside an image
  if (s->mask & m_image_open) {
    return 0;
  }

  switch(type) {
    case MD_SPAN_EM:        s->html = sdscat(s->html, "<em>"); break;
    case MD_SPAN_STRONG:    s->html = sdscat(s->html, "<strong>"); break;
    case MD_SPAN_CODE:      s->html = sdscat(s->html, "<code>"); break;
    case MD_SPAN_DEL:       s->html = sdscat(s->html, "<del>"); break;
    case MD_SPAN_A:         s->html = cat_a(s->html, detail); break;
    case MD_SPAN_IMG:
      s->html = cat_img(s->html, detail);
      s->mask |= m_image_open;
      break;
  }

  return 0;
}
static int cb_leave_span(MD_SPANTYPE type, void* detail, void* userdata) {

  ParseState* s = (ParseState*)userdata;

  // Ignore formatting inside an image
  if (s->mask & m_image_open && type != MD_SPAN_IMG) {
    return 0;
  }

  switch(type) {
    case MD_SPAN_EM:        s->html = sdscat(s->html, "</em>"); break;
    case MD_SPAN_STRONG:    s->html = sdscat(s->html, "</strong>"); break;
    case MD_SPAN_CODE:      s->html = sdscat(s->html, "</code>"); break;
    case MD_SPAN_DEL:       s->html = sdscat(s->html, "</del>"); break;
    case MD_SPAN_A:         s->html = sdscat(s->html, "</a>"); break;
    case MD_SPAN_IMG:
      s->html = sdscat(s->html, "\">");
      s->mask ^= m_image_open;
      break;
  }

  return 0;
}

static int cb_text(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size, void* userdata) {

  ParseState* s = (ParseState*)userdata;

  switch(type) {
    case MD_TEXT_NORMAL:    /* fall through */
    case MD_TEXT_ENTITY:    /* fall through */
    case MD_TEXT_HTML:      /* fall through */
    case MD_TEXT_CODE:      s->html = sdscatlen(s->html, text, size); break;
    case MD_TEXT_NULLCHAR:  /* nothing (breaking common mark compliance) */ break;
    case MD_TEXT_BR:        /* nothing */ break;
    case MD_TEXT_SOFTBR:    s->html = sdscat(s->html, "\n"); break;
  }

  return 0;
}


/* Public API */

// Renders a given markdown text to html.
// This will later also index the note and
// add everything to the database, or maybe take
// a callback for recording items to an index...
// I think the second approach sounds better :)
sds parse(const char* text) {
  // Parser state and md4c parser
  ParseState s = {
    0,
    0,
    NULL,
    NULL,
  };
  s.indx_buf = sdsempty();
  s.html = sdsempty();

  MD_PARSER parser = {
    0,
    MD_FLAG_NOHTML | MD_FLAG_COLLAPSEWHITESPACE | MD_FLAG_STRIKETHROUGH,
    cb_enter_block,
    cb_leave_block,
    cb_enter_span,
    cb_leave_span,
    cb_text,
    NULL,
    NULL,
  };

  // All the really hard work
  md_parse(text, strlen(text), &parser, (void*)&s);

  // Free temporary buffers and return result
  sdsfree(s.indx_buf);
  return s.html;
}
