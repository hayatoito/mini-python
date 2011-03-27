#include "parser.h"

using namespace std;

Parser::Parser(tokenizer_t tz)
    : tz_(tz) {
  tz_->NextToken();
}

file_input_t Parser::ParseFileInput() {
  stmt_vec* slist = new stmt_vec;
  while (!IsCurrentTokenKind(TOK_EOF)) {
    if (IsCurrentTokenKind(TOK_NEWLINE)) {
      // Skips it. Never happens in current Tokenizer implementation.
      NextToken();
    } else {
      slist->push_back(ParseStatement());
    }
  }
  return slist;
}

token_kind_t Parser::CurrentTokenKind() {
  return tz_->CurrentTokenKind();
}

void Parser::NextToken() {
  tz_->NextToken();
}

bool Parser::IsCurrentTokenKind(token_kind_t k) {
  return CurrentTokenKind() == k;
}

void Parser::Eat(token_kind_t kind) {
  if (CurrentTokenKind() != kind) {
    stringstream s;
    s << "Cannot eat: " << TokenKindAsString(kind).c_str();
    Error(s.str());
  }
  NextToken();
}

void Parser::Error(const string& message) {
  cerr << "Parse error: " << message.c_str() << endl;
  cerr << tz_->CurrentLineString() << " - "
       << " (" << tz_->CurrentLineNo() << ","
       << tz_->CurrentColumnNo() << ")" << endl;
  cerr << tz_->CurrentTokenInfo() << endl;
  exit(1);
}

src_pos_t Parser::MakeSrcPos() {
  static const string* filename = new string("filename");
  return new SrcPos(filename, tz_->CurrentLineNo(), tz_->CurrentColumnNo());
}

bool Parser::CanBeExpression() {
  switch (CurrentTokenKind()) {
    // Todo: More tokens
    case TOK_ID:
    case TOK_LBRACE:
    case TOK_LBRACKET:
    case TOK_LPAREN:
    case TOK_LITERAL_INT:
    case TOK_LITERAL_FLOAT:
    case TOK_LITERAL_STRING:
    case TOK_PLUS:
    case TOK_MINUS:
      return true;
    default:
      return false;
  }
}

class Parser::ExpressionListWithComma {
 public:
  ExpressionListWithComma(expr_vec_t _ev, bool _last_comma)
      : ev(_ev), last_comma(_last_comma) {}
  expr_vec_t ev;
  const bool last_comma;

  bool IsTuple() const {
    return ev->size() > 1 || ev->size() == 1 && last_comma;
  }

  expr_t ToExpression() const {
    if (IsTuple()) {
      return Expr::MakeDisplayTuple(ev, (*ev)[0]->pos);
    } else if (ev->size() == 1) {
      return (*ev)[0];
    } else {
      return NULL;
    }
  }
};

/*
  expression ::=
  or_test
*/
expr_t Parser::ParseExpression() {
  return ParseOrTest();
}

/*
  or_test ::=
  and_test ( "or" and_test )*
*/
expr_t Parser::ParseOrTest() {
  //cout << "In ParseOrTest" << endl;
  return ParseRepeatExpression<&Parser::ParseAndTest, TOK_KW_OR>();
}

/*
  and_test ::=
  not_test ( "and" not_test )*
*/
expr_t Parser::ParseAndTest() {
  return ParseRepeatExpression<&Parser::ParseNotTest, TOK_KW_AND>();
}

/*
  not_test ::=
  comparison | "not" not_test
*/
expr_t Parser::ParseNotTest() {
  if (IsCurrentTokenKind(TOK_KW_NOT)) {
    NextToken();
    expr_t e0 = ParseNotTest();
    return Expr::MakePrefixOperator(TOK_KW_NOT, e0, e0->pos);
  } else {
    return ParseComparison();
  }
}

/*
  comparison ::=
  or_expr [ comp_operator or_expr ]
*/
expr_t Parser::ParseComparison() {
  expr_t e0 = ParseOrExpr();
  token_kind_t k = CurrentTokenKind();
  switch (k) {
    case TOK_LT:
    case TOK_GT:
    case TOK_EQ_EQ:
    case TOK_GEQ:
    case TOK_LEQ:
    case TOK_NEQ:
      NextToken();
      return Expr::MakeInfixOperator(k, e0, ParseOrExpr(), e0->pos);
    case TOK_KW_IS:
      NextToken();
      if (IsCurrentTokenKind(TOK_KW_NOT)) {
        NextToken();
        return Expr::MakeInfixOperator(TOK_KW_IS_NOT, e0, ParseOrExpr(), e0->pos);
      } else {
        return Expr::MakeInfixOperator(TOK_KW_IS, e0, ParseOrExpr(), e0->pos);
      }
    case TOK_KW_NOT:
      NextToken();
      if (IsCurrentTokenKind(TOK_KW_IN)) {
        NextToken();
        return Expr::MakeInfixOperator(TOK_KW_NOT_IN, e0, ParseOrExpr(), e0->pos);
      } else {
        return Expr::MakeInfixOperator(TOK_KW_NOT, e0, ParseOrExpr(), e0->pos);
      }
    default:
      return e0;
  }
}

/*
  or_expr ::=
  xor_expr ( "|" xor_expr )*
*/
expr_t Parser::ParseOrExpr() {
  return ParseRepeatExpression<&Parser::ParseXorExpr, TOK_BAR>();
}

//xor_expr ::=
//             and_expr ( "^" and_expr )*
expr_t Parser::ParseXorExpr() {
  return ParseRepeatExpression<&Parser::ParseAndExpr, TOK_XOR>();
}

//and_expr ::=
//         shift_expr ( "&" shift_expr )*
expr_t Parser::ParseAndExpr() {
  return ParseRepeatExpression<&Parser::ParseShiftExpr, TOK_AMP>();
}

//shift_expr ::=
//         a_expr ( ( "<<" | ">>" ) a_expr )*
expr_t Parser::ParseShiftExpr() {
  return ParseRepeatExpression<&Parser::ParseAExpr, TOK_LSHIFT, TOK_RSHIFT>();
}

//a_expr ::=
//         m_expr ( ( "+" | "-" ) m_expr )*
expr_t Parser::ParseAExpr() {
  return ParseRepeatExpression<&Parser::ParseMExpr, TOK_PLUS, TOK_MINUS>();
}

//m_expr ::=
//         u_expr ( ( "*" | "/" | "%" ) u_expr )*
expr_t Parser::ParseMExpr() {
  return ParseRepeatExpression<&Parser::ParseUExpr, TOK_MUL, TOK_DIV, TOK_MOD>();
}

//u_expr ::=
//         primary | "-" u_expr | "+" u_expr | "~" u_expr
expr_t Parser::ParseUExpr() {
  token_kind_t k = CurrentTokenKind();
  switch (k) {
    case TOK_MINUS:
    case TOK_PLUS:
    case TOK_TILDE: {
      NextToken();
      expr_t e = ParseUExpr();
      return Expr::MakePrefixOperator(k, e, e->pos);
    }
    default:
      return ParsePrimary();
  }
}

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
expr_t Parser::ParsePrimary() {
  return ParsePrimaryAttribute(ParseAtom());
}

expr_t Parser::ParsePrimaryAttribute(expr_t pre_expr) {
  switch (CurrentTokenKind()) {
    case TOK_PERIOD: {
      NextToken();
      expr_t attref = ParseIdentifier();

      // Assumes we don't allow x.f.   (attref).
      // So assumes x.f(1,2,..)

      // Now - pre_expr.attref
      // Going - pre_expr.attref(1, 2, 3)  (method call)
      //       - pre_expr.attref(1, 2, 3)(4, 5, 6)
      //       - pre_expr.attref(1, 2, 3).a(1,2)
      Eat(TOK_LPAREN);
      const ExpressionListWithComma* elist = ParseExpressionListWithComma();
      Eat(TOK_RPAREN);

      // pre_expr.attref(1,2,3) --> attref(pre_expr, 1, 2, 3)
      expr_vec* ps = new expr_vec;
      ps->push_back(pre_expr);
      const int n = elist->ev->size();
      for (int i = 0; i < n; ++i) {
        ps->push_back((*elist->ev)[i]);
      }
      return ParsePrimaryAttribute(Expr::MakeCall(attref, ps, pre_expr->pos));

      // x.f - Never happens in this course.
      //return Expr::MakeAttref(e0, attref->u.var, e0->pos);
    }
    case TOK_LBRACKET: {
      NextToken();
      const ExpressionListWithComma* elist = ParseExpressionListWithComma();
      Eat(TOK_RBRACKET);
      if (elist->IsTuple()) {
        return ParsePrimaryAttribute(
            Expr::MakeSubscript(pre_expr,
                                Expr::MakeDisplayTuple(elist->ev, pre_expr->pos),
                                pre_expr->pos));
      } else {
        if (elist->ev->size() == 1) {
          return ParsePrimaryAttribute(
              Expr::MakeSubscript(pre_expr, (*elist->ev)[0], pre_expr->pos));
        } else {
          Error("Subscrit with empty");
        }
      }
    }
    case TOK_LPAREN: {
      NextToken();
      const ExpressionListWithComma* elist = ParseExpressionListWithComma();
      Eat(TOK_RPAREN);
      return ParsePrimaryAttribute(
          Expr::MakeCall(pre_expr, elist->ev, pre_expr->pos));
    }
    default:
      return pre_expr;
  }
}

//atom ::=
//         identifier | literal
//       | list_display | dict_display
//       | "(" expression_list_with_comma ")"
expr_t Parser::ParseAtom() {
  switch (CurrentTokenKind()) {
    case TOK_ID:
      return ParseIdentifier();
    case TOK_LITERAL_INT:
    case TOK_LITERAL_FLOAT:
    case TOK_LITERAL_STRING:
      return ParseLiteral();
    case TOK_NONE:
      NextToken();
      return Expr::MakeNone(MakeSrcPos());
    case TOK_LBRACKET:
      return ParseListDisplay();
    case TOK_LBRACE:
      return ParseDictDisplay();
    case TOK_LPAREN: {
      NextToken();
      const ExpressionListWithComma* elist = ParseExpressionListWithComma();
      Eat(TOK_RPAREN);
      if (elist->IsTuple()) {
        return elist->ToExpression();
      } else {
        if (elist->ev->size() == 1) {
          return Expr::MakeParen(elist->ToExpression(), MakeSrcPos());
        } else {
          // Empty tuple
          return Expr::MakeDisplayTuple(elist->ev, MakeSrcPos());
        }
      }
    }
    default:
      Error("ParseAtom failed");
      exit(1);
  }
}

expr_t Parser::ParseIdentifier() {
  // Assumes TOK_ID
  assert(IsCurrentTokenKind(TOK_ID));
  expr_t e = Expr::MakeVar(new string(tz_->CurrentTokenStringValue()),
                           MakeSrcPos());
  NextToken();
  return e;
}

expr_t Parser::ParseLiteral() {
  switch (CurrentTokenKind()) {
    case TOK_LITERAL_INT: {
      expr_t e = Expr::MakeLiteralInt(tz_->CurrentTokenIntValue(),
                                      MakeSrcPos());
      NextToken();
      return e;
    }
    case TOK_LITERAL_FLOAT: {
      expr_t e = Expr::MakeLiteralFloat(tz_->CurrentTokenFloatValue(),
                                        MakeSrcPos());
      NextToken();
      return e;
    }
    case TOK_LITERAL_STRING: {
      // Eval literal string here.
      string literal = tz_->CurrentTokenStringValue();
      const int n = literal.size();
      ostringstream s;
      bool slash = false;
      for (int i = 1; i + 1 < n; ++i) {
        char c = literal[i];
        if (slash) {
          if (c == 'n') {
            s << '\n';
          } else if (c == 't') {
            s << '\t';
          } else {
            s << c;
          }
          slash = false;
        } else {
          if (c == '\\') {
            slash = true;
          } else {
            s << c;
          }
        }
      }

      expr_t e = Expr::MakeLiteralString(new string(s.str()), MakeSrcPos());
      NextToken();
      return e;
    }
    default:
      Error("ParseLiteral");
      exit(1);
  }
}

//  list_display ::=
//         "[" expression_list_with_comma "]"
expr_t Parser::ParseListDisplay() {
  Eat(TOK_LBRACKET);
  expr_vec_t ev = ParseExpressionListWithComma()->ev;
  Eat(TOK_RBRACKET);
  return Expr::MakeDisplayList(ev, MakeSrcPos());
}

//dict_display ::=
//         "{" key_datum_list "}"
expr_t Parser::ParseDictDisplay() {
  Eat(TOK_LBRACE);
  expr_vec_t ev = ParseKeyDatumList();
  Eat(TOK_RBRACE);
  return Expr::MakeDisplayDict(ev, MakeSrcPos());
}

//key_datum_list ::=
//         [ key_datum ("," key_datum)* ]
// Todo: Allow last comma.
expr_vec_t Parser::ParseKeyDatumList() {
  expr_vec* ev = new expr_vec;
  if (IsCurrentTokenKind(TOK_RBRACE)) {
    return ev;
  }
  const KeyDatum* key_datum = ParseKeyDatum();
  ev->push_back(key_datum->key);
  ev->push_back(key_datum->datum);
  while (IsCurrentTokenKind(TOK_COMMA)) {
    NextToken();
    key_datum = ParseKeyDatum();
    ev->push_back(key_datum->key);
    ev->push_back(key_datum->datum);
  }
  return ev;
}

//key_datum ::=
//         expression ":" expression
const Parser::KeyDatum* Parser::ParseKeyDatum() {
  KeyDatum* key_datum = new KeyDatum;
  key_datum->key = ParseExpression();
  Eat(TOK_COLON);
  key_datum->datum = ParseExpression();
  return key_datum;
}

//expression_list_with_comma ::=
//         [ expression ( "," expression )* [","] ]
const Parser::ExpressionListWithComma* Parser::ParseExpressionListWithComma() {
  expr_vec* ev = new expr_vec;
  if (!CanBeExpression()) {
    return new ExpressionListWithComma(ev, false);
  }
  ev->push_back(ParseExpression());
  while (IsCurrentTokenKind(TOK_COMMA)) {
    NextToken();
    if (!CanBeExpression()) {
      return new ExpressionListWithComma(ev, true);
    }
    ev->push_back(ParseExpression());
  }
  return new ExpressionListWithComma(ev, false);
}

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
stmt_t Parser::ParseStatement() {
  src_pos_t p = MakeSrcPos();
  switch (CurrentTokenKind()) {
    case TOK_KW_PASS: {
      NextToken();
      stmt_t stmt = Stmt::MakePass(p);
      Eat(TOK_NEWLINE);
      return stmt;
    }
    case TOK_KW_BREAK: {
      NextToken();
      stmt_t stmt = Stmt::MakeBreak(p);
      Eat(TOK_NEWLINE);
      return stmt;
    }
    case TOK_KW_CONTINUE: {
      NextToken();
      stmt_t stmt = Stmt::MakeContinue(p);
      Eat(TOK_NEWLINE);
      return stmt;
    }
    case TOK_KW_RETURN: {
      stmt_t stmt = ParseReturnStmt();
      Eat(TOK_NEWLINE);
      return stmt;
    }
    case TOK_KW_DEL: {
      stmt_t stmt = ParseDelStmt();
      Eat(TOK_NEWLINE);
      return stmt;
    }
    case TOK_KW_GLOBAL: {
      stmt_t stmt = ParseGlobalStmt();
      Eat(TOK_NEWLINE);
      return stmt;
    }
    case TOK_KW_PRINT: {
      stmt_t stmt = ParsePrintStmt();
      Eat(TOK_NEWLINE);
      return stmt;
    }
    case TOK_KW_IF:
      return ParseIfStmt();
    case TOK_KW_WHILE:
      return ParseWhileStmt();
    case TOK_KW_FOR:
      return ParseForStmt();
    case TOK_KW_DEF:
      return ParseFuncDef();
    default:
      expr_t e = ParseExpression();
      if (IsCurrentTokenKind(TOK_EQ)) {
        // assignment_stmt
        if (e->IsLValue()) {
          NextToken();
          expr_t v = ParseExpression();
          Eat(TOK_NEWLINE);
          return Stmt::MakeAssingment(e, v, e->pos);
        } else {
          Error("Not LValue");
          exit(1);
        }
      } else {
        // expression_stmt
        Eat(TOK_NEWLINE);
        return Stmt::MakeExpression(e, e->pos);
      }
  }
}

//return_stmt ::=
//         "return" [expression_list_with_comma]
stmt_t Parser::ParseReturnStmt() {
  Eat(TOK_KW_RETURN);
  const ExpressionListWithComma* elist = ParseExpressionListWithComma();
  return Stmt::MakeReturn(elist->ToExpression(), MakeSrcPos());
}

//global_stmt ::=
//         "global" identifier
stmt_t Parser::ParseGlobalStmt() {
  Eat(TOK_KW_GLOBAL);
  expr_t e = ParseIdentifier();
  return Stmt::MakeGlobal(e->u.var, e->pos);
}

//print_stmt ::=
//         "print" expression_list_with_comma
stmt_t Parser::ParsePrintStmt() {
  Eat(TOK_KW_PRINT);
  const ExpressionListWithComma* elist = ParseExpressionListWithComma();
  return Stmt::MakePrint(elist->ToExpression(), MakeSrcPos());
}

//del_stmt ::=
//     "del" subscription
stmt_t Parser::ParseDelStmt() {
  Eat(TOK_KW_DEL);
  expr_t e = ParseSubscription();
  return Stmt::MakeDel(e, e->pos);
}

//subscription ::=
//      primary "[" expression_list_with_comma "]"
expr_t Parser::ParseSubscription() {
  expr_t e = ParsePrimary();
  Eat(TOK_LBRACKET);
  const ExpressionListWithComma* elist = ParseExpressionListWithComma();
  Eat(TOK_RBRACKET);
  if (elist->ev->empty()) {
    Error("Subscription with empty");
  }
  return Expr::MakeSubscript(e, elist->ToExpression(), MakeSrcPos());
}

//if_stmt ::=
//         "if" expression ":" suite
//            ( "elif" expression ":" suite )*
//            ["else" ":" suite]
stmt_t Parser::ParseIfStmt() {
  Eat(TOK_KW_IF);
  expr_t e = ParseExpression();
  //cout << "In ParseIfStmt: " << tz_->CurrentTokenInfo() << endl;

  Eat(TOK_COLON);
  stmt_vec_t suite = ParseSuite();
  if_branch_vec* ifb_vec = new if_branch_vec;
  ifb_vec->push_back(new IfBranch(e, suite));
  while (IsCurrentTokenKind(TOK_KW_ELIF) ||
         IsCurrentTokenKind(TOK_KW_ELSE)) {
    if (IsCurrentTokenKind(TOK_KW_ELIF)) {
      NextToken();
      e = ParseExpression();
      Eat(TOK_COLON);
      suite = ParseSuite();
      ifb_vec->push_back(new IfBranch(e, suite));
    } else {
      NextToken();
      Eat(TOK_COLON);
      suite = ParseSuite();
      ifb_vec->push_back(new IfBranch(suite));
      break;
    }
  }
  return Stmt::MakeIf(ifb_vec, MakeSrcPos());
}

//suite ::=
//         NEWLINE INDENT statement+ DEDENT
stmt_vec_t Parser::ParseSuite() {
  Eat(TOK_NEWLINE);
  Eat(TOK_INDENT);
  stmt_vec_t slist = ParseStatementList();
  Eat(TOK_DEDENT);
  return slist;
}

// statement+
stmt_vec_t Parser::ParseStatementList() {
  stmt_vec* slist = new stmt_vec;
  slist->push_back(ParseStatement());
  while (!IsCurrentTokenKind(TOK_DEDENT)) {
    slist->push_back(ParseStatement());
  }
  return slist;
}

/*
  while_stmt ::=
  "while" expression ":" NEWLINE INDENT statement+ DEDENT
*/
stmt_t Parser::ParseWhileStmt() {
  Eat(TOK_KW_WHILE);
  expr_t e = ParseExpression();
  Eat(TOK_COLON);
  stmt_vec_t suite = ParseSuite();
  return Stmt::MakeWhile(e, suite, e->pos);
}

/*
  for_stmt ::=
  "for" identifier "in" expression ":" suite
*/
stmt_t Parser::ParseForStmt() {
  Eat(TOK_KW_FOR);
  expr_t id = ParseIdentifier();
  Eat(TOK_KW_IN);
  expr_t e = ParseExpression();
  Eat(TOK_COLON);
  stmt_vec_t suite = ParseSuite();
  return Stmt::MakeFor(id->u.var, e, suite, id->pos);
}

//parameter_list ::=
//     [ identifier ("," identifier)* ]
str_vec_t Parser::ParseParameterList() {
  str_vec* plist = new str_vec;
  if (IsCurrentTokenKind(TOK_ID)) {
    plist->push_back(ParseIdentifier()->u.var);
  }
  while (IsCurrentTokenKind(TOK_COMMA)) {
    NextToken();
    plist->push_back(ParseIdentifier()->u.var);
  }
  return plist;
}

/*
  funcdef ::=
  "def" funcname "(" parameter_list ")"
  ":" suite
*/
stmt_t Parser::ParseFuncDef() {
  Eat(TOK_KW_DEF);
  expr_t id = ParseIdentifier();
  Eat(TOK_LPAREN);
  str_vec_t parameter_list = ParseParameterList();
  Eat(TOK_RPAREN);
  Eat(TOK_COLON);
  stmt_vec_t suite = ParseSuite();
  return Stmt::MakeFunDef(id->u.var, parameter_list, suite, id->pos);
}
