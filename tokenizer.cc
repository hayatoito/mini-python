#include "tokenizer.h"

using namespace std;

const keyword_map_t& TokenKeywordMap() {
  static keyword_map_t kw;
  static bool initialized = false;
  // Todo: Lock
  if (!initialized) {
    kw["or"] = TOK_KW_OR;
    kw["and"] = TOK_KW_AND;
    kw["not"] = TOK_KW_NOT;
    kw["is"] = TOK_KW_IS;
    kw["in"] = TOK_KW_IN;

    kw["None"] = TOK_NONE;
    kw["break"] = TOK_KW_BREAK;
    kw["continue"] = TOK_KW_CONTINUE;
    kw["pass"] = TOK_KW_PASS;
    kw["return"] = TOK_KW_RETURN;
    kw["del"] = TOK_KW_DEL;
    kw["print"] = TOK_KW_PRINT;
    kw["global"] = TOK_KW_GLOBAL;
    kw["if"] = TOK_KW_IF;
    kw["elif"] = TOK_KW_ELIF;
    kw["else"] = TOK_KW_ELSE;
    kw["for"] = TOK_KW_FOR;
    kw["while"] = TOK_KW_WHILE;
    kw["def"] = TOK_KW_DEF;
    initialized = true;
  }
  return kw;
}

string TokenKindAsString(token_kind_t token_kind) {
  switch (token_kind) {
    case TOK_RPAREN: return "TOK_RPAREN";
    case TOK_LPAREN: return "TOK_LPAREN";
    case TOK_LBRACE: return "TOK_LBRACE";
    case TOK_RBRACE: return "TOK_RBRACE";
    case TOK_RBRACKET: return "TOK_RBRACKET";
    case TOK_LBRACKET: return "TOK_LBRACKET";
    case TOK_PERIOD: return "TOK_PERIOD";
    case TOK_COMMA: return "TOK_COMMA";
    case TOK_COLON: return "TOK_COLON";
    case TOK_EQ_EQ: return "TOK_EQ_EQ";
    case TOK_EQ: return "TOK_EQ";
    case TOK_NEQ: return "TOK_NEQ";
    case TOK_GEQ: return "TOK_GEQ";
    case TOK_RSHIFT: return "TOK_RSHIFT";
    case TOK_GT: return "TOK_GT";
    case TOK_LEQ: return "TOK_LEQ";
    case TOK_LSHIFT: return "TOK_LSHIFT";
    case TOK_LT: return "TOK_LT";
    case TOK_MUL: return "TOK_MUL";
    case TOK_DIV: return "TOK_DIV";
    case TOK_MOD: return "TOK_MOD";
    case TOK_TILDE: return "TOK_TILDE";
    case TOK_AMP: return "TOK_AMP";
    case TOK_PLUS: return "TOK_PLUS";
    case TOK_MINUS: return "TOK_MINUS";
    case TOK_BAR: return "TOK_BAR";
    case TOK_XOR: return "TOK_XOR";
    case TOK_KW_OR: return "TOK_KW_OR";
    case TOK_KW_AND: return "TOK_KW_AND";
    case TOK_KW_NOT: return "TOK_KW_NOT";
    case TOK_KW_IS: return "TOK_KW_IS";
    case TOK_KW_IN: return "TOK_KW_IN";
    case TOK_KW_BREAK: return "TOK_KW_BREAK";
    case TOK_KW_CONTINUE: return "TOK_KW_CONTINUE";
    case TOK_KW_PASS: return "TOK_KW_PASS";
    case TOK_KW_RETURN: return "TOK_KW_RETURN";
    case TOK_KW_DEL: return "TOK_KW_DEL";
    case TOK_KW_PRINT: return "TOK_KW_PRINT";
    case TOK_KW_GLOBAL: return "TOK_KW_GLOBAL";
    case TOK_KW_IF: return "TOK_KW_IF";
    case TOK_KW_ELIF: return "TOK_KW_ELIF";
    case TOK_KW_ELSE: return "TOK_KW_ELSE";
    case TOK_KW_FOR: return "TOK_KW_FOR";
    case TOK_KW_WHILE: return "TOK_KW_WHILE";
    case TOK_KW_DEF: return "TOK_KW_DEF";
    case TOK_LITERAL_INT: return "TOK_LITERAL_INT";
    case TOK_LITERAL_FLOAT: return "TOK_LITERAL_FLOAT";
    case TOK_LITERAL_STRING: return "TOK_LITERAL_STRING";
    case TOK_ID: return "TOK_ID";
    case TOK_NEWLINE: return "TOK_NEWLINE";
    case TOK_INDENT: return "TOK_INDENT";
    case TOK_DEDENT: return "TOK_DEDENT";
    case TOK_NONE: return "TOK_NONE";
    case TOK_EOF: return "TOK_EOF";
      // case TOK_UNRECOGNIZED: return "TOK_UNRECOGNIZED";
    default: return "Undefined";
  }
}


Tokenizer::Tokenizer(char_stream_t char_stream)
    : cs_(char_stream) {

  // For combinience
  token_.kind = TOK_NEWLINE;

  // We should be in state of NextChar.
  NextChar();
  //indents_.Push(0);
  indents_.push(0);
}

Tokenizer::~Tokenizer() {
  delete cs_;
}

void Tokenizer::SkipWhiteSpaces() {
  // Todo: consider [Tab]

  if (cs_->CurrentColumn() == 1) {
    // '@     a = 2'
    // In leading spaces
    leading_spaces_ = 0; // >0 means that we are in leading space.
    while (Char() == ' ') {
      NextChar();
      ++leading_spaces_;
    }
    if (IsNewLine()) {
        // Empty Line. Continue scanning.
        NextChar();
        SkipWhiteSpaces();
    } else if (Char() == '#') {
        // Comment line. Read until Newline.
        while (!IsEndOfLine()) {
          NextChar();
        }
        if (IsNewLine()) {
          NextChar();
          SkipWhiteSpaces();
        } else {
          // EOF
          // do nothing
        }
    } else {
      // do nothing
    }
  } else {
    leading_spaces_ = -1;
    //
    // '  a = @  2'
    while (Char() == ' ') {
      NextChar();
    }
    if (Char() == '#') {
      // '  a = 2 @ # comment'
      while (!IsEndOfLine()) {
        NextChar();
      }
      // We should emit TOK_NEWLINE. So don't continue to next line.
    } else {
      // do nothing
    }
  }
}

int Tokenizer::DoIndentDedent() {
  //CHECK(!indents_.empty())
  if (leading_spaces_ > indents_.top()) {
    indents_.push(leading_spaces_);
    token_.kind = TOK_INDENT;
    return 1;
  } else if (leading_spaces_ < indents_.top()) {
    // if 1:
    //   foo a in ab:
    //     print a
    // @ab
    while (leading_spaces_ < indents_.top()) {
      Token dedent;
      dedent.kind = TOK_DEDENT;
      next_tokens_.push(dedent);
      indents_.pop();
    }
    if (leading_spaces_ > indents_.top()) {
      // TODO
      ParseError("Invalid indent");
    }
    next_tokens_.pop();
    token_.kind = TOK_DEDENT;
    return 1;
  }
  return 2;
}

int Tokenizer::NextToken() {
  if (!next_tokens_.empty()) {
    token_ = next_tokens_.front();
    next_tokens_.pop();
    return 1;
  }

  // Clears
  //token_buffer_.str("");

  SkipWhiteSpaces();

  token_buffer_.str("");

  if (IsEOF()) {
    // Emits TOK_DEDENT
    if (indents_.size() == 1) {
      token_.kind = TOK_EOF;
      return 0;
    } else {
      token_.kind = TOK_DEDENT;
      indents_.pop();
      return 1;
    }
  }

  if (leading_spaces_ >= 0) {
    int res = DoIndentDedent();
    if (res == 1) {
      return 1;
    } else if (res == -1) {
      // error
      return 0;
    } else {
      // do nothing. Continues scanning.
    }
  }

  switch (Char()) {
    case '\n':
      token_.kind = TOK_NEWLINE;
      NextChar(); break;
    case '(':
      token_.kind = TOK_LPAREN;
      NextChar(); break;
    case ')':
      token_.kind = TOK_RPAREN;
      NextChar(); break;
    case '{':
      token_.kind = TOK_LBRACE;
      NextChar(); break;
    case '}':
      token_.kind = TOK_RBRACE;
      NextChar(); break;
    case '[':
      token_.kind = TOK_LBRACKET;
      NextChar(); break;
    case ']':
      token_.kind = TOK_RBRACKET;
      NextChar(); break;
    case '.':
      NextChar();
      if (!isdigit(Char())) {
        token_.kind = TOK_PERIOD;
      } else {
        ParseFloatLiteral();
      }
      break;
    case ',':
      token_.kind = TOK_COMMA;
      NextChar(); break;
    case '!':
      // !=
      NextChar();
      if (Char() == '=') {
        token_.kind = TOK_NEQ;
        NextChar();
      } else {
        ParseError("wrong token !");
      }
      break;
    case '@':
      ParseError("wrong token @");
    case '#':
      ParseError("wrong token #");
    case '$':
      ParseError("wrong token $");
    case '%':
      token_.kind = TOK_MOD;
      NextChar(); break;
    case '^':
      token_.kind = TOK_XOR;
      NextChar(); break;
    case '&':
      token_.kind = TOK_AMP;
      NextChar(); break;
    case '*':
      // Todo: ** is allowed?
      token_.kind = TOK_MUL;
      NextChar(); break;
    case '-':
      token_.kind = TOK_MINUS;
      NextChar(); break;
    case '_':
      ParseIdentifier();
      break;
    case '=':
      // = | ==
      NextChar();
      if (Char() == '=') {
        token_.kind = TOK_EQ_EQ;
        NextChar();
      } else {
        token_.kind = TOK_EQ;
      }
      break;
    case '+':
      token_.kind = TOK_PLUS;
      NextChar(); break;
    case '\\':
      // Todo: Supports escape.
      ParseError("wrong token \\");
    case '|':
      token_.kind = TOK_BAR;
      NextChar(); break;
    case ';':
      ParseError("wrong token ;");
      break;
    case ':':
      token_.kind = TOK_COLON;
      NextChar(); break;
    case '\'':
      ParseStringLiteral();
      break;
    case '"':
      ParseStringLiteral();
      break;
    case '<':
      // < | << | <=
      NextChar();
      if (Char() == '<') {
        token_.kind = TOK_LSHIFT;
        NextChar();
      } else if (Char() == '=') {
        token_.kind = TOK_LEQ;
        NextChar();
      } else {
        token_.kind = TOK_LT;
      }
      break;
    case '>':
      // > | >> | >=
      NextChar();
      if (Char() == '>') {
        token_.kind = TOK_RSHIFT;
        NextChar();
      } else if (Char() == '=') {
        token_.kind = TOK_GEQ;
        NextChar();
      } else {
        token_.kind = TOK_GT;
      }
      break;
    case '/':
      // TODO: Supports '//' - True Division
      token_.kind = TOK_DIV;
      NextChar(); break;
    case '?':
      ParseError("wrong token ?"); break;
    case '~':
      token_.kind = TOK_TILDE;
      NextChar(); break;
    default:
      if (isdigit(Char())) {
        ParseNumberLiteral();
      } else if (isalpha(Char())) {
        ParseIdentifier();
      } else {
        cerr << "Invalid Char: " << Char() << endl;
        ParseError("wrong token. invalid character");
      }
  }
  return 1;
}

int Tokenizer::ParseNumberLiteral() {
  /*
  integer = 0 | non_zero_digit digit*
  float = digit* [. digit* ]
  non_zero_digit = (1|...|9)
  digit = (0|1|...|9)
  */
  while (isdigit(Char())) {
    //token_buffer_ << Char();
    NextChar();
  }
  if (Char() == '.') {
    // float
    NextChar();
    return ParseFloatLiteral();
  } else {
    if (token_buffer_.str().size() > 1 && token_buffer_.str()[0] == '0') {
      ParseError("Invalid int literal");
    }
    token_.kind = TOK_LITERAL_INT;
    token_.string_value = token_buffer_.str();
    token_.int_value = atoi(token_buffer_.str().c_str());
  }
  return 1;
}

int Tokenizer::ParseFloatLiteral() {
  while (isdigit(Char())) {
    NextChar();
  }
  token_.kind = TOK_LITERAL_FLOAT;
  token_.string_value = token_buffer_.str();
  token_.float_value = atof(token_buffer_.str().c_str());
  return 1;
}

int Tokenizer::ParseStringLiteral() {
  char start = Char();
  NextChar();

  bool backslash = false;
  stringstream literal;
  while (Char() != start || backslash) { // Todo: More checks (Newline / backslash)
    if (backslash) {
      // if (Char() == 'n') {
      //   literal << '\n';
      // } else {
      //   literal << Char();
      // }
      backslash = false;
    } else {
      if (Char() == '\\') {
        backslash = true;
      } else {
        //literal << Char();
      }
    }
    NextChar();
    if (IsEndOfLine()) {
      ParseError("wrong string literal. string literal encontered NewLine");
    }
  }
  NextChar();
  token_.kind = TOK_LITERAL_STRING;
  //token_.string_value = literal.str();
  token_.string_value = token_buffer_.str();
  return 1;
}

int Tokenizer::ParseIdentifier() {
  ostringstream chars;
  while (isalnum(Char()) || Char() == '_') {
    chars << Char();
    NextChar();
  }
  // Todo: Check Keywords
  const keyword_map_t& kw = TokenKeywordMap();
  keyword_map_t::const_iterator found = kw.find(chars.str());
  if (found != kw.end()) {
    token_.kind = found->second;
  } else {
    token_.kind = TOK_ID;
    token_.string_value = chars.str();
  }
  return 1;
}
