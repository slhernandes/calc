#ifndef LEXER_H
#define LEXER_H
#include <stddef.h>
#define NOB_STRIP_PREFIX
#include "nob.h"

typedef enum {
  TT_LeftParen,
  TT_RightParen,
  TT_Add,
  TT_Sub,
  TT_Div,
  TT_IntDiv,
  TT_Mult,
  TT_Mod,
  TT_SignPos,
  TT_SignNeg,
  TT_Exp,
  TT_NumberInt,
  TT_NumberFloat,
  TT_Illegal
} TokenType;

typedef union {
  long int_val;
  double float_val;
} Number;

typedef struct {
  TokenType type;
  Number data;
} Data;

typedef struct {
  Data *items;
  size_t count;
  size_t capacity;
} DataArray;

void skip_whitespaces(char *str_in);
size_t read_num(char *str_in, Data *data);
void tokenize(char *str_in, DataArray *tokens);
void print_da(DataArray *da);

#endif
