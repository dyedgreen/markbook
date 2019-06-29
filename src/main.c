#include <stdio.h>
#include "notebook/parse.h"

int main() {
  printf("Hello World! Let's parse something...\n\n");

  const char* markdown = \
    "# Hello World\n"\
    "\n"\
    "This is some text\n"\
    "in a paragraph.\n"\
    "It contains some [links](https://tilman.xyz)!\n"\
    "\n"\
    "![this is a description](https://tilman.xyz/assets/hero.png)\n"\
    "\n"\
    "This is a new\n"\
    "paragraph!\n"\
    "\n";


  char* json = (char*)render_json(markdown);
  printf("%s\n", json); // should free sds string here

  return 0;
}
