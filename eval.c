#include "eval.h"
#include "lexer.h"
#include "parser.h"

#define STB_DS_IMPLEMENTATION
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
    ret.opt_num = (OptionNumber){.some = tkn.token.data};
  } break;
  case TT_NumberInt: {
    ret.ret_type = RT_Int;
    ret.opt_num = (OptionNumber){.some = tkn.token.data};
  } break;
  case TT_Ident: {
    ret = shget(map, tkn.token.data.str_val);
  } break;
  default:
    UNREACHABLE("Unreachable!");
  }
  return ret;
fail:
  ret.opt_num = (OptionNumber){.none = true};
  ret.ret_type = RT_Error;
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
      long res = fsrv.opt_num.some.int_val + scrv.opt_num.some.int_val;
      DataValue res_num = {
          .int_val = res,
      };
      *ret_type = RT_Int;
      return (OptionNumber){.some = res_num};
    } else {
      double res;
      if (fs.token.type == TT_NumberFloat) {
        if (sc.token.type == TT_NumberFloat) {
          res = fsrv.opt_num.some.float_val + scrv.opt_num.some.float_val;
        } else {
          res = fsrv.opt_num.some.float_val + scrv.opt_num.some.int_val;
        }
      } else {
        res = fsrv.opt_num.some.int_val + scrv.opt_num.some.float_val;
      }
      DataValue res_num = {
          .float_val = res,
      };
      *ret_type = RT_Float;
      return (OptionNumber){.some = res_num};
    }
  } break;
  case TT_Sub: {
    if (rt == RT_Int) {
      long res = fsrv.opt_num.some.int_val - scrv.opt_num.some.int_val;
      DataValue res_num = {
          .int_val = res,
      };
      *ret_type = RT_Int;
      return (OptionNumber){.some = res_num};
    } else {
      double res;
      if (fs.token.type == TT_NumberFloat) {
        if (sc.token.type == TT_NumberFloat) {
          res = fsrv.opt_num.some.float_val - scrv.opt_num.some.float_val;
        } else {
          res = fsrv.opt_num.some.float_val - scrv.opt_num.some.int_val;
        }
      } else {
        res = fsrv.opt_num.some.int_val - scrv.opt_num.some.float_val;
      }
      DataValue res_num = {
          .float_val = res,
      };
      *ret_type = RT_Float;
      return (OptionNumber){.some = res_num};
    }
  } break;
  case TT_Div: {
    // rt = RT_Float;
    double res;
    if (fs.token.type == TT_NumberFloat) {
      if (sc.token.type == TT_NumberFloat) {
        if (scrv.opt_num.some.float_val == 0.0) {
          goto fail;
        }
        res = fsrv.opt_num.some.float_val / scrv.opt_num.some.float_val;
      } else {
        if (scrv.opt_num.some.int_val == 0) {
          goto fail;
        }
        res = fsrv.opt_num.some.float_val / (double)scrv.opt_num.some.int_val;
      }
    } else {
      if (sc.token.type == TT_NumberFloat) {
        if (scrv.opt_num.some.float_val == 0.0) {
          goto fail;
        }
        res = (double)fsrv.opt_num.some.int_val / scrv.opt_num.some.float_val;
      } else {
        if (scrv.opt_num.some.int_val == 0.0) {
          goto fail;
        }
        res = (double)fsrv.opt_num.some.int_val /
              (double)scrv.opt_num.some.int_val;
      }
    }
    DataValue res_num = {
        .float_val = res,
    };
    *ret_type = RT_Float;
    return (OptionNumber){.some = res_num};
  } break;
  case TT_IntDiv: {
    // rt = RT_Int;
    long res;
    if (fs.token.type == TT_NumberFloat) {
      if (sc.token.type == TT_NumberFloat) {
        if (scrv.opt_num.some.float_val == 0.0) {
          goto fail;
        }
        res = (long)(fsrv.opt_num.some.float_val / scrv.opt_num.some.float_val);
      } else {
        if (scrv.opt_num.some.int_val == 0) {
          goto fail;
        }
        res = (long)(fsrv.opt_num.some.float_val / scrv.opt_num.some.int_val);
      }
    } else {
      if (sc.token.type == TT_NumberFloat) {
        if (scrv.opt_num.some.float_val == 0.0) {
          goto fail;
        }
        res = (long)(fsrv.opt_num.some.int_val / scrv.opt_num.some.float_val);
      } else {
        if (scrv.opt_num.some.int_val == 0.0) {
          goto fail;
        }
        res = fsrv.opt_num.some.int_val / scrv.opt_num.some.int_val;
      }
    }
    DataValue res_num = {
        .int_val = res,
    };
    *ret_type = RT_Int;
    return (OptionNumber){.some = res_num};
  } break;
  case TT_Mult: {
    if (rt == RT_Int) {
      long res = fsrv.opt_num.some.int_val * scrv.opt_num.some.int_val;
      DataValue res_num = {
          .int_val = res,
      };
      *ret_type = RT_Int;
      return (OptionNumber){.some = res_num};
    } else {
      double res;
      if (fs.token.type == TT_NumberFloat) {
        if (sc.token.type == TT_NumberFloat) {
          res = fsrv.opt_num.some.float_val * scrv.opt_num.some.float_val;
        } else {
          res = fsrv.opt_num.some.float_val * scrv.opt_num.some.int_val;
        }
      } else {
        res = fsrv.opt_num.some.int_val * scrv.opt_num.some.float_val;
      }
      DataValue res_num = {
          .float_val = res,
      };
      *ret_type = RT_Float;
      return (OptionNumber){.some = res_num};
    }
  } break;
  case TT_Mod: {
    if (rt == RT_Int) {
      long res = fsrv.opt_num.some.int_val % scrv.opt_num.some.int_val;
      DataValue res_num = {
          .int_val = res,
      };
      *ret_type = RT_Int;
      return (OptionNumber){.some = res_num};
    } else {
      double res;
      if (fs.token.type == TT_NumberFloat) {
        if (sc.token.type == TT_NumberFloat) {
          res = remainder(fsrv.opt_num.some.float_val,
                          scrv.opt_num.some.float_val);
        } else {
          res = remainder(fsrv.opt_num.some.float_val,
                          (double)scrv.opt_num.some.int_val);
        }
      } else {
        res = remainder((double)fsrv.opt_num.some.int_val,
                        scrv.opt_num.some.float_val);
      }
      while (res < 0) {
        res += scrv.opt_num.some.float_val;
      }
      DataValue res_num = {
          .float_val = res,
      };
      *ret_type = RT_Float;
      return (OptionNumber){.some = res_num};
    }
  } break;
  case TT_Exp: {
    if (rt == RT_Int) {
      if (scrv.opt_num.some.int_val >= 0) {
        long res = powll(fsrv.opt_num.some.int_val, scrv.opt_num.some.int_val);
        DataValue res_num = {
            .int_val = res,
        };
        *ret_type = RT_Int;
        return (OptionNumber){.some = res_num};
      } else {
        double res = powf(fsrv.opt_num.some.int_val, scrv.opt_num.some.int_val);
        DataValue res_num = {
            .float_val = res,
        };
        *ret_type = RT_Float;
        return (OptionNumber){.some = res_num};
      }
    } else {
      double res;
      if (fs.token.type == TT_NumberFloat) {
        if (sc.token.type == TT_NumberFloat) {
          res = powf(fsrv.opt_num.some.float_val, scrv.opt_num.some.float_val);
        } else {
          res = powf(fsrv.opt_num.some.float_val, scrv.opt_num.some.int_val);
        }
      } else {
        res = powf(fsrv.opt_num.some.int_val, scrv.opt_num.some.float_val);
      }
      DataValue res_num = {
          .float_val = res,
      };
      *ret_type = RT_Float;
      return (OptionNumber){.some = res_num};
    }
  } break;
  default:
    UNREACHABLE("Unreachable!");
  }
fail:
  *ret_type = RT_Error;
  return (OptionNumber){.none = true};
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
  if (token.token.type == TT_NumberFloat) {
    *ret_type = RT_Float;
    double ret_num = multiplier * token.token.data.float_val;
    OptionNumber ret = {
        .some =
            (DataValue){
                .float_val = ret_num,
            },
    };
    return ret;
  } else if (token.token.type == TT_NumberInt) {
    *ret_type = RT_Int;
    long ret_num = multiplier * token.token.data.int_val;
    OptionNumber ret = {
        .some =
            (DataValue){
                .int_val = ret_num,
            },
    };
    return ret;
  } else if (token.token.type == TT_Ident) {
    RetValue rv = shget(map, token.token.data.str_val);
    *ret_type = rv.ret_type;
    if (*ret_type == RT_Int) {
      rv.opt_num.some.int_val *= multiplier;
    } else if (*ret_type == RT_Float) {
      rv.opt_num.some.float_val *= multiplier;
    } else {
      goto fail;
    }
    return rv.opt_num;
  }
fail:
  *ret_type = RT_Error;
  return (OptionNumber){.none = true};
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
          OptionNumber ret = {.none = true};
          return (RetValue){.ret_type = RT_Error, .opt_num = ret};
        }
        RPNToken left = num_stack.items[num_stack.count - 2];
        RPNToken right = num_stack.items[num_stack.count - 1];
        num_stack.count -= 2;
        if (left.token.type != TT_Ident) {
          OptionNumber ret = {.none = true};
          return (RetValue){.ret_type = RT_Error, .opt_num = ret};
        }
        if (right.token.type == TT_Ident) {
          RetValue rv = shget(*map, right.token.data.str_val);
          rt = rv.ret_type;
          temp = rv.opt_num;
        } else if (right.token.type == TT_NumberInt) {
          rt = RT_Int;
          temp = (OptionNumber){
              .some = right.token.data,
          };
        } else if (right.token.type == TT_NumberFloat) {
          rt = RT_Float;
          temp = (OptionNumber){
              .some = right.token.data,
          };
        } else {
          OptionNumber ret = {.none = true};
          return (RetValue){.ret_type = RT_Error, .opt_num = ret};
        }
        RetValue to_assign = {.ret_type = rt, .opt_num = temp};
        shput(*map, left.token.data.str_val, to_assign);
      } else if (rpn->items[i].token.type == TT_SignNeg ||
                 rpn->items[i].token.type == TT_SignPos) {
        if (num_stack.count < 1) {
          OptionNumber ret = {.none = true};
          return (RetValue){.ret_type = RT_Error, .opt_num = ret};
        }
        RPNToken num_tok = num_stack.items[num_stack.count - 1];
        num_stack.count--;
        temp = calc_1_arg(rpn->items[i], num_tok, &rt, *map);
      } else {
        if (num_stack.count < 2) {
          OptionNumber ret = {.none = true};
          return (RetValue){.ret_type = RT_Error, .opt_num = ret};
        }
        RPNToken left = num_stack.items[num_stack.count - 2];
        RPNToken right = num_stack.items[num_stack.count - 1];
        num_stack.count -= 2;
        temp = calc_2_args(rpn->items[i], left, right, &rt, *map);
      }
      Data temp_data;
      if (rt == RT_Int) {
        temp_data = (Data){.type = TT_NumberInt, .data = temp.some};
      } else if (rt == RT_Float) {
        temp_data = (Data){.type = TT_NumberFloat, .data = temp.some};
      } else {
        OptionNumber ret = {.none = true};
        return (RetValue){.ret_type = RT_Error, .opt_num = ret};
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
    OptionNumber ret = {.none = true};
    return (RetValue){.ret_type = RT_Error, .opt_num = ret};
  }
  UNREACHABLE("Unreachable!");
  OptionNumber ret = {.none = true};
  return (RetValue){.ret_type = RT_Error, .opt_num = ret};
}

void print_rv(RetValue rv) {
  if (rv.ret_type == RT_Error) {
    printf("[\033[1;31mERROR\033[0m] Invalid Syntax\n");
  } else if (rv.ret_type == RT_Float) {
    printf("[\033[1;32mRESULT\033[0m]: %f\n", rv.opt_num.some.float_val);
  } else if (rv.ret_type == RT_Int) {
    printf("[\033[1;32mRESULT\033[0m]: %ld\n", rv.opt_num.some.int_val);
  } else {
    UNREACHABLE("Invalid RetType");
  }
}
