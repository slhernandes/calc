#include "parser.h"

void print_ra(const RPNArray *ra) {
  for (size_t i = 0; i < ra->count; i++) {
    switch (ra->items[i].tc) {
    case TC_Number: {
      printf("Number: ");
    } break;
    case TC_Operator: {
      printf("Operator: ");
    } break;
    case TC_Parens: {
      printf("Parens: ");
    } break;
    case TC_Illegal: {
      printf("Illegal: ");
    } break;
    default:
      assert(0 && "Unreachable!");
    }
    switch (ra->items[i].token.type) {
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
    case TT_Exp: {
      printf("TT_Exp\n");
    } break;
    case TT_NumberInt: {
      int num = ra->items[i].token.data.int_val;
      printf("TT_NumberInt(%d)\n", num);
    } break;
    case TT_NumberFloat: {
      float num = ra->items[i].token.data.float_val;
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

RPNArray *compress_add_sub(DataArray *data) {
  Data filler = {
      .type = TT_Illegal,
      .data = {0},
  };
  da_append(data, filler);
  RPNArray *ret = malloc(sizeof(DataArray));
  size_t orig_len = data->count;
  size_t count_neg = 0;
  bool start = false;
  for (size_t i = 0; i < orig_len; i++) {
    if (data->items[i].type != TT_Add && data->items[i].type != TT_Sub) {
      if (start) {
        Data temp_data = {0};
        if (count_neg % 2) {
          temp_data = (Data){
              .type = TT_Sub,
              .data = {0},
          };
        } else {
          temp_data = (Data){
              .type = TT_Add,
              .data = {0},
          };
        }

        RPNToken temp = {
            .token = temp_data,
            .tc = TC_Operator,
        };

        da_append(ret, temp);
        count_neg = 0;
        start = false;
      }
      if (i != orig_len - 1) {
        RPNToken temp = {
            .token = data->items[i],
        };
        switch (data->items[i].type) {
        case TT_LeftParen:
        case TT_RightParen: {
          temp.tc = TC_Parens;
        } break;
        case TT_Add:
        case TT_Sub:
        case TT_Div:
        case TT_IntDiv:
        case TT_Mult:
        case TT_Exp: {
          temp.tc = TC_Operator;
        } break;
        case TT_NumberInt:
        case TT_NumberFloat: {
          temp.tc = TC_Number;
        } break;
        case TT_Illegal: {
          temp.tc = TC_Illegal;
        } break;
        default:
          assert(0 && "Unreachable!");
        }
        da_append(ret, temp);
      }
    } else {
      start = true;
      if (data->items[i].type == TT_Sub) {
        count_neg++;
      }
    }
  }
  // Remove the last illegal token
  data->count--;
  return ret;
}

int precedence(RPNToken token) {
  switch (token.token.type) {
  case TT_Add:
  case TT_Sub: {
    return 0;
  } break;
  case TT_Div:
  case TT_IntDiv:
  case TT_Mult: {
    return 1;
  } break;
  case TT_Exp: {
    return 2;
  } break;
  default:
    return -1;
  }
  assert(0 && "Unreachable!");
  return -2;
}

bool pop_op(RPNToken top, RPNToken in) {
  bool lt = precedence(in) < precedence(top);
  bool eq = (precedence(in) == precedence(top)) && (in.token.type != TT_Exp);
  return lt || eq;
}

RPNArray *infix_to_rpn(const RPNArray *ra) {
  RPNArray *ret = malloc(sizeof(RPNArray));
  RPNArray op_q = {0};
  size_t ra_size = ra->count;
  for (size_t i = 0; i < ra_size; i++) {
    switch (ra->items[i].tc) {
    case TC_Number: {
      da_append(ret, ra->items[i]);
    } break;
    case TC_Operator: {
      size_t st_size = op_q.count;
      if (st_size == 0 || !pop_op(op_q.items[st_size - 1], ra->items[i])) {
        da_append(&op_q, ra->items[i]);
      } else {
        size_t cnt = st_size;
        for (; cnt > 0; cnt--) {
          RPNToken top = op_q.items[cnt - 1];
          if (pop_op(top, ra->items[i]))
            da_append(ret, top);
          else
            break;
        }
        op_q.count = cnt;
        da_append(&op_q, ra->items[i]);
      }
    } break;
    case TC_Parens: {
      if (ra->items[i].token.type == TT_LeftParen) {
        da_append(&op_q, ra->items[i]);
      } else if (op_q.count > 0) {
        size_t idx = op_q.count - 1;
        while (op_q.items[idx].token.type != TT_LeftParen) {
          RPNToken top = op_q.items[idx];
          da_append(ret, top);
          if (idx > 0)
            idx--;
          else
            break;
        }
        op_q.count = idx;
      } else {
        assert(0 && "Unreachable!");
      }
    } break;
    case TC_Illegal: {
    } break;
    default:
      assert(0 && "Unreachable!");
    }
  }
  size_t st_size = op_q.count;
  printf("size: %ld\n", st_size);
  for (size_t i = st_size; i > 0; i--) {
    da_append(ret, op_q.items[i - 1]);
  }
  op_q.count = 0;
  return ret;
}

void parser_ra_free(RPNArray *ra) { free(ra); }
