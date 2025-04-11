#include "eval.h"
#include "lexer.h"
#include "parser.h"

int powii(int a, int b) {
  if (b < 0)
    return 0;
  if (b == 0)
    return 1;
  if (b == 1)
    return a;
  int ret = powii(a, b / 2);
  ret *= ret * powii(a, b % 2);
  return ret;
}

OptionNumber calc_2_args(RPNToken op, RPNToken fs, RPNToken sc,
                         RetType *ret_type) {
  bool correct_args =
      op.tc == TC_Operator && fs.tc == TC_Number && sc.tc == TC_Number;
  if (!correct_args)
    return (OptionNumber){.none = true};
  RetType rt = RT_Int;
  if (fs.token.type == TT_NumberFloat || sc.token.type == TT_NumberFloat) {
    rt = RT_Float;
  }
  switch (op.token.type) {
  case TT_Add: {
    if (rt == RT_Int) {
      int res = fs.token.data.int_val + sc.token.data.int_val;
      Number res_num = {
          .int_val = res,
      };
      *ret_type = RT_Int;
      return (OptionNumber){.some = res_num};
    } else {
      float res;
      if (fs.token.type == TT_NumberFloat) {
        if (sc.token.type == TT_NumberFloat) {
          res = fs.token.data.float_val + sc.token.data.float_val;
        } else {
          res = fs.token.data.float_val + sc.token.data.int_val;
        }
      } else {
        res = fs.token.data.int_val + sc.token.data.float_val;
      }
      Number res_num = {
          .float_val = res,
      };
      *ret_type = RT_Float;
      return (OptionNumber){.some = res_num};
    }
  } break;
  case TT_Sub: {
    if (rt == RT_Int) {
      int res = fs.token.data.int_val - sc.token.data.int_val;
      Number res_num = {
          .int_val = res,
      };
      *ret_type = RT_Int;
      return (OptionNumber){.some = res_num};
    } else {
      float res;
      if (fs.token.type == TT_NumberFloat) {
        if (sc.token.type == TT_NumberFloat) {
          res = fs.token.data.float_val - sc.token.data.float_val;
        } else {
          res = fs.token.data.float_val - sc.token.data.int_val;
        }
      } else {
        res = fs.token.data.int_val - sc.token.data.float_val;
      }
      Number res_num = {
          .float_val = res,
      };
      *ret_type = RT_Float;
      return (OptionNumber){.some = res_num};
    }
  } break;
  case TT_Div: {
    // rt = RT_Float;
    float res;
    if (fs.token.type == TT_NumberFloat) {
      if (sc.token.type == TT_NumberFloat) {
        if (sc.token.data.float_val == 0.0) {
          *ret_type = RT_Error;
          return (OptionNumber){.none = true};
        }
        res = fs.token.data.float_val / sc.token.data.float_val;
      } else {
        if (sc.token.data.int_val == 0) {
          *ret_type = RT_Error;
          return (OptionNumber){.none = true};
        }
        res = fs.token.data.float_val / (float)sc.token.data.int_val;
      }
    } else {
      if (sc.token.type == TT_NumberFloat) {
        if (sc.token.data.float_val == 0.0) {
          *ret_type = RT_Error;
          return (OptionNumber){.none = true};
        }
        res = (float)fs.token.data.int_val / sc.token.data.float_val;
      } else {
        if (sc.token.data.int_val == 0.0) {
          *ret_type = RT_Error;
          return (OptionNumber){.none = true};
        }
        res = (float)fs.token.data.int_val / (float)sc.token.data.int_val;
      }
    }
    Number res_num = {
        .float_val = res,
    };
    *ret_type = RT_Float;
    return (OptionNumber){.some = res_num};
  } break;
  case TT_IntDiv: {
    // rt = RT_Int;
    int res;
    if (fs.token.type == TT_NumberFloat) {
      if (sc.token.type == TT_NumberFloat) {
        if (sc.token.data.float_val == 0.0) {
          *ret_type = RT_Error;
          return (OptionNumber){.none = true};
        }
        res = (int)(fs.token.data.float_val / sc.token.data.float_val);
      } else {
        if (sc.token.data.int_val == 0) {
          *ret_type = RT_Error;
          return (OptionNumber){.none = true};
        }
        res = (int)(fs.token.data.float_val / sc.token.data.int_val);
      }
    } else {
      if (sc.token.type == TT_NumberFloat) {
        if (sc.token.data.float_val == 0.0) {
          *ret_type = RT_Error;
          return (OptionNumber){.none = true};
        }
        res = (int)(fs.token.data.int_val / sc.token.data.float_val);
      } else {
        if (sc.token.data.int_val == 0.0) {
          *ret_type = RT_Error;
          return (OptionNumber){.none = true};
        }
        res = fs.token.data.int_val / sc.token.data.int_val;
      }
    }
    Number res_num = {
        .int_val = res,
    };
    *ret_type = RT_Int;
    return (OptionNumber){.some = res_num};
  } break;
  case TT_Mult: {
    if (rt == RT_Int) {
      int res = fs.token.data.int_val * sc.token.data.int_val;
      Number res_num = {
          .int_val = res,
      };
      *ret_type = RT_Int;
      return (OptionNumber){.some = res_num};
    } else {
      float res;
      if (fs.token.type == TT_NumberFloat) {
        if (sc.token.type == TT_NumberFloat) {
          res = fs.token.data.float_val * sc.token.data.float_val;
        } else {
          res = fs.token.data.float_val * sc.token.data.int_val;
        }
      } else {
        res = fs.token.data.int_val * sc.token.data.float_val;
      }
      Number res_num = {
          .float_val = res,
      };
      *ret_type = RT_Float;
      return (OptionNumber){.some = res_num};
    }
  } break;
  case TT_Exp: {
    if (rt == RT_Int) {
      if (sc.token.data.int_val >= 0) {
        int res = powii(fs.token.data.int_val, sc.token.data.int_val);
        Number res_num = {
            .int_val = res,
        };
        *ret_type = RT_Int;
        return (OptionNumber){.some = res_num};
      } else {
        float res = powf(fs.token.data.int_val, sc.token.data.int_val);
        Number res_num = {
            .float_val = res,
        };
        *ret_type = RT_Float;
        return (OptionNumber){.some = res_num};
      }
    } else {
      float res;
      if (fs.token.type == TT_NumberFloat) {
        if (sc.token.type == TT_NumberFloat) {
          res = powf(fs.token.data.float_val, sc.token.data.float_val);
        } else {
          res = powf(fs.token.data.float_val, sc.token.data.int_val);
        }
      } else {
        res = powf(fs.token.data.int_val, sc.token.data.float_val);
      }
      Number res_num = {
          .float_val = res,
      };
      *ret_type = RT_Float;
      return (OptionNumber){.some = res_num};
    }
  } break;
  default:
    assert(0 && "Unreachable!");
  }
}

OptionNumber calc_1_arg(RPNToken op, RPNToken token, RetType *ret_type) {
  if (token.tc != TC_Number || op.tc != TC_Operator) {
    *ret_type = RT_Error;
    return (OptionNumber){.none = true};
  }
  int multiplier = 1;
  switch (op.token.type) {
  case TT_SignPos:
    break;
  case TT_SignNeg: {
    multiplier = -1;
  } break;
  default:
    *ret_type = RT_Error;
    return (OptionNumber){.none = true};
  }
  if (token.token.type == TT_NumberFloat) {
    *ret_type = RT_Float;
    float ret_num = multiplier * token.token.data.float_val;
    OptionNumber ret = {
        .some =
            (Number){
                .float_val = ret_num,
            },
    };
    return ret;
  } else if (token.token.type == TT_NumberInt) {
    *ret_type = RT_Int;
    int ret_num = multiplier * token.token.data.int_val;
    OptionNumber ret = {
        .some =
            (Number){
                .int_val = ret_num,
            },
    };
    return ret;
  }
  *ret_type = RT_Error;
  return (OptionNumber){.none = true};
}

OptionNumber eval(const RPNArray *rpn, RetType *ret_type) {
  RPNArray num_stack = {0};
  for (size_t i = 0; i < rpn->count; i++) {
    switch (rpn->items[i].tc) {
    case TC_Operator: {
      RetType *rt = malloc(sizeof(RetType));
      OptionNumber temp;
      if (rpn->items[i].token.type == TT_SignNeg ||
          rpn->items[i].token.type == TT_SignPos) {
        if (num_stack.count < 1) {
          *ret_type = RT_Error;
          free(rt);
          return (OptionNumber){.none = true};
        }
        RPNToken num_tok = num_stack.items[num_stack.count - 1];
        num_stack.count--;
        temp = calc_1_arg(rpn->items[i], num_tok, rt);
      } else {
        if (num_stack.count < 2) {
          *ret_type = RT_Error;
          free(rt);
          return (OptionNumber){.none = true};
        }
        RPNToken left = num_stack.items[num_stack.count - 2];
        RPNToken right = num_stack.items[num_stack.count - 1];
        num_stack.count -= 2;
        temp = calc_2_args(rpn->items[i], left, right, rt);
      }
      Data temp_data;
      if (*rt == RT_Int) {
        temp_data = (Data){.type = TT_NumberInt, .data = temp.some};
      } else if (*rt == RT_Float) {
        temp_data = (Data){.type = TT_NumberFloat, .data = temp.some};
      } else {
        free(rt);
        return (OptionNumber){.none = true};
      }
      RPNToken ret = {
          .token = temp_data,
          .tc = TC_Number,
      };
      free(rt);
      da_append(&num_stack, ret);
    } break;
    case TC_Number: {
      da_append(&num_stack, rpn->items[i]);
    } break;
    case TC_Parens:
    case TC_Illegal:
    default:
      assert(0 && "Unreachable!");
    }
  }
  if (num_stack.count == 1) {
    if (num_stack.items[0].tc != TC_Number)
      assert(0 && "Unreachable!");
    Number temp = num_stack.items[0].token.data;
    if (num_stack.items[0].token.type == TT_NumberInt) {
      *ret_type = RT_Int;
    } else {
      *ret_type = RT_Float;
    }
    return (OptionNumber){.some = temp};
  } else {
    *ret_type = RT_Error;
    return (OptionNumber){.none = true};
  }
  assert(0 && "Unreachable!");
  return (OptionNumber){.none = true};
}

void print_on(OptionNumber on, RetType ret_type) {
  if (ret_type == RT_Error) {
    printf("[\033[1;31mERROR\033[0m] Invalid Syntax\n");
  } else if (ret_type == RT_Float) {
    printf("[\033[1;32mRESULT\033[0m]: %f\n", on.some.float_val);
  } else if (ret_type == RT_Int) {
    printf("[\033[1;32mRESULT\033[0m]: %d\n", on.some.int_val);
  } else {
    UNREACHABLE("Invalid RetType");
  }
}
