#ifndef EVAL_H
#define EVAL_H

#define NOB_STRIP_PREFIX
#include "nob.h"

#include "lexer.h"
#include "parser.h"
#include <math.h>

typedef union {
  DataValue some;
  bool none;
} OptionNumber;

typedef enum {
  RT_Int,
  RT_Float,
  RT_Error,
} RetType;

typedef struct {
  OptionNumber opt_num;
  RetType ret_type;
} RetValue;

typedef struct {
  char *key;
  RetValue value;
} MapStrRV;

RetValue eval(const RPNArray *rpn, MapStrRV **map);
void print_rv(RetValue rv);

#endif // EVAL_H
