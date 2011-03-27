#ifndef MINIPY_PYVALUES_H__
#define MINIPY_PYVALUES_H__

#include "syntree.h"

#include <string>
#include <vector>

#include <iostream>

using namespace std;

class py_val;
/* $B$9$Y$F$O0J2<$N(Bpy_val_t$B7?$N%G!<%?(B(32bit)$B$H$7$FI=8=$9$k(B */
typedef py_val* py_val_t;

/* py_val_t$B$N%Y%/%?(B */
//typedef struct py_val_vec * py_val_vec_t;
typedef vector<py_val_t>* py_val_vec_t;

class stack_trace;
typedef stack_trace* stack_trace_t;

/* Python$BCM$N<oN`$rI=$9Dj?t(B */
typedef enum
{
  py_type_int,			/* $B@0?t(B */
  py_type_char,			/* $BJ8;z(B($BD9$5(B1$B$NJ8;zNs(B) */
  py_type_float,		/* $BIbF0>.?tE@(B */
  py_type_tuple,		/* $B%?%W%k(B */
  py_type_list,			/* $B%j%9%H(B */
  py_type_dict,			/* $B<-=q(B */
  py_type_string,		/* $BJ8;zNs(B */
  py_type_ifun,			/* Python$B4X?t(B */
  py_type_nfun			/* Native$B4X?t(B */
} py_type_t;

/* $BJ8;zNs(B */
// typedef struct py_string
// {
//   int n;			/* $BJ8;z?t(B */
//   char * a;			/* $BJ8;zNs$X$N%]%$%s%?(B */
// } py_string, * py_string_t;

typedef string py_string;
typedef const py_string* py_string_t;

/* $B<-=q$N(B1$B%(%s%H%j(B (key : value $B$NAH(B) */
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
  string_t name;			/* def$B$5$l$?L>A0(B */
  str_vec_t ps;			/* $B0z?t$N%Y%/%?(B($BJ8;zNs$N%Y%/%?(B) */
  stmt_vec_t b;			/* $BK\BN(B($BJ8$N%Y%/%?(B) */
};
typedef py_ifun* py_ifun_t;

// typedef py_val_t (*C_fun_t)();	/* C$B4X?t(B */
typedef py_val_t (*C_fun_t)(py_val_vec_t, stack_trace_t, src_pos_t);	/* C$B4X?t(B */

/* Native$B4X?t(B */
class py_nfun {
 public:
  py_nfun(string_t _name, int _arity, C_fun_t _f)
      : name(_name), arity(_arity), f(_f) {}
  string_t name;			/* $BL>A0(B */
  int arity;			/* $B0z?t$N?t(B */
  C_fun_t f;			/* C$B4X?t(B */
};
typedef py_nfun* py_nfun_t;

/* $B9=B$BNCf$KCV$+$l$k(BPython$BCM!%(B
   $B@0?t!$J8;z!$(BNone$B$O(Bpy_val_t (32bit)$B$NCf$K$9$Y$F3JG<$5$l$k$3$H$KCm0U(B */

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
    double f;			/* $BIbF0>.?tE@?t(B */
    py_string_t s;		/* $BJ8;zNs(B */
    py_ifun_t i;			/* Python$B4X?t(B */
    py_nfun_t n;			/* Native$B4X?t(B */
    py_val_vec_t l;		/* tuple or list */
    py_dict_entry_vec_t d;	/* dict */
  } u;
};

/*
py_val_t$B$K3JG<$5$l$k(Bbit$BNs$NI=8=(B

              ...1 : int                    ($B:G2<0L(Bbit=1$B$O@0?t(B)
             ...00 : pointer (None = 0)     (py_val$B$X$N%]%$%s%?!%>e$r8+$h(B)
             ...10 : other values           ($B$=$NB>!%J8;z$H!$=hM}7OFbIt$G$N$_;H$o$l$kFCJL$JCM(B)
         ...   010 : char                   ($BJ8;z(B)
         ...   110 : special                ($B=hM}7OFbIt$G$N$_;H$o$l$kFCJL$JCM(B)
        ... 000110 : continue$B!!!!!!!!!!!!!!(B $B$"$kJ8$,(Bcontinue$B$r<B9T$7$?$3$H$rI=$9CM(B
        ... 001110 : break$B!!!!!!!!!!!!!!!!!!$"$kJ8$,(Bbreak$B$r<B9T$7$?$3$H$rI=$9CM(B
        ... 010110 : next                   $B$"$kJ8$,(Bcontinue/break/return$B$r<B9T$7$J$+$C$?(B
                                            $B$3$H$rI=$9CM(B
        ... 011110 : not found              $BJQ?t$NCM$rF@$h$&$H$7$?$i8+$D$+$i$J$+$C$?$3$H$r<($9CM(B
        ... 100110 : var is global          $BJQ?t$NCM$rF@$h$&$H$7$?$i(Bglobal$BJQ?t$H$7$F(B
                                            $B8+$D$+$C$?$3$H$r<($9CM(B

$BFCJL$JCM$K$D$$$F$O:#$OM}2r$G$-$J$/$FNI$$(B($B8e$K<B:]$KI>2A4o$r:n$k:]$K@bL@$9$k(B)$B!%(B

*/

/* $B>e5-$N%G!<%?I=8=$r=u$1$k$?$a$NDj?t(B */
enum {
  py_val_tag_int      = 1,	/* $B@0?t$N%?%0(B */
  py_val_tag_other    = 2,	/* other values$B$N%?%0(B */
  py_val_tag_char     = (0 << 2) + py_val_tag_other, /* $BJ8;z$N%?%0(B */
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

/* Python$BCM$r:n$j=P$94X?t$?$A(B */
py_val_t mk_py_int(int i);
py_val_t mk_py_char(int c);
py_val_t mk_py_float(double f);
py_val_t mk_py_string(string_t a);
py_val_t mk_py_ifun(string_t name, str_vec_t ps, stmt_vec_t b);
py_val_t mk_py_nfun(string_t name, int arity, C_fun_t f);
py_val_t mk_py_tuple(py_val_vec_t l);
py_val_t mk_py_list(py_val_vec_t l);
py_val_t mk_py_dict(py_val_vec_t l);

  /* if $BJ8$G(Bfalse$B07$$$5$l$kCM$J$i(B1 */
int py_is_false(py_val_t p);

  /* $B$=$l$>$l(Bp$B$,@0?t!$J8;z$+!$IbF0>.?tE@$J$i(B1 */
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


/* $B$=$l$>$l(Bp$B$,@0?t(B/$BJ8;z(B/$BIbF0>.?tE@$N$H$-$K!$$=$NCM$r<h$j=P$9(B */
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

/* Python$BCM$N%Y%/%?(B */
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
