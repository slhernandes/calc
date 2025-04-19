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
  TT_Ident,
  TT_Assign,
  TT_Illegal
} TokenType;

typedef union {
  long int_val;
  double float_val;
  char *str_val;
} DataValue;

typedef struct {
  TokenType type;
  DataValue data;
} Data;

typedef struct {
  Data *items;
  size_t count;
  size_t capacity;
} DataArray;

size_t skip_whitespaces(char *str_in);
size_t read_hexbin(char *str_in, Data *data, int base);
size_t read_num(char *str_in, Data *data);
size_t read_ident(char *str_in, Data *data);
void tokenize(char *str_in, DataArray *tokens);
void print_da(DataArray *da);

#endif
