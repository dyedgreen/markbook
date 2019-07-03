#include <stdio.h>
#include "notebook/parse.h"

int indexer(IndexType type, const char* value, size_t size) {
  printf("Indexed type %i: ;;%.*s;;\n", type, size, value);
  return 0;
}

int main() {
  printf("Hello World! Let's parse something...\n\n");

  const char* markdown = \
    "# Hello World\n"\
    "\n"\
    "This is some text *emph* **strong** *nested **strong** here*.\n"\
    "In a paragraph.\n"\
    "It contains    some [*links*](https://tilman.xyz)!\n"\
    "\n"\
    "> ## This is a quote header\n"\
    "> *quote*\n"\
    "> hello.\n"\
    "\n"\
    "$$\n"\
    "\\LaTeX \\int_a^b x^2 \\alpha dx\n"\
    "$$\n"
    "\n"\
    "This is a $a+b=c$ ~deleted here~ list:\n"\
    "This is a $\\int_a^b x^2$ list:\n"\
    "* Hello\n* World\n"\
    "\n"\
    "1) Hi\n 2) There!\n"\
    "\n"\
    "```Python\n"\
    "print('Hello World!')\n"\
    "```\n"\
    "\n"\
    "```\n"\
    "print('Hello World!')\n"\
    "```\n"\
    "\n"\
    "![this *is* a description](https://tilman.xyz/assets/hero.png)\n"\
    "\n"\
    "This is a new ~deleted here~ &amp; with `code!!!` \\$\n"\
    "paragraph!\n"\
    "\n";


  char* html = (char*)parse(markdown, indexer);
  printf("%s\n", html);
  sdsfree(html);

  return 0;
}
