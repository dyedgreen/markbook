#include <stdio.h>
#include "notebook/parse.h"

int main() {
  printf("Hello World! Let's parse something...\n\n");

  const char* markdown = \
    "# Hello World\n"\
    "\n"\
    "This is some text *emph* **strong** *nested **strong** here*.\n"\
    "in a paragraph.\n"\
    "It contains    some [*links*](https://tilman.xyz)!\n"\
    "\n"\
    "> # This is a\n"\
    "> *quote*\n"\
    "> hello.\n"\
    "\n"\
    // "$$\n"\
    // "\\LaTeX \\int_a^b x^2 \\alpha dx\n"\
    // "$$\n"
    "\n"\
    "This is a $a+b=c$ ~deleted here~ list:\n"\
    "This is a $a+b=c$ list:\n"\
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
    "This is a new ~deleted here~ &amp; \\$\n"\
    "paragraph!\n"\
    "\n";


  char* html = (char*)parse(markdown);
  printf("%s\n", html);
  sdsfree(html);

  return 0;
}
