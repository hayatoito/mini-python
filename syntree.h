#ifndef MINIPY_SYNTREE_H__
#define MINIPY_SYNTREE_H__

#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>

#include "base.h"

#include "tokenizer.h"

using namespace std;

typedef const string* string_t;

class SrcPos {
 public:
  SrcPos(string_t _filename, int _line_no, int _column_no)
      : filename(_filename), line_no(_line_no), column_no(_column_no) {
  }
  string_t filename;
  int line_no;
  int column_no;
};

typedef const SrcPos* src_pos_t;

/* ------------------- $B<0(B (expression) ------------------- */

/*$B!V<0!W$N<oN`$r$"$i$o$9Dj?t$?$A(B:

var           : $BJQ?t!%Nc(B: x + y $B$K=P$F$/$k(B x $B$d(B y$B$J$I(B
literal_int   : $B@0?t%j%F%i%k!%Nc(B: 15, 38 $B$J$I(B
literal_float : $BIbF0>.?tE@?t%j%F%i%k!%Nc(B: 15.3, 38.5 $B$J$I(B
literal_string: $BJ8;zNs%j%F%i%k!%Nc(B: "abc", "def" $B$J$I(B
literal_none  : None
display_tuple : $B%?%W%k!%Nc(B (1, 2, 3), (4, x, f(y)) $B$J$I(B
display_list  : $B%j%9%H!%Nc(B [1, 2, 3], [4, x, (f(y), z) ] $B$J$I(B
display_dict  : $B<-=q!%Nc(B { 1 : 2, 2 : 3, 3 : 4 } $B$J$I(B

 */

typedef enum {
  expr_kind_var,		/* $BJQ?t(B */
  expr_kind_literal_int,	/* $B@0?t%j%F%i%k(B */
  expr_kind_literal_float,	/* $BIbF0>.?tE@%j%F%i%k(B */
  expr_kind_literal_string,	/* $BJ8;zNs%j%F%i%k(B */
  expr_kind_none,		/* None */
  expr_kind_display_tuple,	/* $B%?%W%k$r:n$k(B ( a, b, c,... ) */
  expr_kind_display_list,	/* $B%j%9%H$r:n$k(B [ a, b, c,...] */
  expr_kind_display_dict,	/* $B<-=q$r:n$k(B   { a : x, b : y } */
  expr_kind_paren,		/* $B<0$r3g8L$G$/$/$C$?$b$N(B ( e ) */
  expr_kind_operator,		/* $B$"$i$f$k1i;;;R(B(+, -, ...)*/
  expr_kind_attref,		/* e.f ($B2<$r8+$h(B) */
  expr_kind_subscript,		/* e[e] ($B<-=q!$%j%9%H!$%?%W%k!$J8;zNs$N(B
				   $BMWAG$r%"%/%;%9$9$k(B
				   a[x], b[3], b[3][2] etc.) */
  expr_kind_call		/* $B4X?t8F$S=P$7(B
				   e(e,e,..) */
} expr_kind_t;


class Expr;
class ExprOperator;
class ExprAttref;
class ExprCall;

typedef const Expr* expr_t;
typedef const ExprOperator* expr_operator_t;
typedef const ExprAttref* expr_attref_t;
typedef const ExprCall* expr_call_t;

typedef vector<string_t> str_vec;
typedef vector<expr_t> expr_vec;

typedef const str_vec* str_vec_t;
typedef const expr_vec* expr_vec_t;



// typedef struct expr * const Expr*;
// typedef struct expr_vec * const ExprVec*;

/* $B1i;;;R$K$O(B prefix : $B1i;;;R$,%*%Z%i%s%I$NA0$KCV$+$l$k$b$N(B
              infix : $B1i;;;R$,(B2$B8D$N%*%Z%i%s%I$N4V$KCV$+$l$k$b$N(B
   $B$,$"$k!%(B
   prefix :  - x $B$J$I(B
   infix :   a - b $B$J$I(B

- $B$O(B prefix$B$G$b(Binfix$B$H$b;H$($k$N$G$=$l$r6hJL$9$k(B
*/
typedef enum {
  fix_kind_prefix,
  fix_kind_infix
} fix_kind_t;

/* $B1i;;<0(B (a + b, - x, etc.) */
class ExprOperator {
 public:
  ExprOperator(token_kind_t k, fix_kind_t f, expr_vec_t a)
      : kind(k), fix(f), args(a) {
  }
  token_kind_t kind;		/* $B1i;;;R$N<oN`(B */
  fix_kind_t fix;		/* prefix or infix
				   (unary$B$N(B+/-$B$H(Bbinary$B$N(B+/-$B$N6hJL(B) */
  expr_vec_t args;		/* $B%*%Z%i%s%I(B */
};



/* expr_attr:
   a.f $B$_$?$$$J<0!%Nc!'(B

   L = [1, 2, 3]
   L.append(4)
   ^^^^^^^^$B$3$l(B

   mini-Python$B$G$O$3$l$O0J2<$N7A(B()$B$G$N$_;H$&(B

   e0.f(e1, e2, ...)

$B$D$^$j(B L.f $BC1FH$G$N;HMQ$O$;$:!$I,$:4X?t8F$S=P$7$N7A$G$N$_8=$l$k$3$H$H(B
$B$9$k!%$=$7$F!$>e$N$h$&$J<0$O7A<0E*$K(B

   f(e0, e1, e2, ...)

$B$HF1CM$H$_$J$9!%(B

$B$D$^$j!$(B
   L.append(4) $B$O(B append(L, 4)$B$H$$$&$3$H(B
 */
class ExprAttref {
 public:
  expr_t a;
  string_t f;

  ExprAttref(expr_t expr, string_t str)
      : a(expr), f(str) {
  }
};

/* expr_call:
   $B4X?t8F$S=P$7(B f(x, y, z) */
class ExprCall {
 public:
  expr_t f;			/* $(IJ=$AJ}(B */
  expr_vec_t args;		/* $AR}J}(B */
  ExprCall(expr_t expr, expr_vec_t ls)
      : f(expr), args(ls) {
  }
};


/* $B<0A4BN$O0J2<$N$h$&$J(B, $B<oN`$r$"$i$o$9(Bfield (kind)$B$H(B, $B$=$l$K1~$8$?(B
   field$B$N(B union */
class Expr {
 public:
  Expr(const expr_kind_t k, src_pos_t p)
      : kind(k), pos(p) {
  }
  const expr_kind_t kind;		/* $B<0$N<oN`(B */
  src_pos_t pos;
  union {
    string_t var;			/* var */
    int lit_i;			/* int literal */
    double lit_f;		/* float literal */
    string_t lit_s;		/* string literal */
    expr_t paren;		/* parened */
    expr_vec_t disp;		/* list/tuple/dict display */
    const ExprOperator* op;		/* operator */
    const ExprAttref* atr;		/* attribute ref */
    expr_vec_t sub;		/* subscript */
    const ExprCall* call;		/* call */
  } u;

  static expr_t MakeVar(string_t var, src_pos_t pos) {
    Expr* e = new Expr(expr_kind_var, pos);
    e->u.var = var;
    return e;
  }

  static expr_t MakeLiteralInt(int i, src_pos_t pos) {
    Expr* e = new Expr(expr_kind_literal_int, pos);
    e->u.lit_i = i;
    return e;
  }

  static expr_t MakeLiteralFloat(double f, src_pos_t pos) {
    Expr* e = new Expr(expr_kind_literal_float, pos);
    e->u.lit_f = f;
    return e;
  }

  static expr_t MakeLiteralString(string_t s, src_pos_t pos) {
    Expr* e = new Expr(expr_kind_literal_string, pos);
    e->u.lit_s = s;
    return e;
  }

  static expr_t MakeNone(src_pos_t pos) {
    Expr* e = new Expr(expr_kind_none, pos);
    return e;
  }

  static expr_t MakeParen(expr_t expr, src_pos_t pos) {
    Expr* e = new Expr(expr_kind_paren, pos);
    e->u.paren = expr;
    return e;
  }


  // Todo: Use template <T> MakeDisplay
  //  .. = new Expr(T, pos)

  static expr_t MakeDisplayTuple(expr_vec_t a, src_pos_t pos) {
    Expr* e = new Expr(expr_kind_display_tuple, pos);
    e->u.disp = a;
    return e;
  }

  static expr_t MakeDisplayList(expr_vec_t a, src_pos_t pos) {
    Expr* e = new Expr(expr_kind_display_list, pos);
    e->u.disp = a;
    return e;
  }

  static expr_t MakeDisplayDict(expr_vec_t a, src_pos_t pos) {
    Expr* e = new Expr(expr_kind_display_dict, pos);
    e->u.disp = a;
    return e;
  }

  static expr_t MakeOperator(token_kind_t op, fix_kind_t f,
                                  expr_vec_t a, src_pos_t pos) {
    Expr* e = new Expr(expr_kind_operator, pos);
    e->u.op = new ExprOperator(op, f, a);
    return e;
  }

  static expr_t MakeInfixOperator(token_kind_t op, expr_t a,
                                       expr_t b, src_pos_t pos) {
    expr_vec* args = new expr_vec;
    args->push_back(a);
    args->push_back(b);
    return MakeOperator(op, fix_kind_infix, args, pos);
  }

  static expr_t MakePrefixOperator(token_kind_t op, expr_t a,
                                        src_pos_t pos) {
    expr_vec* args = new expr_vec;
    args->push_back(a);
    return MakeOperator(op, fix_kind_prefix, args, pos);
  }

  static expr_t MakeAttref(expr_t a, string_t f,
                                src_pos_t pos) {
    Expr* e = new Expr(expr_kind_attref, pos);
    e->u.atr = new ExprAttref(a, f);
    return e;
  }

  static expr_t MakeSubscript(expr_t a, expr_t i,
                                src_pos_t pos) {
    Expr* e = new Expr(expr_kind_subscript, pos);
    expr_vec* args = new expr_vec;
    args->push_back(a);
    args->push_back(i);
    e->u.sub = args;
    return e;
  }

  static expr_t MakeCall(expr_t f, expr_vec_t args,
                              src_pos_t pos) {
    Expr* e = new Expr(expr_kind_call, pos);
    e->u.call = new ExprCall(f, args);
    return e;
  }

  bool IsLValue() const {
    return kind == expr_kind_var || kind == expr_kind_subscript;
  }

};



/* ------------------- $BJ8(B (statement) ------------------- */

/* $BJ8$N<oN`(B */
typedef enum {
  stmt_kind_expression,		/* $B<0J8!%<0$r=q$/$H$=$l$,J8$K$J$k!%(B
				   $BNc!'(B  f(x) */
  stmt_kind_assignment,		/* $BBeF~(B.  a = x, a[x] = b, etc. */
  stmt_kind_del,		/* del.   del a[x] */
  stmt_kind_pass,		/* pass */
  stmt_kind_return,		/* return */
  stmt_kind_break,		/* break */
  stmt_kind_continue,		/* continue */
  stmt_kind_print,		/* print */
  stmt_kind_global,		/* global X$B$OJQ?t(BX$B$,(Bglobal$BJQ?t$G$"$k(B
				   $B$H$$$&@k8@(B */
  stmt_kind_if,			/* if$BJ8(B */
  stmt_kind_while,		/* while$BJ8(B */
  stmt_kind_for,		/* for$BJ8(B */
  stmt_kind_fundef,		/* def$BJ8!%$3$l$b<B$OJ8$N0l<o(B */
} stmt_kind_t;

/* $BJ8A4BN$O(B stmt $B9=B$BN$G(B, $BCf?H$O8e$KDj5A$5$l$k(B. stmt_t $B$O$=$l$X$N%]%$%s%?(B
   $B$H$7$F$3$3$GDj5A$7$F$*$/(B */
// typedef struct stmt * stmt_t;
/* stmt_vec_t$B$OJ8$rJB$Y$?$b$N$G(B, $B3F=j(B(for$BJ8$d4X?tDj5A$N(Bbody$BItJ,$J$I(B)$B$G(B,
   $BJ#?t$NJ8$r$J$i$Y$?$b$N$rI=$9$N$KMQ$$$k(B */
// typedef struct stmt_vec * stmt_vec_t;

class Stmt;
class StmtAssignment;
class IfBranch;
class StmtWhile;
class StmtFor;
class StmtFundef;

typedef const Stmt* stmt_t;
typedef const StmtAssignment* stmt_assignment_t;
typedef const IfBranch* if_branch_t;
typedef const StmtWhile* stmt_while_t;
typedef const StmtFor* stmt_for_t;
typedef const StmtFundef* stmt_fundef_t;

typedef vector<stmt_t> stmt_vec;
typedef vector<if_branch_t> if_branch_vec;

typedef const stmt_vec* stmt_vec_t;
typedef const if_branch_vec* if_branch_vec_t;

/* series of sub structures for various statements */
/* $BBeF~J8(B */
class StmtAssignment {
 public:
  StmtAssignment(expr_t t, expr_t v)
      : target(t), val(v){
  }
  expr_t target;
  expr_t val;

};


/* if$B$NCf$K8=$l$k0l$D$N;^(B

if x:
    S
elif y:
    T

$B$NCf$N(B x$B$H(BS$B$NAH$d(By$B$H(BT$B$NAH!%(B

else:
    U

$B$O!$(Be == 0$B$H$7$F$"$i$o$9(B
 */
class IfBranch {
 public:
  IfBranch(expr_t expr, stmt_vec_t statements)
      : e(expr), s(statements), is_else(false) {}
  IfBranch(stmt_vec_t statements)
      : e(NULL), s(statements), is_else(true) {}
  expr_t e;
  stmt_vec_t s;
  bool is_else;

  // void if_branch_vec_add(if_branch_vec_t v, if_branch_t b);
  //int if_branch_vec_size(if_branch_vec_t v);
  //if_branch_t if_branch_vec_get(if_branch_vec_t v, int i);
};

/* while$BJ8(B */
class StmtWhile { /* while e: b */
 public:
  expr_t e;
  stmt_vec_t b;
  StmtWhile(expr_t expr, stmt_vec_t block)
      : e(expr), b(block) {};
};

/* for$BJ8(B */
class StmtFor { /* for x in e: s */
 public:
  string_t x;
  expr_t e;
  stmt_vec_t b;			/* body */
  StmtFor(string_t xx, expr_t expr, stmt_vec_t bb)
      : x(xx), e(expr), b(bb) {};
};

/* def$BJ8(B */
class StmtFundef {
 public:
  string_t f;			/* f (p1, ..., pn) b */
  str_vec_t ps;
  stmt_vec_t b;
  StmtFundef(string_t ff, str_vec_t pss, stmt_vec_t bb)
      : f(ff), ps(pss), b(bb) {};
};



/* definition of statements */
class Stmt {
 public:
  stmt_kind_t kind;
  src_pos_t pos;
  union {
    expr_t e; /* expression, return, print statement, del */
    stmt_assignment_t a;
    if_branch_vec_t i;
    stmt_while_t w;
    stmt_for_t f;
    stmt_fundef_t d;
    string_t g;			/* global x */
  } u;

  Stmt(stmt_kind_t k, src_pos_t p)
      : kind(k), pos(p) {}

  template <stmt_kind_t T>
  static stmt_t MakeStatement(expr_t e, src_pos_t p) {
    Stmt* s = new Stmt(T, p);
    s->u.e = e;
    return s;
  }

  static stmt_t MakeAssingment(expr_t t, expr_t v,
                                    src_pos_t p) {
    Stmt* s = new Stmt(stmt_kind_assignment, p);
    s->u.a = new StmtAssignment(t, v);
    return s;
  }

  static stmt_t MakeExpression(expr_t e, src_pos_t p) {
    return MakeStatement<stmt_kind_expression>(e, p);
  }

  static stmt_t MakeDel(expr_t e, src_pos_t p) {
    return MakeStatement<stmt_kind_del>(e, p);
  }

  // e might be NULL
  static stmt_t MakeReturn(expr_t e, src_pos_t p) {
    return MakeStatement<stmt_kind_return>(e, p);
  }

  static stmt_t MakePrint(expr_t e, src_pos_t p) {
    return MakeStatement<stmt_kind_print>(e, p);
  }

  template <stmt_kind_t T>
  static stmt_t MakeStatement(src_pos_t p) {
    Stmt* s = new Stmt(T, p);
    return s;
  }

  static stmt_t MakePass(src_pos_t p) {
    return MakeStatement<stmt_kind_pass>(p);
  }

  static stmt_t MakeBreak(src_pos_t p) {
    return MakeStatement<stmt_kind_break>(p);
  }

  static stmt_t MakeContinue(src_pos_t p) {
    return MakeStatement<stmt_kind_continue>(p);
  }

  static stmt_t MakeGlobal(string_t g, src_pos_t p) {
    Stmt* s = new Stmt(stmt_kind_global, p);
    s->u.g = g;
    return s;
  }

  static stmt_t MakeIf(if_branch_vec_t branches, src_pos_t p) {
    Stmt* s = new Stmt(stmt_kind_if, p);
    s->u.i = branches;
    return s;
  }

  static stmt_t MakeWhile(expr_t e, stmt_vec_t b,
                               src_pos_t p) {
    Stmt* s = new Stmt(stmt_kind_while, p);
    s->u.w = new StmtWhile(e, b);
    return s;
  }

  static stmt_t MakeFor(string_t x, expr_t e, stmt_vec_t b,
                             src_pos_t p) {
    Stmt* s = new Stmt(stmt_kind_for, p);
    s->u.f = new StmtFor(x, e, b);
    return s;
  }

  static stmt_t MakeFunDef(string_t f, str_vec_t ps,
                                stmt_vec_t b, src_pos_t p) {
    Stmt* s = new Stmt(stmt_kind_fundef, p);
    s->u.d = new StmtFundef(f, ps, b);
    return s;
  }
};

typedef stmt_vec_t file_input_t;

inline string bomb() {
  return "BOMB!";
}

class Printer {
 public:
  Printer(ostream& _f = std::cout)
      : f(_f) {
  }

  void PrintFileInput(file_input_t u) {
    PrintStmtVec(u, 0);
    f << "\n";
  }

 private:
  ostream& f;

  static string OperatorFunNameDebug(token_kind_t k, fix_kind_t fix) {
    switch (k) {
      case TOK_KW_NOT: return "not_";
      case TOK_KW_IS: return "bi_is_";
      case TOK_KW_IN: return "contains";
      case TOK_EQ_EQ: return "eq";
      case TOK_NEQ: return "ne";
      case TOK_GT: return "gt";
      case TOK_GEQ: return "ge";
      case TOK_LT: return "lt";
      case TOK_LEQ: return "le";
      case TOK_PLUS:
        if (fix == fix_kind_prefix) return "prefix_add";
        else return "add";
      case TOK_MINUS:
        if (fix == fix_kind_prefix) return "prefix_sub";
        else return "sub";
      case TOK_MUL: return "mul";
      case TOK_DIV: return "div";
      case TOK_MOD: return "mod";
      case TOK_TILDE: return "invert";
      case TOK_LSHIFT: return "lshift";
      case TOK_RSHIFT: return "rshift";
      case TOK_XOR: return "xor";
      case TOK_AMP: return "and_";
      case TOK_BAR: return "or_";
      case TOK_KW_IS_NOT: return "is_not_";
      case TOK_KW_NOT_IN: return "not_in__";
      default:
        return bomb();
    }
  }

  static string OperatorFunName(token_kind_t k, fix_kind_t fix) {
    switch (k) {
      case TOK_KW_NOT: return "not";
      case TOK_KW_IS: return "is";
      case TOK_KW_IN: return "in";
      case TOK_KW_OR: return "or";
      case TOK_KW_AND: return "and";
      case TOK_EQ_EQ: return "==";
      case TOK_EQ: return "=";
      case TOK_NEQ: return "!=";
      case TOK_GT: return ">";
      case TOK_GEQ: return ">=";
      case TOK_LT: return "<";
      case TOK_LEQ: return "<=";
      case TOK_PLUS:
        if (fix == fix_kind_prefix) return "+";
        else return "+";
      case TOK_MINUS:
        if (fix == fix_kind_prefix) return "-";
        else return "-";
      case TOK_MUL: return "*";
      case TOK_DIV: return "/";
      case TOK_MOD: return "%";
      case TOK_TILDE: return "~";
      case TOK_LSHIFT: return "<<";
      case TOK_RSHIFT: return ">>";
      case TOK_XOR: return "^";
      case TOK_AMP: return "&";
      case TOK_BAR: return "|";
      case TOK_KW_IS_NOT: return "is not";
      case TOK_KW_NOT_IN: return "not in";
      default:
        return bomb();
    }
  }

  void PrintExpr(expr_t e) {
    switch(e->kind) {
      case expr_kind_var:
        f << e->u.var->c_str();
        break;
      case expr_kind_literal_int:
        f << e->u.lit_i;
        break;
      case expr_kind_literal_float:
        f << e->u.lit_f;
        break;
      case expr_kind_literal_string:
        f << '"';
        FOREACH(it, *e->u.lit_s) {
          switch (*it) {
            case '\n':
              f << "\\n"; break;
            case '\r':
              f << "\\r"; break;
            case '\\':
              f << "\\\\"; break;
            case '"':
              f << "\\\""; break;
            default:
              f << *it; break;
          }
        }
        f << '"';
        break;
      case expr_kind_none:
        f << "None";
        break;
      case expr_kind_paren:
        f << "("; PrintExpr(e->u.paren); f << ")";
        break;
      case expr_kind_display_tuple:
        f << "( "; PrintExprVecTuple(e->u.disp); f << " )";
        break;
      case expr_kind_display_list:
        f << "[ "; PrintExprVecList(e->u.disp); f << " ]";
        break;
      case expr_kind_display_dict:
        f << "{ "; PrintExprVecDict(e->u.disp); f << " }";
        break;
      case expr_kind_attref:
        PrintExpr(e->u.atr->a); f << "."; f << e->u.atr->f;
        break;
      case expr_kind_operator:
        PrintExprOperator(e);
        break;
      case expr_kind_subscript:
        PrintExpr((*e->u.sub)[0]);
        f << "["; PrintExpr((*e->u.sub)[1]); f << "]";
        break;
      case expr_kind_call:
        PrintExpr(e->u.call->f); f << "(";
        PrintExprVecList(e->u.call->args); f << ")";
        break;
      default:
        bomb();
        break;
    }
  }

  void PrintExprOperator(expr_t e) {
    switch (e->u.op->fix) {
      case fix_kind_prefix:
        f << OperatorFunName(e->u.op->kind, fix_kind_prefix).c_str();
        f << " ";
        PrintExpr((*e->u.op->args)[0]);
        break;
      case fix_kind_infix:
        PrintExpr((*e->u.op->args)[0]);
        f << " ";
        f << OperatorFunName(e->u.op->kind, fix_kind_infix).c_str();
        f << " ";
        PrintExpr((*e->u.op->args)[1]);
        break;
      default:
        bomb();
    }
  }

  void PrintExprVecList(expr_vec_t v) {
    const int n = v->size();
    for (int i = 0; i < n; ++i) {
      if (i > 0) f << ", ";
      PrintExpr((*v)[i]);
    }
  }

  void PrintExprVecTuple(expr_vec_t v) {
    PrintExprVecList(v);
    if (v->size() == 1) f << ", ";
  }

  void PrintExprVecDict(expr_vec_t v) {
    const int n = v->size();
    for (int i = 0; i < n; i += 2) {
      if (i > 0) f << ", ";
      PrintExpr((*v)[i]);
      f << " : ";
      PrintExpr((*v)[i + 1]);
    }
  }

  void PrintStmt(stmt_t s, int indent) {
    PrintIndent(indent);
    switch (s->kind) {
      case stmt_kind_expression:
        PrintExpr(s->u.e);
        f << "\n";
        break;
      case stmt_kind_assignment:
        PrintExpr(s->u.a->target);
        f << " = ";
        PrintExpr(s->u.a->val);
        f << "\n";
        break;
      case stmt_kind_pass:
        f << "pass\n";
        break;
      case stmt_kind_return:
        f << "return";
        if (s->u.e) {
          f << " "; PrintExpr(s->u.e);
        }
        f << "\n";
        break;
      case stmt_kind_break:
        f << "break\n";
        break;
      case stmt_kind_continue:
        f << "continue\n";
        break;
      case stmt_kind_del:
        f << "del "; PrintExpr(s->u.e); f << "\n";
        break;
      case stmt_kind_print:
        f << "print "; PrintExpr(s->u.e); f << "\n";
        break;
      case stmt_kind_global:
        f << "global "; f << s->u.g; f << "\n";
        break;
      case stmt_kind_if:
        {
          if_branch_vec_t brs = s->u.i;
          int n = brs->size();
          int i;
          for (i = 0; i < n; i++) {
            if_branch_t br = (*brs)[i];
            if (i == 0) {
              f << "if ";
              PrintExpr(br->e);
            } else if (br->e) {
              PrintIndent(indent);
              f << "elif ";
              PrintExpr(br->e);
            } else {
              PrintIndent(indent);
              f << "else";
            }
            f << ":\n";
            PrintStmtVec(br->s, indent + 1);
          }
        }
        break;
      case stmt_kind_while:
        f << "while ";
        PrintExpr(s->u.w->e); f << ":\n";
        PrintStmtVec(s->u.w->b, indent + 1);
        break;
      case stmt_kind_for:
        f << "for "; f << s->u.f->x->c_str(); f << " in ";
        PrintExpr(s->u.f->e); f << ":\n";
        PrintStmtVec(s->u.f->b, indent + 1);
        break;
      case stmt_kind_fundef:
        f << "def "; f << s->u.d->f->c_str(); f << "(";
        PrintStrVec(s->u.d->ps); f << "):\n";
        PrintStmtVec(s->u.d->b, indent + 1);
        break;
      default:
        bomb();
    }
  }

  void PrintIndent(int indent) {
    for (int i = 0; i < indent; ++i) {
      f << "    ";
    }
  }

  void PrintStmtVec(stmt_vec_t sv, int indent) {
    FOREACH(it, *sv) {
      PrintStmt(*it, indent);
    }
  }

  void PrintStrVec(str_vec_t sv) {
    const int n = sv->size();
    for (int i = 0; i < n; ++i) {
      if (i > 0) f << ", ";
      f << (*sv)[i]->c_str();
    }
  }

};

#endif  // MINIPY_SYNTREE_H__
