#include "eval.h"
#include "lexer.h"
#include "parser.h"

#include "stb_ds.h"

long powll(long a, long b) {
  if (b < 0)
    return 0;
  if (b == 0)
    return 1;
  if (b == 1)
    return a;
  long ret = powll(a, b / 2);
  ret *= ret * powll(a, b % 2);
  return ret;
}

RetValue retvalue_from_rpntoken(RPNToken tkn, MapStrRV *map) {
  RetValue ret;
  if (tkn.tc != TC_Operand) {
    goto fail;
  }
  switch (tkn.token.type) {
  case TT_NumberFloat: {
    ret.ret_type = RT_Float;
    ret.opt_num = (OptionNumber){.dv = tkn.token.data};
    ret.pos = tkn.token.pos;
  } break;
  case TT_NumberInt: {
    ret.ret_type = RT_Int;
    ret.opt_num = (OptionNumber){.dv = tkn.token.data};
    ret.pos = tkn.token.pos;
  } break;
  case TT_Ident: {
    ret = shget(map, tkn.token.data.str_val);
  } break;
  default:
    UNREACHABLE("Unreachable!");
  }
  return ret;
fail:
  ret.opt_num = (OptionNumber){.et = ET_InvalidSyntax};
  ret.ret_type = RT_Error;
  ret.pos = tkn.token.pos;
  return ret;
}

OptionNumber calc_2_args(RPNToken op, RPNToken fs, RPNToken sc,
                         RetType *ret_type, MapStrRV *map) {
  bool correct_args =
      op.tc == TC_Operator && fs.tc == TC_Operand && sc.tc == TC_Operand;
  if (!correct_args)
    goto fail;

  RetValue fsrv = retvalue_from_rpntoken(fs, map);
  RetValue scrv = retvalue_from_rpntoken(sc, map);

  RetType rt = RT_Int;
  if (fsrv.ret_type == RT_Float || scrv.ret_type == RT_Float) {
    rt = RT_Float;
  }
  switch (op.token.type) {
  case TT_Add: {
    if (rt == RT_Int) {
      long res = fsrv.opt_num.dv.int_val + scrv.opt_num.dv.int_val;
      DataValue res_num = {
          .int_val = res,
      };
      *ret_type = RT_Int;
      return (OptionNumber){.dv = res_num};
    } else {
      double res;
      if (fsrv.ret_type == RT_Float) {
        if (scrv.ret_type == RT_Float) {
          res = fsrv.opt_num.dv.float_val + scrv.opt_num.dv.float_val;
        } else {
          res = fsrv.opt_num.dv.float_val + scrv.opt_num.dv.int_val;
        }
      } else {
        res = fsrv.opt_num.dv.int_val + scrv.opt_num.dv.float_val;
      }
      DataValue res_num = {
          .float_val = res,
      };
      *ret_type = RT_Float;
      return (OptionNumber){.dv = res_num};
    }
  } break;
  case TT_Sub: {
    if (rt == RT_Int) {
      long res = fsrv.opt_num.dv.int_val - scrv.opt_num.dv.int_val;
      DataValue res_num = {
          .int_val = res,
      };
      *ret_type = RT_Int;
      return (OptionNumber){.dv = res_num};
    } else {
      double res;
      if (fsrv.ret_type == RT_Float) {
        if (scrv.ret_type == RT_Float) {
          res = fsrv.opt_num.dv.float_val - scrv.opt_num.dv.float_val;
        } else {
          res = fsrv.opt_num.dv.float_val - scrv.opt_num.dv.int_val;
        }
      } else {
        res = fsrv.opt_num.dv.int_val - scrv.opt_num.dv.float_val;
      }
      DataValue res_num = {
          .float_val = res,
      };
      *ret_type = RT_Float;
      return (OptionNumber){.dv = res_num};
    }
  } break;
  case TT_Div: {
    // rt = RT_Float;
    double res;
    if (fsrv.ret_type == RT_Float) {
      if (scrv.ret_type == RT_Float) {
        if (scrv.opt_num.dv.float_val == 0.0) {
          goto fail_zero;
        }
        res = fsrv.opt_num.dv.float_val / scrv.opt_num.dv.float_val;
      } else {
        if (scrv.opt_num.dv.int_val == 0) {
          goto fail_zero;
        }
        res = fsrv.opt_num.dv.float_val / (double)scrv.opt_num.dv.int_val;
      }
    } else {
      if (scrv.ret_type == RT_Float) {
        if (scrv.opt_num.dv.float_val == 0.0) {
          goto fail_zero;
        }
        res = (double)fsrv.opt_num.dv.int_val / scrv.opt_num.dv.float_val;
      } else {
        if (scrv.opt_num.dv.int_val == 0.0) {
          goto fail_zero;
        }
        res = (double)fsrv.opt_num.dv.int_val / (double)scrv.opt_num.dv.int_val;
      }
    }
    DataValue res_num = {
        .float_val = res,
    };
    *ret_type = RT_Float;
    return (OptionNumber){.dv = res_num};
  } break;
  case TT_IntDiv: {
    // rt = RT_Int;
    long res;
    if (fsrv.ret_type == RT_Float) {
      if (scrv.ret_type == RT_Float) {
        if (scrv.opt_num.dv.float_val == 0.0) {
          goto fail_zero;
        }
        res = (long)(fsrv.opt_num.dv.float_val / scrv.opt_num.dv.float_val);
      } else {
        if (scrv.opt_num.dv.int_val == 0) {
          goto fail_zero;
        }
        res = (long)(fsrv.opt_num.dv.float_val / scrv.opt_num.dv.int_val);
      }
    } else {
      if (scrv.ret_type == RT_Float) {
        if (scrv.opt_num.dv.float_val == 0.0) {
          goto fail_zero;
        }
        res = (long)(fsrv.opt_num.dv.int_val / scrv.opt_num.dv.float_val);
      } else {
        if (scrv.opt_num.dv.int_val == 0.0) {
          goto fail_zero;
        }
        res = fsrv.opt_num.dv.int_val / scrv.opt_num.dv.int_val;
      }
    }
    DataValue res_num = {
        .int_val = res,
    };
    *ret_type = RT_Int;
    return (OptionNumber){.dv = res_num};
  } break;
  case TT_Mult: {
    if (rt == RT_Int) {
      long res = fsrv.opt_num.dv.int_val * scrv.opt_num.dv.int_val;
      DataValue res_num = {
          .int_val = res,
      };
      *ret_type = RT_Int;
      return (OptionNumber){.dv = res_num};
    } else {
      double res;
      if (fsrv.ret_type == RT_Float) {
        if (scrv.ret_type == RT_Float) {
          res = fsrv.opt_num.dv.float_val * scrv.opt_num.dv.float_val;
        } else {
          res = fsrv.opt_num.dv.float_val * scrv.opt_num.dv.int_val;
        }
      } else {
        res = fsrv.opt_num.dv.int_val * scrv.opt_num.dv.float_val;
      }
      DataValue res_num = {
          .float_val = res,
      };
      *ret_type = RT_Float;
      return (OptionNumber){.dv = res_num};
    }
  } break;
  case TT_Mod: {
    if (rt == RT_Int) {
      if (scrv.opt_num.dv.int_val == 0)
        goto fail_zero;
      long res = fsrv.opt_num.dv.int_val % scrv.opt_num.dv.int_val;
      DataValue res_num = {
          .int_val = res,
      };
      *ret_type = RT_Int;
      return (OptionNumber){.dv = res_num};
    } else {
      double res;
      if (fsrv.ret_type == RT_Float) {
        if (scrv.ret_type == RT_Float) {
          if (scrv.opt_num.dv.float_val == 0)
            goto fail_zero;
          res = remainder(fsrv.opt_num.dv.float_val, scrv.opt_num.dv.float_val);
        } else {
          if (scrv.opt_num.dv.int_val == 0)
            goto fail_zero;
          res = remainder(fsrv.opt_num.dv.float_val,
                          (double)scrv.opt_num.dv.int_val);
        }
      } else {
        if (scrv.opt_num.dv.float_val == 0)
          goto fail_zero;
        res = remainder((double)fsrv.opt_num.dv.int_val,
                        scrv.opt_num.dv.float_val);
      }
      while (res < 0) {
        res += scrv.opt_num.dv.float_val;
      }
      DataValue res_num = {
          .float_val = res,
      };
      *ret_type = RT_Float;
      return (OptionNumber){.dv = res_num};
    }
  } break;
  case TT_Exp: {
    if (rt == RT_Int) {
      if (scrv.opt_num.dv.int_val >= 0) {
        long res = powll(fsrv.opt_num.dv.int_val, scrv.opt_num.dv.int_val);
        DataValue res_num = {
            .int_val = res,
        };
        *ret_type = RT_Int;
        return (OptionNumber){.dv = res_num};
      } else {
        double res = powf(fsrv.opt_num.dv.int_val, scrv.opt_num.dv.int_val);
        DataValue res_num = {
            .float_val = res,
        };
        *ret_type = RT_Float;
        return (OptionNumber){.dv = res_num};
      }
    } else {
      double res;
      if (fsrv.ret_type == RT_Float) {
        if (scrv.ret_type == RT_Float) {
          res = powf(fsrv.opt_num.dv.float_val, scrv.opt_num.dv.float_val);
        } else {
          res = powf(fsrv.opt_num.dv.float_val, scrv.opt_num.dv.int_val);
        }
      } else {
        res = powf(fsrv.opt_num.dv.int_val, scrv.opt_num.dv.float_val);
      }
      DataValue res_num = {
          .float_val = res,
      };
      *ret_type = RT_Float;
      return (OptionNumber){.dv = res_num};
    }
  } break;
  default:
    UNREACHABLE("Unreachable!");
  }
fail:
  *ret_type = RT_Error;
  return (OptionNumber){.et = ET_InvalidSyntax};
fail_zero:
  *ret_type = RT_Error;
  return (OptionNumber){.et = ET_DivisionByZero};
}

OptionNumber calc_1_arg(RPNToken op, RPNToken token, RetType *ret_type,
                        MapStrRV *map) {
  if (token.tc != TC_Operand || op.tc != TC_Operator) {
    goto fail;
  }
  long multiplier = 1;
  switch (op.token.type) {
  case TT_SignPos:
    break;
  case TT_SignNeg: {
    multiplier = -1;
  } break;
  default:
    goto fail;
  }

  RetValue tokenrv = retvalue_from_rpntoken(token, map);

  if (tokenrv.ret_type == RT_Float) {
    *ret_type = RT_Float;
    double ret_num = multiplier * tokenrv.opt_num.dv.float_val;
    OptionNumber ret = {
        .dv =
            (DataValue){
                .float_val = ret_num,
            },
    };
    return ret;
  } else if (tokenrv.ret_type == RT_Int) {
    *ret_type = RT_Int;
    long ret_num = multiplier * tokenrv.opt_num.dv.int_val;
    OptionNumber ret = {
        .dv =
            (DataValue){
                .int_val = ret_num,
            },
    };
    return ret;
  }
fail:
  *ret_type = RT_Error;
  return (OptionNumber){.et = ET_InvalidSyntax};
}

RetValue eval(const RPNArray *rpn, MapStrRV **map) {
  RPNArray num_stack = {0};
  for (size_t i = 0; i < rpn->count; i++) {
    switch (rpn->items[i].tc) {
    case TC_Operator: {
      RetType rt = RT_Error;
      OptionNumber temp;
      if (rpn->items[i].token.type == TT_Assign) {
        // TT_Assign can only be on the bottom of the stack;
        if (i != rpn->count - 1 || num_stack.count < 2) {
          OptionNumber ret = {.et = ET_InvalidSyntax};
          return (RetValue){.ret_type = RT_Error, .opt_num = ret};
        }
        RPNToken left = num_stack.items[num_stack.count - 2];
        RPNToken right = num_stack.items[num_stack.count - 1];
        num_stack.count -= 2;
        if (left.token.type != TT_Ident) {
          OptionNumber ret = {.et = ET_InvalidSyntax};
          return (RetValue){
              .ret_type = RT_Error, .opt_num = ret, .pos = left.token.pos};
        }
        if (right.token.type == TT_Ident) {
          RetValue rv = shget(*map, right.token.data.str_val);
          rt = rv.ret_type;
          temp = rv.opt_num;
        } else if (right.token.type == TT_NumberInt) {
          rt = RT_Int;
          temp = (OptionNumber){
              .dv = right.token.data,
          };
        } else if (right.token.type == TT_NumberFloat) {
          rt = RT_Float;
          temp = (OptionNumber){
              .dv = right.token.data,
          };
        } else {
          OptionNumber ret = {.et = ET_InvalidSyntax};
          return (RetValue){
              .ret_type = RT_Error, .opt_num = ret, .pos = right.token.pos};
        }
        RetValue to_assign = {
            .ret_type = rt, .opt_num = temp, .pos = left.token.pos};
        shput(*map, left.token.data.str_val, to_assign);
      } else if (rpn->items[i].token.type == TT_SignNeg ||
                 rpn->items[i].token.type == TT_SignPos) {
        if (num_stack.count < 1) {
          OptionNumber ret = {.et = ET_InvalidSyntax};
          return (RetValue){.ret_type = RT_Error,
                            .opt_num = ret,
                            .pos = rpn->items[i].token.pos + 1};
        }
        RPNToken num_tok = num_stack.items[num_stack.count - 1];
        num_stack.count--;
        temp = calc_1_arg(rpn->items[i], num_tok, &rt, *map);
      } else {
        if (num_stack.count < 2) {
          OptionNumber ret = {.et = ET_InvalidSyntax};
          return (RetValue){.ret_type = RT_Error,
                            .opt_num = ret,
                            .pos = rpn->items[i].token.pos};
        }
        RPNToken left = num_stack.items[num_stack.count - 2];
        RPNToken right = num_stack.items[num_stack.count - 1];
        num_stack.count -= 2;
        temp = calc_2_args(rpn->items[i], left, right, &rt, *map);
      }
      Data temp_data;
      if (rt == RT_Int) {
        temp_data = (Data){.type = TT_NumberInt,
                           .data = temp.dv,
                           .pos = rpn->items[i].token.pos};
      } else if (rt == RT_Float) {
        temp_data = (Data){.type = TT_NumberFloat,
                           .data = temp.dv,
                           .pos = rpn->items[i].token.pos};
      } else {
        return (RetValue){.ret_type = RT_Error,
                          .opt_num = temp,
                          .pos = rpn->items[i].token.pos};
      }
      RPNToken ret = {
          .token = temp_data,
          .tc = TC_Operand,
      };
      da_append(&num_stack, ret);
    } break;
    case TC_Operand: {
      da_append(&num_stack, rpn->items[i]);
    } break;
    case TC_Parens:
    case TC_Illegal:
    default:
      UNREACHABLE("Unreachable!");
    }
  }
  if (num_stack.count == 1) {
    if (num_stack.items[0].tc != TC_Operand)
      UNREACHABLE("Unreachable!");
    return retvalue_from_rpntoken(num_stack.items[0], *map);
  } else {
    OptionNumber ret = {.et = ET_InvalidSyntax};
    return (RetValue){.ret_type = RT_Error,
                      .opt_num = ret,
                      .pos = num_stack.items[0].token.pos};
  }
  UNREACHABLE("Unreachable!");
  OptionNumber ret = {.et = ET_InvalidSyntax};
  return (RetValue){.ret_type = RT_Error,
                    .opt_num = ret,
                    .pos = num_stack.items[0].token.pos};
}

void print_rv(RetValue rv) {
  if (rv.ret_type == RT_Error) {
    char *marker = "^";
    int offset = 7;
    switch (rv.opt_num.et) {
    case ET_InvalidSyntax: {
      printf("%*s\n[\033[1;31mERROR\033[0m] Invalid Syntax\n",
             offset + (int)rv.pos, marker);
    } break;
    case ET_DivisionByZero: {
      printf("%*s\n[\033[1;31mERROR\033[0m] Division/Modulo by zero\n",
             offset + (int)rv.pos, marker);
    } break;
    default:
      UNREACHABLE("Unknown Error");
    }
  } else if (rv.ret_type == RT_Float) {
    printf("[\033[1;32mRESULT\033[0m]: %f\n", rv.opt_num.dv.float_val);
  } else if (rv.ret_type == RT_Int) {
    printf("[\033[1;32mRESULT\033[0m]: %ld\n", rv.opt_num.dv.int_val);
  } else {
    UNREACHABLE("Invalid RetType");
  }
}
