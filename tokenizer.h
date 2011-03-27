#ifndef MINIPY_TOKENIZER_H__
#define MINIPY_TOKENIZER_H__

// #include "int_stack.h"
// #include "char_buffer.h"
#include "char_stream.h"

#include <cctype>
#include <cstdlib>
#include <stdio.h>  // For EOF
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <stack>
#include <queue>

using namespace std;

typedef enum {

  // 2.1 Ooperators
  TOK_KW_OR, // or
  TOK_KW_AND,// and
  TOK_KW_NOT,// not
  TOK_KW_IS, // is
  TOK_KW_IN, // in
  TOK_EQ,    // =
  TOK_EQ_EQ, // ==
  TOK_NEQ,   // !=
  TOK_GT,    // >
  TOK_GEQ,   // >=
  TOK_LT,    // <
  TOK_LEQ,   // <=
  TOK_PLUS,  // +
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

  // 2.2 None
  TOK_NONE,

  // 2.3 Tokens for statements.
  TOK_KW_BREAK,
  TOK_KW_CONTINUE,
  TOK_KW_PASS,
  TOK_KW_RETURN,
  TOK_KW_DEL,
  TOK_KW_PRINT,
  TOK_KW_GLOBAL,
  TOK_KW_IF,
  TOK_KW_ELIF,
  TOK_KW_ELSE,
  TOK_KW_FOR,
  TOK_KW_WHILE,
  TOK_KW_DEF,

  // 2.4 Other Symbols
  TOK_LPAREN,  // (
  TOK_RPAREN,  // )
  TOK_LBRACE,  // {
  TOK_RBRACE,  // }
  TOK_LBRACKET,// [
  TOK_RBRACKET,// ]
  TOK_PERIOD,  // ,
  TOK_COMMA,   // .
  TOK_COLON,   // :

  // 2.5 Literals
  TOK_LITERAL_INT,
  TOK_LITERAL_FLOAT,
  TOK_LITERAL_STRING,

  // 2.6 Identifiers
  TOK_ID,

  // 2.7 End of file
  TOK_EOF,

  // 2.8 Newline
  TOK_NEWLINE,

  // 2.9 Indent/dedent
  TOK_INDENT,
  TOK_DEDENT,

  // 構文木を作るために, 便宜的に導入
  TOK_KW_IS_NOT,
  TOK_KW_NOT_IN,

  //TOK_UNRECOGNIZED, // Lexical error which orrures when token can't be read.
} token_kind_t;

class Token {
 public:
  token_kind_t kind;

  string string_value;
  int int_value;
  double float_value;
};

string TokenKindAsString(token_kind_t);

typedef map<string, token_kind_t> keyword_map_t;
const keyword_map_t& TokenKeywordMap();

class Tokenizer;
typedef Tokenizer* tokenizer_t;

class Tokenizer {
 public:
  // Takes owership.
  Tokenizer(CharStream* char_stream);
  virtual ~Tokenizer();
  token_kind_t CurrentTokenKind() {
    return token_.kind;
  }
  // Value of id or literal
  string CurrentTokenStringValue() {
    return token_.string_value;
  }
  int CurrentTokenIntValue() {
    return token_.int_value;
  }
  double CurrentTokenFloatValue() {
    return token_.float_value;
  }

  int NextToken();
  int NextTokenDebug() {
    int ret = NextToken();
    cout << "'" << CurrentLineString() << "'"
         << " (" << CurrentLineNo() << "," << CurrentColumnNo() << ")"
         << " " << CurrentTokenInfo() << endl;
    return ret;
  }

  int CurrentLineNo() {
    return cs_->CurrentLine();
  };

  int CurrentColumnNo() {
    return cs_->CurrentColumn();
  }

  string CurrentLineString() {
    return cs_->CurrentLineString();
  }

  string CurrentTokenKindString() {
    return TokenKindAsString(token_.kind);
  }

  string CurrentTokenInfo() {
    ostringstream s;
    s << CurrentTokenKindString();
    if (token_.kind == TOK_ID) {
      s << " (" << token_.string_value << ")";
    } else if (token_.kind == TOK_LITERAL_INT) {
      s << " (" << token_.string_value << ")";
    } else if (token_.kind == TOK_LITERAL_FLOAT) {
      s << " (" << token_.string_value << ")";
    } else if (token_.kind == TOK_LITERAL_STRING) {
      s << " (" << token_.string_value << ")";
    }
    return s.str();
  }

 private:
  char Char() {
    return cs_->CurrentChar();
  }

  void NextChar() {
    token_buffer_ << Char();
    cs_->NextChar();
  }

  bool IsEOF() {
    return Char() == EOF;
  }

  bool IsNewLine() {
    return Char() == '\n';
  }

  bool IsEndOfLine() {
    return IsEOF() || IsNewLine();
  }

  void ParseError(string message) {
    cerr << message << endl;
    cerr << CurrentLineString() << " - "
         << " (" << CurrentLineNo() << ","
         << CurrentColumnNo() << ")" << endl;
    exit(1);
  }

  void SkipWhiteSpaces();
  int DoIndentDedent();

  int ParseNumberLiteral();
  int ParseFloatLiteral();
  int ParseStringLiteral();
  int ParseIdentifier();


  char_stream_t cs_;
  ostringstream token_buffer_;
  stack<int> indents_;
  int leading_spaces_;
  Token token_;
  queue<Token> next_tokens_;

  friend class TokenizerTest;

  //DISALLOW_COPY_AND_ASSIGN(Tokenizer:Tokenizer);
};

#endif // MINIPY_TOKENIZER_H__
