#ifndef MINIPY_EVAL_H__
#define MINIPY_EVAL_H__

#include "syntree.h"
#include "pyvalues.h"

#include <iostream>

using namespace std;

class env;
typedef env* env_t;

void env_set(env_t e, string_t name, py_val_t a);
env_t mk_env(env_t genv = NULL, ostream& os = std::cout);
py_val_t eval_file_input(env_t e, file_input_t fi);

ostream& operator<<(ostream& os, const env_t &obj);

#endif  // MINIPY_EVAL_H__
