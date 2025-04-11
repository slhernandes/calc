#include "eval.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#define MAX_BUF_LEN 100

int main() {
  FILE *test = fopen("./test.in", "r");
  if (test == NULL) {
    return -1;
  }

  String_Builder sb = {0};
  char buf[MAX_BUF_LEN];
  while (fgets(buf, MAX_BUF_LEN, test) != NULL) {
    sb_append_cstr(&sb, buf);
  }
  sb_append_null(&sb);
  DataArray *tokens = malloc(sizeof(DataArray));
  tokenize(sb.items, tokens);
  RPNArray *compressed = compress_add_sub(tokens);
#ifdef DEBUG
  print_ra(compressed);
  printf("--------------------\n");
#endif
  RPNArray *rpn = infix_to_rpn(compressed);
#ifdef DEBUG
  print_ra(rpn);
  printf("--------------------\n");
#endif
  RetType *rt = malloc(sizeof(RetType));
  OptionNumber res = eval(rpn, rt);
  printf(
      "[\033[1;33mExpr\033[0m]:\n%s\n----------------------------------------"
      "----------------------\n",
      sb.items);
  print_on(res, *rt);
  free(rt);
  lexer_da_free(tokens);
  parser_ra_free(compressed);
  parser_ra_free(rpn);
  return 0;
}
