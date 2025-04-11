#include "eval.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#define MAX_BUF_LEN 100

#include <readline/history.h>
#include <readline/readline.h>

int main() {
  // Old input
  // FILE *test = fopen("./test.in", "r");
  // if (test == NULL) {
  //   return -1;
  // }
  // String_Builder sb = {0};
  // char buf[MAX_BUF_LEN];
  // while (fgets(buf, MAX_BUF_LEN, test) != NULL) {
  //   sb_append_cstr(&sb, buf);
  // }
  // sb_append_null(&sb);

  char *input;
  DataArray tokens = {0};
  RPNArray compressed = {0}, rpn = {0};
  RetType rt;
  using_history();

  while (true) {
    input = readline("calc> ");
    if (input == NULL)
      goto fail;
    if (!strcmp(input, "exit"))
      goto quit;
    add_history(input);
    tokens.count = 0;
    tokenize(input, &tokens);

    compressed = compress_add_sub(&tokens);
    rpn = infix_to_rpn(&compressed);

#ifdef DEBUG
    print_ra(compressed);
    printf("--------------------\n");
    print_ra(rpn);
    printf("--------------------\n");
#endif

    OptionNumber res = eval(&rpn, &rt);
    // printf(
    //     "[\033[1;33mExpr\033[0m]:
    //     %s\n----------------------------------------"
    //     "----------------------\n",
    //     input);
    print_on(res, rt);
  }
  free(input);
  return 0;
quit:
  free(input);
  return 0;
fail:
  free(input);
  return 1;
}
