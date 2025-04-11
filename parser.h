#ifndef PARSER_H
#define PARSER_H

#define NOB_STRIP_PREFIX
#include "lexer.h"
#include "nob.h"

typedef enum {
  TC_Number,
  TC_Operator,
  TC_Parens,
  TC_Illegal,
} TokenClass;

typedef struct {
  Data token;
  TokenClass tc;
} RPNToken;

typedef struct {
  RPNToken *items;
  size_t count;
  size_t capacity;
} RPNArray;

void print_ra(const RPNArray *ra);
RPNArray compress_add_sub(DataArray *data);
RPNArray infix_to_rpn(const RPNArray *ra);

#endif // PARSER_H
