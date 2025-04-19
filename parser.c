#include "parser.h"
#include "lexer.h"

void print_ra(const RPNArray *ra) {
  for (size_t i = 0; i < ra->count; i++) {
    switch (ra->items[i].tc) {
    case TC_Operand: {
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
      UNREACHABLE("Unreachable!");
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
      long num = ra->items[i].token.data.int_val;
      printf("TT_NumberInt(%ld)\n", num);
    } break;
    case TT_NumberFloat: {
      double num = ra->items[i].token.data.float_val;
      printf("TT_NumberFloat(%f)\n", num);
    } break;
    case TT_Assign: {
      printf("TT_Assign\n");
    } break;
    case TT_Ident: {
      printf("TT_Ident(%s)\n", ra->items[i].token.data.str_val);
    } break;
    case TT_Illegal: {
      printf("TT_Illegal\n");
    } break;
    default:
      UNREACHABLE("Unreachable!");
    }
  }
}

RPNArray compress_add_sub(DataArray *data) {
  Data filler = {
      .type = TT_Illegal,
      .data = {0},
  };
  da_append(data, filler);
  RPNArray ret = {0};
  size_t orig_len = data->count;
  size_t count_neg = 0;
  bool start = false;
  for (size_t i = 0; i < orig_len; i++) {
    if (data->items[i].type != TT_SignPos &&
        data->items[i].type != TT_SignNeg) {
      if (start) {
        Data temp_data = {0};
        if (count_neg % 2) {
          temp_data = (Data){
              .type = TT_SignNeg,
              .data = {0},
              .pos = data->items[i].pos,
          };
        } else {
          temp_data = (Data){
              .type = TT_SignPos,
              .data = {0},
              .pos = data->items[i].pos,
          };
        }

        RPNToken temp = {
            .token = temp_data,
            .tc = TC_Operator,
        };

        da_append(&ret, temp);
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
        case TT_Mod:
        case TT_SignPos:
        case TT_SignNeg:
        case TT_Assign:
        case TT_Exp: {
          temp.tc = TC_Operator;
        } break;
        case TT_Ident:
        case TT_NumberInt:
        case TT_NumberFloat: {
          temp.tc = TC_Operand;
        } break;
        case TT_Illegal: {
          temp.tc = TC_Illegal;
        } break;
        default:
          UNREACHABLE("Unreachable!");
        }
        da_append(&ret, temp);
      }
    } else {
      start = true;
      if (data->items[i].type == TT_SignNeg) {
        count_neg++;
      }
    }
  }
  // Remove the last illegal token
  data->count--;
  return ret;
}

long precedence(RPNToken token) {
  switch (token.token.type) {
  case TT_Assign: {
    return 0;
  } break;
  case TT_Add:
  case TT_Sub: {
    return 1;
  } break;
  case TT_Div:
  case TT_IntDiv:
  case TT_Mult:
  case TT_Mod: {
    return 2;
  } break;
  case TT_SignPos:
  case TT_SignNeg:
  case TT_Exp: {
    return 3;
  } break;
  default:
    return -1;
  }
  UNREACHABLE("Unreachable!");
  return -2;
}

int assoc(TokenType tt) {
  switch (tt) {
  case TT_SignNeg:
  case TT_SignPos:
  case TT_Exp: {
    return 1;
  } break;
  default:
    return 0;
  }
  UNREACHABLE("Unreachable!");
}

bool pop_op(RPNToken top, RPNToken in) {
  bool lt = precedence(in) < precedence(top);
  bool eq = (precedence(in) == precedence(top)) && (assoc(in.token.type) != 1);
  return lt || eq;
}

RPNArray infix_to_rpn(const RPNArray *ra) {
  RPNArray ret = {0};
  RPNArray op_q = {0};
  size_t ra_size = ra->count;
  for (size_t i = 0; i < ra_size; i++) {
    switch (ra->items[i].tc) {
    case TC_Operand: {
      da_append(&ret, ra->items[i]);
    } break;
    case TC_Operator: {
      size_t st_size = op_q.count;
      if (st_size == 0 || !pop_op(da_last(&op_q), ra->items[i])) {
        da_append(&op_q, ra->items[i]);
      } else {
        size_t cnt = st_size;
        for (; cnt > 0; cnt--) {
          RPNToken top = op_q.items[cnt - 1];
          if (pop_op(top, ra->items[i]))
            da_append(&ret, top);
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
          da_append(&ret, top);
          if (idx > 0)
            idx--;
          else
            break;
        }
        op_q.count = idx;
      } else {
        UNREACHABLE("Unreachable!");
      }
    } break;
    case TC_Illegal: {
    } break;
    default:
      UNREACHABLE("Unreachable!");
    }
  }
  size_t st_size = op_q.count;
  for (size_t i = st_size; i > 0; i--) {
    da_append(&ret, op_q.items[i - 1]);
  }
  op_q.count = 0;
  return ret;
}
