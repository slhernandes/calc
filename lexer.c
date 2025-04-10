#include "lexer.h"
#include <ctype.h>
#include <stddef.h>
#include <string.h>

char *skip_whitespaces(char *str_in) {
  size_t str_size = strlen(str_in);
  while (str_size > 0 && isspace(str_in[0])) {
    str_in++;
    str_size = strlen(str_in);
  }
  return str_in;
}

size_t read_num(char *str_in, Data *data) {
  char *cpy = strdup(str_in);
  size_t str_size = strlen(cpy);
  TokenType t_type = TT_NumberInt;
  size_t ret = 0;
  for (size_t i = 0; i < str_size; i++) {
    if (isdigit(cpy[i])) {
      continue;
    } else if (cpy[i] == '.' && t_type != TT_NumberFloat) {
      t_type = TT_NumberFloat;
    } else {
      cpy[i] = '\0';
      ret = i - 1;
      break;
    }
  }
  data->type = t_type;
  if (data->type == TT_NumberInt) {
    data->data.int_val = atoi(cpy);
  } else {
    data->data.float_val = atof(cpy);
  }
  return ret;
}

void tokenize(char *str_in, DataArray *tokens) {
  char *now = strdup(str_in);
  now = skip_whitespaces(now);
  size_t str_size = strlen(now);
  if (str_size == 0)
    return;
  switch (now[0]) {
  case '(': {
    Data cur = {
        .type = TT_LeftParen,
        .data = {0},
    };
    da_append(tokens, cur);
  } break;
  case ')': {
    Data cur = {
        .type = TT_RightParen,
        .data = {0},
    };
    da_append(tokens, cur);
  } break;
  // case '{': {
  //   Data cur = {
  //       .type = LeftBrace,
  //       .data = {0},
  //   };
  //   da_append(tokens, cur);
  // } break;
  // case '}': {
  //   Data cur = {
  //       .type = RightBrace,
  //       .data = {0},
  //   };
  //   da_append(tokens, cur);
  // } break;
  // case '[': {
  //   Data cur = {
  //       .type = LeftBracket,
  //       .data = {0},
  //   };
  //   da_append(tokens, cur);
  // } break;
  // case ']': {
  //   Data cur = {
  //       .type = RightBracket,
  //       .data = {0},
  //   };
  //   da_append(tokens, cur);
  // } break;
  case '+': {
    Data cur = {
        .type = TT_Add,
        .data = {0},
    };
    da_append(tokens, cur);
  } break;
  case '-': {
    Data cur = {
        .type = TT_Sub,
        .data = {0},
    };
    da_append(tokens, cur);
  } break;
  case '/': {
    TokenType t_type = TT_Div;
    if (str_size > 1 && now[1] == '/') {
      t_type = TT_IntDiv;
      now++;
    }
    Data cur = {
        .type = t_type,
        .data = {0},
    };
    da_append(tokens, cur);
  } break;
  case '*': {
    Data cur = {
        .type = TT_Mult,
        .data = {0},
    };
    da_append(tokens, cur);
  } break;
  case '^': {
    Data cur = {
        .type = TT_Exp,
        .data = {0},
    };
    da_append(tokens, cur);
  } break;
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
  case '.': {
    Data cur = {0};
    size_t offset = read_num(now, &cur);
    now += offset;
    da_append(tokens, cur);
    break;
  }
  default: {
    Data cur = {
        .type = TT_Illegal,
        .data = {0},
    };
    da_append(tokens, cur);
  }
  }
  tokenize(++now, tokens);
}

void lexer_da_free(DataArray *da) { free(da); }

void print_da(DataArray *da) {
  for (size_t i = 0; i < da->count; i++) {
    switch (da->items[i].type) {
    case TT_LeftParen: {
      printf("TT_LeftParen\n");
    } break;
    case TT_RightParen: {
      printf("TT_RightParen\n");
    } break;
    // case LeftBrace: {
    //   printf("LeftBrace\n");
    // } break;
    // case RightBrace: {
    //   printf("RightBrace\n");
    // } break;
    // case LeftBracket: {
    //   printf("LeftBracket\n");
    // } break;
    // case RightBracket: {
    //   printf("RightBracket\n");
    // } break;
    case TT_Add: {
      printf("TT_Add\n");
    } break;
    case TT_Sub: {
      printf("TT_Sub\n");
    } break;
    case TT_Div: {
      printf("TT_Div\n");
    } break;
    case TT_IntDiv: {
      printf("TT_IntDiv\n");
    } break;
    case TT_Mult: {
      printf("TT_Mult\n");
    } break;
    case TT_Exp: {
      printf("TT_Exp\n");
    } break;
    case TT_NumberInt: {
      int num = da->items[i].data.int_val;
      printf("TT_NumberInt(%d)\n", num);
    } break;
    case TT_NumberFloat: {
      float num = da->items[i].data.float_val;
      printf("TT_NumberFloat(%f)\n", num);
    } break;
    case TT_Illegal: {
      printf("TT_Illegal\n");
    } break;
    default:
      assert(0 && "Unreachable!");
    }
  }
}
