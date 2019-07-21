#include <string.h>
#include "md4c/md4c.h"
#include "parse.h"

#define PARSER_FLAGS (\
  MD_FLAG_NOHTML | \
  MD_FLAG_COLLAPSEWHITESPACE | \
  MD_FLAG_STRIKETHROUGH | \
  MD_FLAG_PERMISSIVEAUTOLINKS | \
  MD_FLAG_LATEXMATHSPANS)


/* Parse State */

typedef struct ParseState {
  unsigned mask;   // bit mask with state
  int quote_depth; // How many quotes are open
  sds head_buf;    // Buffer for indexing headings
  sds code_buf;    // Buffer for indexing code
  sds eqn_buf;     // Buffer for indexing equations
  sds html;        // buffer for resulting html
  int (*index_callback)(IndexType /*type*/, const char* /*value*/, size_t /*size*/, void* /*userdata*/);
  void* userdata;  // Data supplied to index callback
} ParseState;

// Bit mask values
#define M_IMAGE_OPEN 0x0001
#define M_QUOTE_OPEN 0x0002

#define M_INDEX_HEAD 0x0004
#define M_INDEX_CODE 0x0008
#define M_INDEX_EQN  0x0010


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


/* Index Helpers */

// Open or close an index
#define OPEN_INDEX(index_flag) (s->mask |= (index_flag))
#define CLOSE_INDEX(index_flag) (s->mask = (s->mask | (index_flag)) ^ (index_flag))

// Test if index is open
#define IS_INDEX_OPEN(index_flag) (s->mask & (index_flag) && s->index_callback != NULL)

// Call index function and clean up string after
#define CALL_INDEX(index_flag) { \
    if (s->index_callback == NULL) return 0; \
    IndexType idx_t = 0; \
    sds idx_buf = NULL; \
    if ((index_flag) == M_INDEX_EQN) {\
      idx_buf = s->eqn_buf; \
      idx_t = IndexEquation; \
    } else if ((index_flag) == M_INDEX_HEAD) { \
      idx_buf = s->head_buf; \
      idx_t = ((MD_BLOCK_H_DETAIL*) detail)->level; \
    } else if ((index_flag) == M_INDEX_CODE) { \
      idx_buf = s->code_buf; \
      idx_t = IndexCode; \
    } \
    if (sdslen(idx_buf) == 0) return 0; \
    int idx_res = s->index_callback(idx_t, (const char*)idx_buf, (size_t)sdslen(idx_buf)*sizeof(char), s->userdata); \
    sdsclear(idx_buf); \
    return idx_res; \
  } while(0)


/* MD4C Callbacks */

static int cb_enter_block(MD_BLOCKTYPE type, void* detail, void* userdata) {

  ParseState* s = (ParseState*)userdata;

  // Ignore tags if an image is open
  if (s->mask & M_IMAGE_OPEN) {
    return 0;
  }

  // Render
  switch(type) {
    case MD_BLOCK_DOC:      /* nothing */ break;
    case MD_BLOCK_QUOTE:
      s->html = sdscat(s->html, "<blockquote>");
      s->mask |= M_QUOTE_OPEN;
      s->quote_depth ++;
      break;
    case MD_BLOCK_UL:       s->html = sdscat(s->html, "<ul>"); break;
    case MD_BLOCK_OL:       s->html = sdscatfmt(s->html, "<ol start=\"%u\">", ((MD_BLOCK_OL_DETAIL*) detail)->start); break;
    case MD_BLOCK_LI:       s->html = sdscat(s->html, "<li>"); break;
    case MD_BLOCK_HR:       s->html = sdscat(s->html, "<hr>"); break;
    case MD_BLOCK_H:        s->html = sdscatfmt(s->html, "<h%u>", ((MD_BLOCK_H_DETAIL*) detail)->level); break;
    case MD_BLOCK_CODE:     s->html = cat_code(s->html, detail); break;
    case MD_BLOCK_P:        s->html = sdscat(s->html, "<p>"); break;
    default:                /* nothing, we don't have tables or html blocks */ break;
  }

  // Index
  switch(type) {
    case MD_BLOCK_H:
      sdsclear(s->head_buf);
      OPEN_INDEX(M_INDEX_HEAD);
      break;
    case MD_BLOCK_CODE:
      sdsclear(s->code_buf);
      OPEN_INDEX(M_INDEX_CODE);
      break;
    default:
      /* no-op */
      break;
  }

  return 0;
}

static int cb_leave_block(MD_BLOCKTYPE type, void* detail, void* userdata) {

  ParseState* s = (ParseState*)userdata;

  // Ignore tags if an image is open
  if (s->mask & M_IMAGE_OPEN) {
    return 0;
  }

  // Render
  switch(type) {
    case MD_BLOCK_DOC:      /* nothing */ break;
    case MD_BLOCK_QUOTE:
      s->html = sdscat(s->html, "</blockquote>");
      s->quote_depth --;
      if (s->quote_depth == 0) {
        s->mask ^= M_QUOTE_OPEN;
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

  // Index
  switch(type) {
    case MD_BLOCK_H:
      CALL_INDEX(M_INDEX_HEAD);
      CLOSE_INDEX(M_INDEX_HEAD);
      break;
    case MD_BLOCK_CODE:
      CALL_INDEX(M_INDEX_CODE);
      CLOSE_INDEX(M_INDEX_CODE);
      break;
    default:
      /* no-op */
      break;
  }

  return 0;
}

static int cb_enter_span(MD_SPANTYPE type, void* detail, void* userdata) {

  ParseState* s = (ParseState*)userdata;

  // Ignore formatting inside an image
  if (s->mask & M_IMAGE_OPEN) {
    return 0;
  }

  // Render
  switch(type) {
    case MD_SPAN_EM:            s->html = sdscat(s->html, "<em>"); break;
    case MD_SPAN_STRONG:        s->html = sdscat(s->html, "<strong>"); break;
    case MD_SPAN_CODE:          s->html = sdscat(s->html, "<code>"); break;
    case MD_SPAN_DEL:           s->html = sdscat(s->html, "<del>"); break;
    case MD_SPAN_A:             s->html = cat_a(s->html, detail); break;
    case MD_SPAN_IMG:
      s->html = cat_img(s->html, detail);
      s->mask |= M_IMAGE_OPEN;
      break;
    case MD_SPAN_LATEXMATH:         s->html = sdscat(s->html, "<equation>"); break;
    case MD_SPAN_LATEXMATH_DISPLAY: s->html = sdscat(s->html, "<equation type=\"display\">"); break;
  }

  // Index
  switch(type) {
    case MD_SPAN_LATEXMATH:
    case MD_SPAN_LATEXMATH_DISPLAY:
      sdsclear(s->eqn_buf);
      OPEN_INDEX(M_INDEX_EQN);
      break;
    case MD_SPAN_CODE:
      sdsclear(s->code_buf);
      OPEN_INDEX(M_INDEX_CODE);
      break;
    default:
      /* no-op */
      break;
  }

  return 0;
}

static int cb_leave_span(MD_SPANTYPE type, void* detail, void* userdata) {

  ParseState* s = (ParseState*)userdata;

  // Ignore formatting inside an image
  if (s->mask & M_IMAGE_OPEN && type != MD_SPAN_IMG) {
    return 0;
  }

  // Render
  switch(type) {
    case MD_SPAN_EM:            s->html = sdscat(s->html, "</em>"); break;
    case MD_SPAN_STRONG:        s->html = sdscat(s->html, "</strong>"); break;
    case MD_SPAN_CODE:          s->html = sdscat(s->html, "</code>"); break;
    case MD_SPAN_DEL:           s->html = sdscat(s->html, "</del>"); break;
    case MD_SPAN_A:             s->html = sdscat(s->html, "</a>"); break;
    case MD_SPAN_IMG:
      s->html = sdscat(s->html, "\" />");
      s->mask ^= M_IMAGE_OPEN;
      break;
    case MD_SPAN_LATEXMATH:
    case MD_SPAN_LATEXMATH_DISPLAY: s->html = sdscat(s->html, "</equation>"); break;
  }

  // Index
  switch(type) {
    case MD_SPAN_LATEXMATH:
    case MD_SPAN_LATEXMATH_DISPLAY:
      CALL_INDEX(M_INDEX_EQN);
      CLOSE_INDEX(M_INDEX_EQN);
      break;
    case MD_SPAN_CODE:
      CALL_INDEX(M_INDEX_CODE);
      CLOSE_INDEX(M_INDEX_CODE);
      break;
    default:
      /* no-op */
      break;
  }

  return 0;
}

static int cb_text(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size, void* userdata) {

  ParseState* s = (ParseState*)userdata;

  // Render
  switch (type) {
    case MD_TEXT_NORMAL:    /* fall through */
    case MD_TEXT_ENTITY:    /* fall through */
    case MD_TEXT_HTML:      /* fall through */
    case MD_TEXT_LATEXMATH: /* fall through */
    case MD_TEXT_CODE:      s->html = sdscatlen(s->html, text, size); break;
    case MD_TEXT_NULLCHAR:  /* nothing (breaking common mark compliance) */ break;
    case MD_TEXT_BR:        s->html = sdscat(s->html, "<br>"); break;
    case MD_TEXT_SOFTBR:    s->html = sdscat(s->html, "\n"); break;
  }

  // Index
  if (IS_INDEX_OPEN(M_INDEX_CODE) && type == MD_TEXT_CODE) {
    s->code_buf = sdscatlen(s->code_buf, text, size);
  } else if (IS_INDEX_OPEN(M_INDEX_EQN) && type == MD_TEXT_LATEXMATH) {
    s->eqn_buf = sdscatlen(s->eqn_buf, text, size);
  } else if (IS_INDEX_OPEN(M_INDEX_HEAD)) {
    switch (type) {
      case MD_TEXT_NORMAL:  /* fall through */
      case MD_TEXT_CODE:    s->head_buf = sdscatlen(s->head_buf, text, size); break;
      case MD_TEXT_BR:      /* fall through */
      case MD_TEXT_SOFTBR:  s->head_buf = sdscat(s->head_buf, " "); break;
      default:              /* no-op */ break;
    }
  }

  return 0;
}


/* Public API */

// Renders a given markdown text to html,
// emitting callbacks to the callback.
sds parse(
  const char* text,
  int (*index_callback)(IndexType /*type*/, const char* /*value*/, size_t /*size*/, void* /*userdata*/),
  void* userdata) {
  // Parser state and md4c parser
  ParseState s = {
    0,
    0,
    sdsempty(),
    sdsempty(),
    sdsempty(),
    sdsempty(),
    index_callback,
    userdata,
  };

  MD_PARSER parser = {
    0,
    PARSER_FLAGS,
    cb_enter_block,
    cb_leave_block,
    cb_enter_span,
    cb_leave_span,
    cb_text,
    NULL,
    NULL,
  };

  // All the really hard work
  if(md_parse(text, strlen(text), &parser, (void*)&s) != 0) {
    sdsfree(s.html);
    s.html = NULL;
  }

  // Free temporary buffers and return result
  sdsfree(s.head_buf);
  sdsfree(s.code_buf);
  sdsfree(s.eqn_buf);
  return s.html;
}

// Convenience wrapper for parse.
sds render_html(const char* text) {
  return parse(text, NULL, NULL);
}
