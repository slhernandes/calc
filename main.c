#include "eval.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>

#include <getopt.h>
#include <readline/history.h>
#include <readline/readline.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define MAX_BUF_LEN 100

int main(int argc, char **argv) {
  int flag = 0, opt;
  while ((opt = getopt(argc, argv, "n")) != -1) {
    switch (opt) {
    case 'n':
      flag = 1;
      break;
    default: /* '?' */
      fprintf(stderr,
              "Usage: %s [-n]\n\t-d: only output last result. (for piping from "
              "other command)\n",
              argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  char *input;
  DataArray tokens = {0};
  RPNArray compressed = {0}, rpn = {0};
  RetValue res = {0};
  MapStrRV *map = NULL;
  using_history();
  size_t size = 0;
  ssize_t nread = 0;

  while (true) {
    if (!flag) {
      input = readline("calc> ");
    } else {
      nread = getline(&input, &size, stdin);
    }
    if (input == NULL || (flag && nread == -1) || strlen(input) <= 1) {
      if (flag) {
        print_rv(&res, flag);
      }
      goto fail;
    }
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

    res = eval(&rpn, &map);

#ifdef DEBUG
    printf("Assigned variables: \n");
    for (int i = 0; i < shlen(map); i++) {
      printf("{ident: %s, ", map[i].key);
      switch (map[i].value.ret_type) {
      case RT_Int: {
        printf("value: %ld}\n", map[i].value.opt_num.dv.int_val);
      } break;
      case RT_Float: {
        printf("value: %lf}\n", map[i].value.opt_num.dv.float_val);
      } break;
      default:
        printf("value: undefined}\n");
      }
    }
    printf("-------------------------\n");
#endif

    if (!flag) {
      print_rv(&res, flag);
    }
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
