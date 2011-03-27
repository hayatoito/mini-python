#include "native.h"

#include "base.h"
#include "pyvalues.h"
#include "eval.h"

namespace {

string_t mk_symbol(string s) {
  // Todo: Interns s
  return new string(s);
}
py_val_t add_int(py_val_t a, py_val_t b, stack_trace_t bt, src_pos_t pos) {
  return mk_py_int(py_val_int(a, bt, pos) +
                   py_val_int(b, bt, pos));
}

// py_val_t add_float(py_val_t a, py_val_t b, stack_trace_t bt, src_pos_t pos) {
//   LOG(ERROR) << "Not implemented.";
//   return 0;
// }

py_val_t add_string(py_val_t a, py_val_t b, stack_trace_t bt, src_pos_t pos) {
  // return mk_py_string(&(*a->u.s + *b->u.s));
  py_string s(*a->u.s + *b->u.s);
  return mk_py_string(&s);
}

py_val_t add(py_val_vec_t args, stack_trace_t bt, src_pos_t pos) {
  py_val_t a = (*args)[0];
  py_val_t b = (*args)[1];
  if (py_is_int(a)) {
    if (py_is_int(b)) {
      return add_int(a, b, bt, pos);
    }
  } else if (py_is_string(a)) {
    if (py_is_string(b)) {
      return add_string(a, b, bt, pos);
    }
  }
  LOG(INFO) << "Error: invalid operation add against ...";
  return NULL;
}

py_val_t lt(py_val_vec_t args, stack_trace_t bt, src_pos_t pos) {
  int a = py_val_int((*args)[0], bt, pos);
  int b = py_val_int((*args)[1], bt, pos);
  return a < b ? mk_py_int(1) : mk_py_int(0);
}

}  // namespace


void register_native(env_t genv, string_t name, int arity, C_fun_t f) {
  env_set(genv, name, mk_py_nfun(name, arity, f));
}

py_val_t eval_file_input(file_input_t u) {
  // Todo: Check wrong 'break or continue'
  env_t env = mk_env(NULL, std::cout);
  register_navite_functions(env);
  return eval_file_input(env, u);
}

void register_navite_functions(env_t genv) {
  register_native(genv, mk_symbol("_plus"), 2, &add);
  register_native(genv, mk_symbol("_lt"), 2, &lt);
}
