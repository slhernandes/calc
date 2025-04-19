#include "lexer.h"
#include <ctype.h>
#include <stddef.h>
#include <string.h>

size_t skip_whitespaces(char *str_in) {
  size_t str_size = strlen(str_in);
  size_t offset = 0;
  while (str_size > offset && isspace(str_in[offset])) {
    offset++;
  }
  return offset;
}

size_t read_hexbin(char *str_in, Data *data, int base) {
  char *cpy = strdup(str_in);
  size_t str_size = strlen(cpy);
  size_t ret = 0;
  bool end = false;
  size_t i = base == 16 ? 2 : 0;
  for (; i < str_size; i++) {
    if (base == 16) {
      switch (cpy[i]) {
      case '0' ... '9':
      case 'a' ... 'f': {
      } break;
      default:
        ret = i - 1;
        cpy[i] = '\0';
        end = true;
        goto done;
        UNREACHABLE("After goto");
      }
    } else {
      switch (cpy[i]) {
      case '0' ... '1': {
      } break;
      default:
        ret = i - 1;
        cpy[i] = '\0';
        end = true;
        goto done;
        UNREACHABLE("After goto");
      }
    }
  }
done:
  if (!end)
    ret = str_size;
  data->type = TT_NumberInt;
  data->data.int_val = strtoul(cpy, (char **)0, base);
  free(cpy);
  return ret;
}

size_t read_num(char *str_in, Data *data) {
  char *cpy = strdup(str_in);
  size_t str_size = strlen(cpy);
  TokenType t_type = TT_NumberInt;
  size_t ret = 0;
  bool end = false;
  for (size_t i = 0; i < str_size; i++) {
    if (isdigit(cpy[i])) {
      continue;
    } else if (cpy[i] == '.' && t_type != TT_NumberFloat) {
      t_type = TT_NumberFloat;
    } else {
      cpy[i] = '\0';
      ret = i - 1;
      end = true;
      break;
    }
  }
  if (!end)
    ret = str_size;
  data->type = t_type;
  if (data->type == TT_NumberInt) {
    data->data.int_val = atoi(cpy);
  } else {
    data->data.float_val = atof(cpy);
  }
  free(cpy);
  return ret;
}

size_t read_ident(char *str_in, Data *data) {
  char *cpy = strdup(str_in);
  size_t str_size = strlen(cpy);
  TokenType t_type = TT_Ident;
  size_t ret = 0;
  bool end = false;
  for (size_t i = 1; i < str_size; i++) {
    if (isalnum(cpy[i]) || cpy[i] == '_') {
      continue;
    } else {
      cpy[i] = '\0';
      ret = i - 1;
      end = true;
      break;
    }
  }
  if (!end)
    ret = str_size;
  data->type = t_type;
  data->data = (DataValue){
      .str_val = cpy,
  };
  return ret;
}

bool next_is_sign(Data token) {
  switch (token.type) {
  case TT_LeftParen:
  case TT_Add:
  case TT_Sub:
  case TT_Mult:
  case TT_Div:
  case TT_IntDiv:
  case TT_Mod:
  case TT_SignPos:
  case TT_SignNeg:
  case TT_Exp: {
    return true;
  } break;
  default:
    return false;
  }
  UNREACHABLE("Unreachable!");
}

void _tokenize_helper(char *str_in, DataArray *tokens, size_t pos) {
  size_t offset = skip_whitespaces(str_in);
  str_in += offset;
  pos += offset;
  size_t str_size = strlen(str_in);
  if (str_size == 0)
    return;
  switch (str_in[0]) {
  case '(': {
    Data cur = {
        .type = TT_LeftParen,
        .data = {0},
        .pos = pos,
    };
    da_append(tokens, cur);
  } break;
  case ')': {
    Data cur = {
        .type = TT_RightParen,
        .data = {0},
        .pos = pos,
    };
    da_append(tokens, cur);
  } break;
  case '+': {
    Data cur = {0};
    if (tokens->count == 0 || next_is_sign(da_last(tokens))) {
      cur = (Data){
          .type = TT_SignPos,
          .data = {0},
          .pos = pos,
      };
    } else {
      cur = (Data){
          .type = TT_Add,
          .data = {0},
          .pos = pos,
      };
    }
    da_append(tokens, cur);
  } break;
  case '-': {
    Data cur = {0};
    if (tokens->count == 0 || next_is_sign(da_last(tokens))) {
      cur = (Data){
          .type = TT_SignNeg,
          .data = {0},
          .pos = pos,
      };
    } else {
      cur = (Data){
          .type = TT_Sub,
          .data = {0},
          .pos = pos,
      };
    }
    da_append(tokens, cur);
  } break;
  case '/': {
    TokenType t_type = TT_Div;
    if (str_size > 1 && str_in[1] == '/') {
      t_type = TT_IntDiv;
      str_in++;
      pos++;
    }
    Data cur = {
        .type = t_type,
        .data = {0},
        .pos = pos,
    };
    da_append(tokens, cur);
  } break;
  case '*': {
    Data cur = {
        .type = TT_Mult,
        .data = {0},
        .pos = pos,
    };
    da_append(tokens, cur);
  } break;
  case '^': {
    Data cur = {
        .type = TT_Exp,
        .data = {0},
        .pos = pos,
    };
    da_append(tokens, cur);
  } break;
  case '%': {
    Data cur = {
        .type = TT_Mod,
        .data = {0},
        .pos = pos,
    };
    da_append(tokens, cur);
  } break;
  case '0': {
    Data cur = {0};
    bool is_hexbin = false;
    if (str_size > 2 && (str_in[1] == 'x' || str_in[1] == 'b')) {
      is_hexbin = true;
      size_t offset;
      if (str_in[1] == 'x')
        offset = read_hexbin(str_in, &cur, 16);
      else {
        str_in += 2;
        pos += 2;
        offset = read_hexbin(str_in, &cur, 2);
      }
      str_in += offset;
      pos += offset;
      cur.pos = pos;
      da_append(tokens, cur);
    }
    if (is_hexbin) {
      break;
    }
    __attribute__((fallthrough));
  }
  case '1' ... '9':
  case '.': {
    Data cur = {0};
    size_t offset = read_num(str_in, &cur);
    str_in += offset;
    pos += offset;
    cur.pos = pos;
    da_append(tokens, cur);
    break;
  }
  case 'a' ... 'z':
  case 'A' ... 'Z':
  case '_': {
    Data cur = {0};
    size_t offset = read_ident(str_in, &cur);
    str_in += offset;
    pos += offset;
    cur.pos = pos;
    da_append(tokens, cur);
  } break;
  case '=': {
    Data cur = {
        .type = TT_Assign,
        .data = {0},
        .pos = pos,
    };
    da_append(tokens, cur);
  } break;
  default: {
    Data cur = {
        .type = TT_Illegal,
        .data = {0},
        .pos = pos,
    };
    da_append(tokens, cur);
  }
  }
  if (strlen(str_in) > 1)
    _tokenize_helper(++str_in, tokens, pos + 1);
}

void tokenize(char *str_in, DataArray *tokens) {
  char *now = strdup(str_in);
  _tokenize_helper(now, tokens, 0);
  free(now);
}

void print_da(DataArray *da) {
  for (size_t i = 0; i < da->count; i++) {
    switch (da->items[i].type) {
    case TT_LeftParen: {
      printf("TT_LeftParen\n");
    } break;
    case TT_RightParen: {
      printf("TT_RightParen\n");
    } break;
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
    case TT_Mod: {
      printf("TT_Mod\n");
    } break;
    case TT_SignPos: {
      printf("TT_SignPos\n");
    } break;
    case TT_SignNeg: {
      printf("TT_SignNeg\n");
    } break;
    case TT_Exp: {
      printf("TT_Exp\n");
    } break;
    case TT_NumberInt: {
      long num = da->items[i].data.int_val;
      printf("TT_NumberInt(%ld)\n", num);
    } break;
    case TT_NumberFloat: {
      double num = da->items[i].data.float_val;
      printf("TT_NumberFloat(%f)\n", num);
    } break;
    case TT_Assign: {
      printf("TT_Assign\n");
    } break;
    case TT_Ident: {
      printf("TT_Ident(%s)\n", da->items[i].data.str_val);
    } break;
    case TT_Illegal: {
      printf("TT_Illegal\n");
    } break;
    default:
      UNREACHABLE("Unreachable!");
    }
  }
}
