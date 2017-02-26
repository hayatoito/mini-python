#include "eval.h"

#include "base.h"
#include "tokenizer.h"

#include <iostream>

using namespace std;

class env {
 public:
  typedef const string key_t;
  typedef py_val_t val_t;

  typedef map<key_t, val_t> entries_map_t;
  typedef entries_map_t::iterator iterator;

  env(env_t _genv, ostream& _os);

  env_t genv;
  ostream& stdout;
  entries_map_t entries;
  stack_trace_t bt;

  void set(key_t key, val_t val);
  void set(key_t* key, val_t val);
  void set_global(key_t& key);
  void del(key_t& key);

  py_val_t eval_expr(expr_t e);

  py_val_t eval_var(expr_t e);
  py_val_t eval_literal_int(expr_t e);
  py_val_t eval_literal_float(expr_t e);
  py_val_t eval_literal_string(expr_t e);
  py_val_t eval_none(expr_t e);
  py_val_t eval_display_tuple(expr_t e);
  py_val_t eval_display_list(expr_t e);
  py_val_t eval_display_dict(expr_t e);
  py_val_t eval_paren(expr_t e);
  py_val_t eval_operator(expr_t e);
  py_val_t eval_attref(expr_t e);
  py_val_t eval_subscript(expr_t e);
  py_val_t eval_call(expr_t e);

  // Utility
  py_val_vec_t eval_expr_vec(expr_vec_t v) {
    py_val_vec_t results = mk_py_val_vec();
    const int n = v->size();
    for (int i = 0; i < n; ++i) {
      results->push_back(eval_expr((*v)[i]));
    }
    return results;
  }

  py_val_t lookup_operator_function(token_kind_t kind, fix_kind_t fix);
  py_val_t lookup(const char* symbol);
  py_val_t lookup(py_string_t symbol);

  py_val_t apply(py_val_t func, py_val_vec_t args, src_pos_t pos);

  void error(const string& msg) {
    cout << msg << endl;
    exit(1);
  }

  py_val_t eval_stmt_vec(stmt_vec_t s);
  py_val_t eval_stmt(stmt_t s);
  py_val_t eval_stmt_print(stmt_t s);
  py_val_t eval_stmt_assignment(stmt_t s);
  py_val_t eval_stmt_fundef(stmt_t s);
  py_val_t eval_stmt_return(stmt_t s);
  py_val_t eval_stmt_if(stmt_t s);
  py_val_t eval_stmt_while(stmt_t s);
  py_val_t eval_stmt_break(stmt_t s);
  py_val_t eval_stmt_continue(stmt_t s);

  friend ostream& operator<<(ostream& os, const env_t& e);
};

env::env(env_t _genv, ostream& _os)
    : genv(_genv),
      stdout(&_os == &std::cout && _genv != NULL ?
             _genv->stdout : _os) {
}

void env::set(key_t key, val_t val) {
  LOG(INFO) << "set " << key << " = " << val;
  iterator it = entries.find(key);
  if (it != entries.end()) {
    if (it->second == py_val_global) {
      if (genv != NULL) {
        genv->set(key, val);
      } else {
        error("global in global");
      }
    } else {
      entries[key] = val;
    }
  } else {
    entries[key] = val;
  }
}

void env::set(key_t* key, val_t val) {
  set(*key, val);
}

void env::set_global(key_t& key) {
  // if genv == NULL: error
  entries[key] = py_val_global;
}

void env::del(key_t& key) {
  entries.erase(key);
}

py_val_t env::eval_expr(expr_t e) {
  switch (e->kind) {
    case expr_kind_var:
      return eval_var(e);
    case expr_kind_literal_int:
      return eval_literal_int(e);
    case expr_kind_literal_float:
      return eval_literal_float(e);
    case expr_kind_literal_string:
      return eval_literal_string(e);
    case expr_kind_none:
      return eval_none(e);
    case expr_kind_display_tuple:
      return eval_display_tuple(e);
    case expr_kind_display_list:
      return eval_display_list(e);
    case expr_kind_display_dict:
      return eval_display_dict(e);
    case expr_kind_paren:
      return eval_paren(e);
    case expr_kind_operator:
      return eval_operator(e);
    case expr_kind_attref:
      return eval_attref(e);
    case expr_kind_subscript:
      return eval_subscript(e);
    case expr_kind_call:
      return eval_call(e);
    default:
      error("unkown expression");
      return 0;
  }
}

// Returns reference?
py_val_t env::eval_var(expr_t e) {
  string_t v = e->u.var;
  return lookup(v);
}

py_val_t env::lookup(const char* symbol) {
  // Todo: Intruduce true symbol.
  return lookup(new string(symbol));
}

py_val_t env::lookup(py_string_t symbol) {
  // logging::Logger<logging::INFO>("ab", 1) << "log macro";
  // logging::Logger<1>("ab", 1) << "log macro";

  LOG(INFO) << "env::lookup lookuping..." << *symbol;
  iterator it = entries.find(*symbol);
  if (it != entries.end()) {
    if (it->second != py_val_global) {
      LOG(INFO) << "evn::lookup found..." << it->second;
      return it->second;
    } else {
      LOG(INFO) << "found: but should be global";
    }

  }
  // Memo: Avoid assigning py_val_global_ to genv's entry when assigning.
  LOG(INFO) << "Try global env...";
  if (genv != NULL) {
    return genv->lookup(symbol);
  } else {
    error("Not found Var");
    return py_val_not_found;
  }
}

py_val_t env::eval_literal_int(expr_t e) {
  return mk_py_int(e->u.lit_i);
}

py_val_t env::eval_literal_float(expr_t e) {
  return mk_py_float(e->u.lit_f);
}

py_val_t env::eval_literal_string(expr_t e) {
  return mk_py_string(e->u.lit_s);
}

py_val_t env::eval_none(expr_t) {
  return mk_py_none();
}

py_val_t env::eval_display_tuple(expr_t e) {
  cout << "Not implemented" << endl;
  return mk_py_none();
}

py_val_t env::eval_display_list(expr_t) {
  cout << "Not implemented" << endl;
  return mk_py_none();
}

py_val_t env::eval_display_dict(expr_t) {
  cout << "Not implemented" << endl;
  return mk_py_none();
}

py_val_t env::eval_paren(expr_t) {
  cout << "Not implemented" << endl;
  return mk_py_none();
}

py_val_t env::eval_attref(expr_t) {
  cout << "Not implemented" << endl;
  return mk_py_none();
}

py_val_t env::eval_subscript(expr_t) {
  cout << "Not implemented" << endl;
  return mk_py_none();
}

py_val_t env::eval_operator(expr_t e) {
  token_kind_t token = e->u.op->kind;
  fix_kind_t fix = e->u.op->fix;
  expr_vec_t args = e->u.op->args;
  // Todo: Specail form - 'and', 'or'
  if (token == TOK_KW_AND) {
    py_val_t v = eval_expr((*args)[0]);
    if (py_is_false(v)) {
      return v;
    } else {
      return eval_expr((*args)[1]);
    }
  } else if (token == TOK_KW_OR) {
    py_val_t v = eval_expr((*args)[0]);
    if (!py_is_false(v)) {
      return v;
    } else {
      return eval_expr((*args)[1]);
    }
  } else {
    return apply(lookup_operator_function(token, fix),
                 eval_expr_vec(args),
                 e->pos);
  }
}

// py_val_t env::eval_attref(expr_t);
// py_val_t env::eval_subscript(expr_t);

py_val_t env::eval_call(expr_t e) {
  return apply(eval_expr(e->u.call->f),
               eval_expr_vec(e->u.call->args),
               e->pos);
}

py_val_t env::lookup_operator_function(token_kind_t kind, fix_kind_t fix) {
  // Todo: consider prefix
  switch (kind) {
    case TOK_KW_OR:  // or
    case TOK_KW_AND:  // and
    case TOK_KW_NOT:  // not
    case TOK_KW_IS:   // is
    case TOK_KW_IN: // in
      //TOK_EQ,    // =
      error("Not supported");
    case TOK_EQ_EQ:  // ==
      return lookup("_eq_eq");
      /*
        TOK_NEQ,   // !=
        TOK_GT,    // >
        TOK_GEQ,   // >=
      */
    case TOK_LT:    // <
      return lookup("_lt");
      /*
        TOK_LEQ,   // <=
      */
    case TOK_PLUS:  // +
      return lookup("_plus");
      /*
        TOK_MINUS, // -
        TOK_MUL,   // *
        TOK_DIV,   // /
        TOK_MOD,   // %
        TOK_TILDE, // ~
        TOK_LSHIFT,// <<
        TOK_RSHIFT,// >>
        TOK_XOR,   // ^
        TOK_AMP,   // &
        TOK_BAR,   // |
      */
    default:
      error("Unsupported operator");
      return 0;
  }
}

py_val_t env::apply(py_val_t f, py_val_vec_t args, src_pos_t pos) {
  LOG(INFO) << "env::apply " << f;

  if (py_is_ifun(f)) {
    py_ifun_t ifun = f->u.i;
    // Extend environment
    // This is old Local-Global environment. scope is not nested.
    env_t new_env = (genv == NULL ? mk_env(this) : mk_env(genv));
    const int n = ifun->ps->size();
    for (int i = 0; i < n; ++i) {
      new_env->set((*ifun->ps)[i], (*args)[i]);
    }
    return new_env->eval_stmt_vec(ifun->b);
  } else if (py_is_nfun(f)) {
    py_nfun_t nfun = f->u.n;
    // assert(args->size() == nfun->arity);
    // Todo stack trace?
    return (*nfun->f)(args, bt, pos);
  } else {
    error("Cannot apply");
    return 0;
  }
}

py_val_t env::eval_stmt_vec(stmt_vec_t s) {
  const int n = s->size();
  for (int i = 0; i < n; ++i) {
    py_val_t ret = env::eval_stmt((*s)[i]);
    if (ret == py_val_next) {
      continue;
    } else {
      return ret;
    }
  }
  // Todo: Return const interned value.
  return py_val_next;
}

py_val_t env::eval_stmt(stmt_t s) {
  switch (s->kind) {
    case stmt_kind_print:
      return eval_stmt_print(s);
    case stmt_kind_assignment:
      return eval_stmt_assignment(s);
    case stmt_kind_fundef:
      return eval_stmt_fundef(s);
    case stmt_kind_return:
      return eval_stmt_return(s);
    case stmt_kind_if:
      return eval_stmt_if(s);
    case stmt_kind_while:
      return eval_stmt_while(s);
    case stmt_kind_break:
      return eval_stmt_break(s);
    case stmt_kind_continue:
      return eval_stmt_continue(s);
    default:
      error("Not implemented");
      return 0;
  }
}

py_val_t env::eval_stmt_print(stmt_t s) {
  py_val_t v = eval_expr(s->u.e);
  if (py_is_int(v)) {
    stdout << py_val_int(v, bt, s->pos) << endl;
  } else if (py_is_string(v)) {
    stdout << *v->u.s << endl;
  } else {
    stdout << "Print againt not int" << endl;
  }
  return py_val_next;
}

py_val_t env::eval_stmt_assignment(stmt_t s) {
  expr_t target = s->u.a->target;
  if (target->IsLValue()) {
    py_val_t val = eval_expr(s->u.a->val);
    if (target->kind == expr_kind_var) {
      set(target->u.var, val);
      return py_val_next;
    }
  }
  error("stmt assingment not supported");
  return 0;
}

py_val_t env::eval_stmt_fundef(stmt_t s) {
  stmt_fundef_t stmt_fundef = s->u.d;
  py_val_t fun = mk_py_ifun(stmt_fundef->f,
                            stmt_fundef->ps,
                            stmt_fundef->b);
  set(stmt_fundef->f, fun);
  return py_val_next;
}

py_val_t env::eval_stmt_return(stmt_t s) {
  expr_t e = s->u.e;
  return eval_expr(e);
}

py_val_t env::eval_stmt_if(stmt_t s) {
  if_branch_vec_t branches = s->u.i;
  const int n = branches->size();
  for (int i = 0; i < n; ++i) {
    if_branch_t branch = (*branches)[i];
    if (branch->is_else) {
      if (i != n-1) {
        error("Invalid if branches. Else appears in the middle");
      }
      return eval_stmt_vec(branch->s);
    }
    py_val_t v = eval_expr(branch->e);
    if (!py_is_false(v)) {
      return eval_stmt_vec(branch->s);
    }
  }
  return py_val_next;
}

py_val_t env::eval_stmt_while(stmt_t s) {
  stmt_while_t w = s->u.w;
  while (true) {
    if (py_is_false(eval_expr(w->e))) {
      return py_val_next;
    }
    py_val_t v = eval_stmt_vec(w->b);
    if (v == py_val_continue || v == py_val_next) {
      continue;
    }
    if (v == py_val_break) {
      return py_val_next;
    }
    return v; // return xx;
  }
}

py_val_t env::eval_stmt_break(stmt_t s) {
  return py_val_break;
}

py_val_t env::eval_stmt_continue(stmt_t s) {
  return py_val_continue;
}

/*
 * Implementations of header.
 */
env_t mk_env(env_t genv, ostream& os) {
  return new env(genv, os);
}

void env_set(env_t e, string_t name, py_val_t a) {
  e->set(name, a);
}

py_val_t eval_file_input(env_t genv, file_input_t u) {
  // Todo: Check wrong 'break or continue'
  return genv->eval_stmt_vec(u);
}

ostream& operator<<(ostream& os, const env_t& e) {
  for (env::iterator it = e->entries.begin();
       it != e->entries.end(); ++it) {
    os << "key:" << it->first
       << " value:" << it->second << endl;
  }
  return os;
}
