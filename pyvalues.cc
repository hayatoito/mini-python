#include <cassert>
#include <string>
#include <cstdio>

#include "base.h"
#include "tokenizer.h"
#include "syntree.h"
#include "parser.h"
#include "pyvalues.h"


// class symbol;
// symbol mk_symbol

#include <stdint.h>

namespace {

int toint(py_val_t val) {
  // return reinterpret_cast<int>(val); // Causes error.
  return reinterpret_cast<intptr_t>(val);
}


}  // namespace



class stack_trace_entry {
 public:
  stack_trace_entry(char* _name, src_pos_t _call_site)
      : name(_name), call_site(_call_site) {}
  char * name;
  src_pos_t call_site;
};

stack_trace_entry_t mk_stack_trace_entry(char * name, src_pos_t call_site) {
  return new stack_trace_entry(name, call_site);
}

void stack_trace::push(char* name, src_pos_t call_site) {
  entries->push_back(mk_stack_trace_entry(name, call_site));
}

void stack_trace::pop(char* name, src_pos_t call_site) {
  // Todo
  cout << "stack_trace::pop not implemented" << endl;
  cout << "name:" << name
       << " pos:" << call_site->line_no << endl;
}

stack_trace_entry_t stack_trace::pop_last() {
  stack_trace_entry_t last = entries->back();
  entries->pop_back();
  return last;
}

void stack_trace::runtime_error(src_pos_t pos, const string& msg) {
  cout << "runtime errro: " << msg << endl;
  cout << pos->filename << ":" << pos->line_no
       << " col:" << pos->column_no << endl;
}

stack_trace_t mk_stack_trace() {
  return new stack_trace();
}

int stack_trace_size(stack_trace_t bt) {
  return bt->size();
}

stack_trace_entry_t stack_trace_get(stack_trace_t bt, int i) {
  return (*bt)[i];
}

void stack_trace_add(stack_trace_t bt, stack_trace_entry_t x) {
  bt->add(x);
}

stack_trace_entry_t stack_trace_pop_last(stack_trace_t bt) {
  return bt->pop_last();
}

void stack_trace_push(stack_trace_t bt, char * name, src_pos_t call_site) {
  bt->push(name, call_site);
}

void stack_trace_pop(stack_trace_t bt, char * name, src_pos_t call_site) {
  bt->pop(name, call_site);
}

void print_stack_trace(stack_trace_t bt) {
  bt->print();
}

void runtime_error(stack_trace_t bt, src_pos_t pos, const char * msg) {
  bt->runtime_error(pos, msg);
}

py_dict_entry_vec_t mk_py_dict_entry_vec() {
  return new py_dict_entry_vec;
}

void py_dict_entry_vec_add(py_dict_entry_vec_t v, py_dict_entry_t x) {
  v->push_back(x);
}

void py_dict_entry_vec_del(py_dict_entry_vec_t v, int i) {
  vec_del(v, i);
}

py_dict_entry_t py_dict_entry_vec_get(py_dict_entry_vec_t v, int i) {
  return (*v)[i];
}

int py_dict_entry_vec_size(py_dict_entry_vec_t v) {
  return v->size();
}

py_val_t mk_py_none() {
  return reinterpret_cast<py_val_t>(0);
}
int py_is_none(py_val_t a) {
  return a == 0;
}

int py_is_false(py_val_t a) {
  /* Pythonで偽とみなされる値ならば1:
     後にif文の条件部を評価する際に使う
     None
     整数の0
     空文字列("")
     空タプル(())
     空リスト([])
     空辞書({})
 */
  static stack_trace_t st = NULL;
  static src_pos_t pos = NULL;

  if (py_is_none(a)) {
    return true;
  } else if (py_is_int(a)) {
    return py_val_int(a, st, pos) == 0;
  } else {
    return a->is_false();
  }
}
/* 参考のため整数と浮動小数点数の場合だけ
   授業の説明に沿った答を載せる */

/* Python値の整数を作る */
py_val_t mk_py_int(int i) {
  //return (py_val_t)((i << 1) + 1);
  return reinterpret_cast<py_val_t>((i << 1) + py_val_tag_int);
}

/* このPython値は整数か? */
int py_is_int(py_val_t p) {
  return toint(p) & py_val_tag_int;
}

/* このPython値が整数ならその値をCの通常の整数値として取り出す */
int py_val_int(py_val_t pi, stack_trace_t bt, src_pos_t pos) {
  if (py_is_int(pi)) {
    return toint(pi) >> 1;
  } else {
    runtime_error(bt, pos, "require <int> value\n");
    return 0;
  }
}

/* Python値の文字を作る */
py_val_t mk_py_char(int c) {
  return reinterpret_cast<py_val_t>((c << 3) + py_val_tag_char);
}

/* このPython値は文字か? */
int py_is_char(py_val_t p) {
  return toint(p) & py_val_tag_char;
}

/* このPython値が文字ならその値をCの通常の整数値として取り出す */
int py_val_char(py_val_t pc, stack_trace_t bt, src_pos_t pos) {
  if (py_is_char(pc)) {
    return toint(pc) >> 3;
  } else {
    runtime_error(bt, pos, "require <char> value\n");
    return 0;
  }
}

/* py_val構造体を割り当てる．型はtypeで示される */
py_val_t alloc_boxed(py_type_t type) {
  return new py_val(type);
}

int py_is_boxed(py_val_t v) {
  return (toint(v) & 3) == 0;
}

/* Python値の浮動小数点数を作る */
py_val_t mk_py_float(double f) {
  py_val_t r = alloc_boxed(py_type_float);
  r->u.f = f;
  return r;
}

/* このPython値は浮動小数点数か? */
int py_is_float(py_val_t p) {
  return py_is_boxed(p) && p->type == py_type_float;
}

/* このPython値が浮動小数点数ならその値を
   Cの通常の浮動小数点数値として取り出す */
double py_val_float(py_val_t v, stack_trace_t bt, src_pos_t pos) {
  if (py_is_float(v)) {
    return v->u.f;
  } else {
    runtime_error(bt, pos, "require <float> value\n");
    return 0.0;
  }
}

/* タプルを作る (lには格納されるべきPython値が入っている) */
py_val_t mk_py_tuple(py_val_vec_t l) {
  py_val_t a = alloc_boxed(py_type_tuple);
  a->u.l = l;
  return a;
}

int py_is_tuple(py_val_t a) {
  return py_is_boxed(a) && a->type == py_type_tuple;
}

/* リストを作る (lには格納されるべきPython値が入っている) */
py_val_t mk_py_list(py_val_vec_t l) {
  py_val_t a = alloc_boxed(py_type_list);
  a->u.l = l;
  return a;
}

int py_is_list(py_val_t a) {
  return py_is_boxed(a) && a->type == py_type_list;
}

/* 辞書を作るための補助. 1エントリ(key : val)を作る */
py_dict_entry_t mk_py_dict_entry(py_val_t key, py_val_t val) {
  return new py_dict_entry(key, val);
}

/* 辞書を作る (lには格納されるべきキー : 値がが入っている) */
// py_val_t mk_py_dict(py_val_vec_t l) {
py_val_t mk_py_dict(py_dict_entry_vec_t l) {
  py_val_t a = alloc_boxed(py_type_dict);
  a->u.d = l;
  return a;
}

int py_is_dict(py_val_t a) {
  return py_is_boxed(a) && a->type == py_type_dict;
}

/* 文字列を作る.
   0文字, 1文字の場合に注意 */
py_val_t mk_py_string(string_t s) {
  py_val_t a = alloc_boxed(py_type_string);
  a->u.s = new string(*s);
  return a;
}

int py_is_string(py_val_t a) {
  return py_is_boxed(a) && a->type == py_type_string;
}

/* Python関数を作る */
py_val_t mk_py_ifun(string_t name, str_vec_t ps, stmt_vec_t b) {
  py_val_t v = alloc_boxed(py_type_ifun);
  v->u.i = new py_ifun(name, ps, b);
  return v;
}

int py_is_ifun(py_val_t a) {
  return py_is_boxed(a) && a->type == py_type_ifun;
}

/* Native関数を作る */
py_val_t mk_py_nfun(string_t name, int arity, C_fun_t f) {
  LOG(INFO) << "mk_py_nfun " << *name;
  py_val_t v = alloc_boxed(py_type_nfun);
  v->u.n = new py_nfun(name, arity, f);
  return v;
}

int py_is_nfun(py_val_t a) {
  return py_is_boxed(a) && a->type == py_type_nfun;
}

py_val_vec_t mk_py_val_vec() {
  return new py_val_vec;
}

void py_val_vec_add(py_val_vec_t a, py_val_t v) {
  return a->push_back(v);
}

py_val_t py_val_vec_get(py_val_vec_t a, int i) {
  return (*a)[i];
}

void py_val_vec_set(py_val_vec_t a, int i, py_val_t x) {
  (*a)[i] = x;
}

py_val_t py_val_vec_del(py_val_vec_t a, int i) {
  return vec_del(a, i);
}

int py_val_vec_size(py_val_vec_t a) {
  return a->size();

}

ostream& operator<<(ostream& os, const py_val_t& v) {
  if (py_is_int(v)) {
    os << "<int> " << py_val_int(v, NULL, NULL);
  } else if (py_is_float(v)) {
    os << "<float> " << py_val_float(v, NULL, NULL);
  } else if (py_is_char(v)) {
    os << "<char>";
  } else {
    switch (v->type) {
      case py_type_tuple:
        os << "<tuple>";
        break;
      case py_type_ifun:
        os << "<ifun>";
        break;
      case py_type_nfun:
        os << "<nfun>";
        break;
      default:
        os << "<Todo(unknown)>";
        break;
    }
  }
  return os;
}

