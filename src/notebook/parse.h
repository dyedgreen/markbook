#pragma once

#include "sds/sds.h"
#include "md4c/md4c.h"


#define PARSER_FLAGS (\
  MD_FLAG_NOHTML | \
  MD_FLAG_COLLAPSEWHITESPACE | \
  MD_FLAG_STRIKETHROUGH | \
  MD_FLAG_PERMISSIVEAUTOLINKS | \
  MD_FLAG_LATEX)

sds parse(const char* text);
