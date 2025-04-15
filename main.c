#include "eval.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>

#include <readline/history.h>
#include <readline/readline.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define MAX_BUF_LEN 100

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
  MapStrRV *map = NULL;
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
    print_ra(&compressed);
    printf("-------------------------\n");
    print_ra(&rpn);
    printf("-------------------------\n");
#endif

    RetValue res = eval(&rpn, &map);

#ifdef DEBUG
    printf("Assigned variables: \n");
    for (int i = 0; i < shlen(map); i++) {
      printf("{ident: %s, ", map[i].key);
      switch (map[i].value.ret_type) {
      case RT_Int: {
        printf("value: %ld}\n", map[i].value.opt_num.some.int_val);
      } break;
      case RT_Float: {
        printf("value: %lf}\n", map[i].value.opt_num.some.float_val);
      } break;
      default:
        printf("value: undefined}\n");
      }
    }
    printf("-------------------------\n");
#endif

    print_rv(res);
  }
  clear_history();
  free(input);
  da_free(tokens);
  da_free(compressed);
  da_free(rpn);
  return 0;
quit:
  clear_history();
  da_free(tokens);
  da_free(compressed);
  da_free(rpn);
  free(input);
  return 0;
fail:
  clear_history();
  da_free(tokens);
  da_free(compressed);
  da_free(rpn);
  free(input);
  return 1;
}
