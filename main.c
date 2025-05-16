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

char *trimwhitespace(char *str) {
  char *end;
  while (isspace((unsigned char)*str))
    str++;
  if (*str == 0)
    return str;
  end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end))
    end--;
  end[1] = '\0';
  return str;
}

void show_help(char *prog_name, FILE *f) {
  fprintf(f,
          "Usage: %s [-n|-h]\n"
          "\t-n: only output last result. (for piping from "
          "other command)\n"
          "\t-h: show this message\n\n"
          "Valid command:\n"
          "\texit: exits the program\n"
          "\thelp: show this page\n"
          "\t0x  : prints previous result in hexadecimal (only if previous "
          "result is integer)\n"
          "\t0b  : prints previous result in binary (only if previous result "
          "is integer)\n\n"
          "Valid operators: "
          "+, -, *, /, //, ^, %%, =, (, )\n",
          prog_name);
}

int main(int argc, char **argv) {
  int flag = 0, opt;
  while ((opt = getopt(argc, argv, "nh")) != -1) {
    switch (opt) {
    case 'n': {
      flag = 1;
    } break;
    case 'h': {
      show_help(argv[0], stdout);
      exit(EXIT_SUCCESS);
    } break;
    default: /* '?' */
      show_help(argv[0], stderr);
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
      input = trimwhitespace(input);
    } else {
      nread = getline(&input, &size, stdin);
    }
    if (input == NULL || (flag && nread == -1) || input[0] == '\n') {
      if (flag) {
        print_rv(&res, flag);
      }
      goto fail;
    }
    if (strlen(input) == 0) {
      continue;
    }
    if (!strcmp(input, "exit")) {
      goto quit;
    } else if (!strcmp(input, "clear")) {
      printf("\e[1;1H\e[2J");
      continue;
    } else if (!strcmp(input, "0x")) {
      switch (res.ret_type) {
      case RT_Int:
      case RT_Int_Bin:
      case RT_Int_Hex: {
        res.ret_type = RT_Int_Hex;
        print_rv(&res, flag);
      } break;
      default:
        OptionNumber temp_on = (OptionNumber){
            .et = ET_NotAnInt,
        };
        RetValue temp_rv = {
            .ret_type = RT_Error,
            .opt_num = temp_on,
            .pos = 0,
        };
        print_rv(&temp_rv, flag);
      }
      continue;
    } else if (!strcmp(input, "0b")) {
      switch (res.ret_type) {
      case RT_Int:
      case RT_Int_Bin:
      case RT_Int_Hex: {
        res.ret_type = RT_Int_Bin;
        print_rv(&res, flag);
        continue;
      } break;
      default:
        OptionNumber temp_on = {
            .et = ET_NotAnInt,
        };
        RetValue temp_rv = {
            .ret_type = RT_Error,
            .opt_num = temp_on,
            .pos = 0,
        };
        print_rv(&temp_rv, flag);
        continue;
      }
    } else if (!strcmp(input, "help")) {
      show_help(argv[0], stdout);
      continue;
    }
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
