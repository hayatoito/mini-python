#ifndef MINIPY_PARSER_H__
#define MINIPY_PARSER_H__

#include "syntree.h"

#include "char_stream.h"

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <stack>
#include <queue>

using namespace std;

class Parser;
typedef Parser* parser_t;

class Parser {
 public:
  Parser(tokenizer_t tz);

  //file_input ::=
  //         (NEWLINE | statement)* EOF
  file_input_t ParseFileInput();

 private:
  tokenizer_t tz_;

  token_kind_t CurrentTokenKind();
  void NextToken();
  bool IsCurrentTokenKind(token_kind_t k);
  void Eat(token_kind_t kind);
  void Error(const string& message);
  src_pos_t MakeSrcPos();
  bool CanBeExpression();

  // Pointer-to-member function.
  typedef expr_t (Parser::*ParseExpressionFunc)();

  /*
     expr ::=
     ParseF ( "KIND" ParseF )*
  */
  template <ParseExpressionFunc F, token_kind_t KIND>
  expr_t ParseRepeatExpression() {
    expr_t e0 = (this->*F)();
    token_kind_t k = CurrentTokenKind();
    while (k == KIND) {
      NextToken();
      expr_t e1 = (this->*F)();
      e0 = Expr::MakeInfixOperator(k, e0, e1, e0->pos);
      k = CurrentTokenKind();
    }
    return e0;
  }

  template <ParseExpressionFunc F, token_kind_t KIND1, token_kind_t KIND2>
  expr_t ParseRepeatExpression() {
    expr_t e0 = (this->*F)();
    token_kind_t k = CurrentTokenKind();
    while (k == KIND1 || k == KIND2) {
      NextToken();
      expr_t e1 = (this->*F)();
      e0 = Expr::MakeInfixOperator(k, e0, e1, e0->pos);
      k = CurrentTokenKind();
    }
    return e0;
  }

  template <ParseExpressionFunc F, token_kind_t KIND1, token_kind_t KIND2,
            token_kind_t KIND3>
  expr_t ParseRepeatExpression() {
    expr_t e0 = (this->*F)();
    token_kind_t k = CurrentTokenKind();
    while (k == KIND1 || k == KIND2 || k == KIND3) {
      NextToken();
      expr_t e1 = (this->*F)();
      e0 = Expr::MakeInfixOperator(k, e0, e1, e0->pos);
      k = CurrentTokenKind();
    }
    return e0;
  }

  /*
    expression ::=
    or_test
  */
  expr_t ParseExpression();

  /*
     or_test ::=
     and_test ( "or" and_test )*
  */
  expr_t ParseOrTest();

  /*
     and_test ::=
     not_test ( "and" not_test )*
  */
  expr_t ParseAndTest();

  /*
     not_test ::=
     comparison | "not" not_test
  */
  expr_t ParseNotTest();

  /*
     comparison ::=
     or_expr [ comp_operator or_expr ]
  */
  expr_t ParseComparison();

  /*
     or_expr ::=
     xor_expr ( "|" xor_expr )*
  */
  expr_t ParseOrExpr();

  //xor_expr ::=
  //             and_expr ( "^" and_expr )*
  expr_t ParseXorExpr();

  //and_expr ::=
  //         shift_expr ( "&" shift_expr )*
  expr_t ParseAndExpr();

  //shift_expr ::=
  //         a_expr ( ( "<<" | ">>" ) a_expr )*
  expr_t ParseShiftExpr();

  //a_expr ::=
  //         m_expr ( ( "+" | "-" ) m_expr )*
  expr_t ParseAExpr();

  //m_expr ::=
  //         u_expr ( ( "*" | "/" | "%" ) u_expr )*
  expr_t ParseMExpr();

  //u_expr ::=
  //         primary | "-" u_expr | "+" u_expr | "~" u_expr
  expr_t ParseUExpr();

  /*
# - atom.f は attref式. mini-Pythonではその直後に atom.f(x, y, z)
#   と続く場合のみが許されるが，
#   このことは以下の文法には現れていない(別途チェックする)．
#   そして，atom.f(x, y, z) は f(atom, x, y, z) と等価としている．
  */

  //primary ::=
  //         atom ( "." identifier
  //           | "[" expression_list_with_comma "]"
  //           | "(" expression_list_with_comma ")" )*
  expr_t ParsePrimary();

  expr_t ParsePrimaryAttribute(expr_t pre_expr);

  //atom ::=
  //         identifier | literal
  //       | list_display | dict_display
  //       | "(" expression_list_with_comma ")"
  expr_t ParseAtom();

  expr_t ParseIdentifier();

  expr_t ParseLiteral();

  //  list_display ::=
  //         "[" expression_list_with_comma "]"
  expr_t ParseListDisplay();

  //dict_display ::=
  //         "{" key_datum_list "}"
  expr_t ParseDictDisplay();

  class KeyDatum {
   public:
    expr_t key;
    expr_t datum;
  };

  //key_datum_list ::=
  //         [ key_datum ("," key_datum)* ]
  // Todo: Allow last comma.
  expr_vec_t ParseKeyDatumList();

  //key_datum ::=
  //         expression ":" expression
  const KeyDatum* ParseKeyDatum();

  class ExpressionListWithComma;

  //expression_list_with_comma ::=
  //         [ expression ( "," expression )* [","] ]
  const ExpressionListWithComma* ParseExpressionListWithComma();

  /*
     statement ::=
     expression_stmt NEWLINE
     | assignment_stmt NEWLINE
     | pass_stmt NEWLINE
     | return_stmt NEWLINE
     | del_stmt NEWLINE
     | break_stmt NEWLINE
     | continue_stmt NEWLINE
     | global_stmt NEWLINE
     | print_stmt NEWLINE
     | if_stmt
     | while_stmt
     | for_stmt
     | funcdef
  */
  stmt_t ParseStatement();

  //return_stmt ::=
  //         "return" [expression_list_with_comma]
  stmt_t ParseReturnStmt();

  //global_stmt ::=
  //         "global" identifier
  stmt_t ParseGlobalStmt();

  //print_stmt ::=
  //         "print" expression_list_with_comma
  stmt_t ParsePrintStmt();

  //del_stmt ::=
  //     "del" subscription
  stmt_t ParseDelStmt();

  //subscription ::=
  //      primary "[" expression_list_with_comma "]"
  expr_t ParseSubscription();

  //if_stmt ::=
  //         "if" expression ":" suite
  //            ( "elif" expression ":" suite )*
  //            ["else" ":" suite]
  stmt_t ParseIfStmt();

  //suite ::=
  //         NEWLINE INDENT statement+ DEDENT
  stmt_vec_t ParseSuite();

  // statement+
  stmt_vec_t ParseStatementList();

  /*
    while_stmt ::=
             "while" expression ":" NEWLINE INDENT statement+ DEDENT
  */
  stmt_t ParseWhileStmt();

  /*
     for_stmt ::=
             "for" identifier "in" expression ":" suite
  */
  stmt_t ParseForStmt();

  //parameter_list ::=
  //     [ identifier ("," identifier)* ]
  str_vec_t ParseParameterList();

  /*
     funcdef ::=
             "def" funcname "(" parameter_list ")"
              ":" suite
  */
  stmt_t ParseFuncDef();
};


#endif // MINIPY_PARSER_H__
