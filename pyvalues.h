#ifndef MINIPY_PYVALUES_H__
#define MINIPY_PYVALUES_H__

#include "syntree.h"

#include <string>
#include <vector>

#include <iostream>

using namespace std;

class py_val;
/* すべては以下のpy_val_t型のデータ(32bit)として表現する */
typedef py_val* py_val_t;

/* py_val_tのベクタ */
//typedef struct py_val_vec * py_val_vec_t;
typedef vector<py_val_t>* py_val_vec_t;

class stack_trace;
typedef stack_trace* stack_trace_t;

/* Python値の種類を表す定数 */
typedef enum
{
  py_type_int,			/* 整数 */
  py_type_char,			/* 文字(長さ1の文字列) */
  py_type_float,		/* 浮動小数点 */
  py_type_tuple,		/* タプル */
  py_type_list,			/* リスト */
  py_type_dict,			/* 辞書 */
  py_type_string,		/* 文字列 */
  py_type_ifun,			/* Python関数 */
  py_type_nfun			/* Native関数 */
} py_type_t;

/* 文字列 */
// typedef struct py_string
// {
//   int n;			/* 文字数 */
//   char * a;			/* 文字列へのポインタ */
// } py_string, * py_string_t;

typedef string py_string;
typedef const py_string* py_string_t;

/* 辞書の1エントリ (key : value の組) */
class py_dict_entry {
 public:
  py_dict_entry(py_val_t k, py_val_t v)
      : key(k), val(v) {}
  py_val_t key;
  py_val_t val;
};
typedef py_dict_entry* py_dict_entry_t;

typedef vector<py_dict_entry_t> py_dict_entry_vec;
typedef py_dict_entry_vec* py_dict_entry_vec_t;

class py_ifun {
 public:
  py_ifun(string_t _name, str_vec_t _ps, stmt_vec_t _b)
      : name(_name), ps(_ps), b(_b) {}
  string_t name;			/* defされた名前 */
  str_vec_t ps;			/* 引数のベクタ(文字列のベクタ) */
  stmt_vec_t b;			/* 本体(文のベクタ) */
};
typedef py_ifun* py_ifun_t;

// typedef py_val_t (*C_fun_t)();	/* C関数 */
typedef py_val_t (*C_fun_t)(py_val_vec_t, stack_trace_t, src_pos_t);	/* C関数 */

/* Native関数 */
class py_nfun {
 public:
  py_nfun(string_t _name, int _arity, C_fun_t _f)
      : name(_name), arity(_arity), f(_f) {}
  string_t name;			/* 名前 */
  int arity;			/* 引数の数 */
  C_fun_t f;			/* C関数 */
};
typedef py_nfun* py_nfun_t;

/* 構造体中に置かれるPython値．
   整数，文字，Noneはpy_val_t (32bit)の中にすべて格納されることに注意 */

class py_val {
 public:
  py_val(py_type_t _type) 
      : type(_type) {}

  bool is_false() {
    switch (type) {
      case py_type_float:
        return u.f == 0.0;
      case py_type_string:
        return u.s->empty();
      case py_type_tuple:
      case py_type_list:
        return u.l->size() == 0;
      case py_type_dict:
        return u.d->size() == 0;
      default:
        return false;
    }
  }
  
  py_type_t type;
  union {
    double f;			/* 浮動小数点数 */
    py_string_t s;		/* 文字列 */
    py_ifun_t i;			/* Python関数 */
    py_nfun_t n;			/* Native関数 */
    py_val_vec_t l;		/* tuple or list */
    py_dict_entry_vec_t d;	/* dict */
  } u;
};

/*
py_val_tに格納されるbit列の表現

              ...1 : int                    (最下位bit=1は整数)
             ...00 : pointer (None = 0)     (py_valへのポインタ．上を見よ)
             ...10 : other values           (その他．文字と，処理系内部でのみ使われる特別な値)
         ...   010 : char                   (文字)
         ...   110 : special                (処理系内部でのみ使われる特別な値)
        ... 000110 : continue　　　　　　　 ある文がcontinueを実行したことを表す値
        ... 001110 : break　　　　　　　　　ある文がbreakを実行したことを表す値
        ... 010110 : next                   ある文がcontinue/break/returnを実行しなかった
                                            ことを表す値
        ... 011110 : not found              変数の値を得ようとしたら見つからなかったことを示す値
        ... 100110 : var is global          変数の値を得ようとしたらglobal変数として
                                            見つかったことを示す値

特別な値については今は理解できなくて良い(後に実際に評価器を作る際に説明する)．

*/

/* 上記のデータ表現を助けるための定数 */
enum {
  py_val_tag_int      = 1,	/* 整数のタグ */
  py_val_tag_other    = 2,	/* other valuesのタグ */
  py_val_tag_char     = (0 << 2) + py_val_tag_other, /* 文字のタグ */
  py_val_tag_special  = (1 << 2) + py_val_tag_other,
  py_val_continue_    = (0 << 3) + py_val_tag_special,
  py_val_break_       = (1 << 3) + py_val_tag_special,
  py_val_next_        = (2 << 3) + py_val_tag_special,
  py_val_not_found_   = (3 << 3) + py_val_tag_special,
  py_val_global_      = (4 << 3) + py_val_tag_special,
  py_val_none         = 0
};

const py_val_t py_val_continue  = ((py_val_t)py_val_continue_);
const py_val_t py_val_break     = ((py_val_t)py_val_break_);
const py_val_t py_val_next      = ((py_val_t)py_val_next_);
const py_val_t py_val_not_found = ((py_val_t)py_val_not_found_);
const py_val_t py_val_global    = ((py_val_t)py_val_global_);

/* Python値を作り出す関数たち */
py_val_t mk_py_int(int i);
py_val_t mk_py_char(int c);
py_val_t mk_py_float(double f);
py_val_t mk_py_string(string_t a);
py_val_t mk_py_ifun(string_t name, str_vec_t ps, stmt_vec_t b);
py_val_t mk_py_nfun(string_t name, int arity, C_fun_t f);
py_val_t mk_py_tuple(py_val_vec_t l);
py_val_t mk_py_list(py_val_vec_t l);
py_val_t mk_py_dict(py_val_vec_t l);

  /* if 文でfalse扱いされる値なら1 */
int py_is_false(py_val_t p);

  /* それぞれpが整数，文字か，浮動小数点なら1 */
int py_is_int(py_val_t p);
int py_is_char(py_val_t p);
int py_is_float(py_val_t p);
int py_is_string(py_val_t p);
int py_is_ifun(py_val_t a);
int py_is_nfun(py_val_t a);
int py_is_tuple(py_val_t a);
int py_is_list(py_val_t a);
int py_is_dict(py_val_t a);

py_val_t mk_py_none();
int py_is_none(py_val_t a);


/* それぞれpが整数/文字/浮動小数点のときに，その値を取り出す */
int py_val_int(py_val_t p, stack_trace_t bt, src_pos_t pos);
int py_val_char(py_val_t p, stack_trace_t bt, src_pos_t pos);
double py_val_float(py_val_t p, stack_trace_t bt, src_pos_t pos);

// My declarations.
// int py_val_int(py_val_t p);
// int py_val_char(py_val_t p);
// double py_val_float(py_val_t p);


class stack_trace_entry;
typedef stack_trace_entry* stack_trace_entry_t;

class stack_trace {
 public:
  vector<stack_trace_entry_t>* entries;
  void push(char* name, src_pos_t call_site);
  void pop(char* name, src_pos_t call_site);
  stack_trace_entry_t pop_last();
  void runtime_error(src_pos_t pos, const string& msg);
  stack_trace_entry_t& operator[](int i) {
    return (*entries)[i];
  }
  int size() {
    return entries->size();
  }
  void add(stack_trace_entry_t x) {
    entries->push_back(x);
  }
  void print() {
    cout << "stack_trace::print Not implemented" << endl;
  }
};

py_dict_entry_t mk_py_dict_entry(py_val_t key, py_val_t val);

template <class T> T vec_del(vector<T>* v, int i) {
  return *(v->erase(v->begin() + i));
}

py_dict_entry_vec_t mk_py_dict_entry_vec();
void py_dict_entry_vec_add(py_dict_entry_vec_t v, py_dict_entry_t x);
void py_dict_entry_vec_del(py_dict_entry_vec_t v, int i);
int py_dict_entry_vec_size(py_dict_entry_vec_t v);
py_dict_entry_t py_dict_entry_vec_get(py_dict_entry_vec_t v, int i);

typedef vector<py_val_t> py_val_vec;
typedef py_val_vec* py_val_vec_t;

/* Python値のベクタ */
py_val_vec_t mk_py_val_vec();
void py_val_vec_add(py_val_vec_t v, py_val_t x);
int py_val_vec_size(py_val_vec_t v);
py_val_t py_val_vec_get(py_val_vec_t v, int i);
void py_val_vec_set(py_val_vec_t v, int i, py_val_t x);
py_val_t py_val_vec_del(py_val_vec_t v, int i);

/* error reporting */
stack_trace_t mk_stack_trace();
void stack_trace_push(stack_trace_t bt, char * name, src_pos_t call_site);
void stack_trace_pop(stack_trace_t bt, char * name, src_pos_t call_site);
void runtime_error(stack_trace_t bt, src_pos_t pos, char * msg);

ostream& operator<<(ostream& os, const py_val_t &obj);

#endif  // MINIPY_PYVALUES_H__
