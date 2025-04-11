#ifndef EVAL_H
#define EVAL_H

#define NOB_STRIP_PREFIX
#include "lexer.h"
#include "nob.h"
#include "parser.h"
#include <math.h>

typedef union {
  Number some;
  bool none;
} OptionNumber;

typedef enum {
  RT_Int,
  RT_Float,
  RT_Error,
} RetType;

OptionNumber eval(const RPNArray *rpn, RetType *ret_type);
void print_on(OptionNumber on, RetType ret_type);

#endif // EVAL_H
