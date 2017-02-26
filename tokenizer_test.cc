#include "tokenizer.h"
#include "char_stream.h"

#include <fstream>
#include <sstream>

#include <gtest/gtest.h>

using namespace std;

class TokenizerTest : public testing::Test {
 protected:
  virtual void SetUp() {
    tz_ = NULL;
  }

  virtual void TearDown() {
    delete tz_;
    tz_ = NULL;
  }

  void ResetStream(const string& contents) {
    delete tz_;
    tz_ = new Tokenizer(new CharStream(new istringstream(contents)));
  }

  void SkipWhiteSpaces() {
    tz_->SkipWhiteSpaces();
  }

  char Char() {
    return tz_->Char();
  }

  void NextChar() {
    tz_->NextChar();
  }

  void CheckTokens(const string& contents, const vector<token_kind_t>& expected) {
    ResetStream(contents);
    int i = 0;
    while (tz_->NextToken()) {
    // while (tz_->NextTokenDebug()) {
      ASSERT_LT(i, static_cast<int>(expected.size()))
          << "unexpected " << i << " Token "
          << tz_->CurrentTokenKind();
      EXPECT_EQ(expected[i], tz_->CurrentTokenKind())
          << TokenKindAsString(expected[i]) << " doesn't match with "
          << tz_->CurrentTokenInfo();
      ++i;
    }
    EXPECT_EQ(expected.size(), i);
  }

  tokenizer_t tz_;
};

namespace  {

TEST_F(TokenizerTest, SkipWhiteSpaces) {
  ResetStream("   a   \n");
  SkipWhiteSpaces();
  EXPECT_EQ('a', Char());

  NextChar();
  SkipWhiteSpaces();
  EXPECT_EQ('\n', Char());

  ResetStream(" a #   \n");
  SkipWhiteSpaces();
  EXPECT_EQ('a', Char());
  NextChar();
  SkipWhiteSpaces();
  EXPECT_EQ('\n', Char());

  ResetStream(" #   \n  a \n");
  SkipWhiteSpaces();
  EXPECT_EQ('a', Char());

  ResetStream(" #   \n #  a \n b");
  SkipWhiteSpaces();
  EXPECT_EQ('b', Char());
}

TEST_F(TokenizerTest, Tokens) {
  string contents = "(\n";
  vector<token_kind_t> tokens;
  tokens.push_back(TOK_LPAREN);
  tokens.push_back(TOK_NEWLINE);
  CheckTokens(contents, tokens);
}

TEST_F(TokenizerTest, IndentDedendent) {
  string contents =
      "(        \n"
      "  (      \n"
      "  (      \n"
      "    (    \n"
      "(        \n";

  vector<token_kind_t> tokens;

  tokens.push_back(TOK_LPAREN);
  tokens.push_back(TOK_NEWLINE);

  tokens.push_back(TOK_INDENT);
  tokens.push_back(TOK_LPAREN);
  tokens.push_back(TOK_NEWLINE);

  tokens.push_back(TOK_LPAREN);
  tokens.push_back(TOK_NEWLINE);

  tokens.push_back(TOK_INDENT);
  tokens.push_back(TOK_LPAREN);
  tokens.push_back(TOK_NEWLINE);

  tokens.push_back(TOK_DEDENT);
  tokens.push_back(TOK_DEDENT);
  tokens.push_back(TOK_LPAREN);
  tokens.push_back(TOK_NEWLINE);

  CheckTokens(contents, tokens);
}

// TEST_F(TokenizerTest, DummyTest) {
//   string contents =
//       "(  for 1  \n"
//       "  ( in = 2 * 4   \n"
//       "  ( a != 2.2 * 4   \n"
//       "  ( def b12 * c  \n"
//       "    (    \n"
//       "(   'abc'     \n";
//   ResetStream(contents);
//   tz_->NextTokenDebug();
//   const string& a = tz_->CurrentLineString();
//   while (tz_->NextTokenDebug()) {
//     cout << "now" << a << endl; // Aliasing
//   }
// }


// TEST_F(TokenizerTest, Compare) {
// }

} // namespace

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
