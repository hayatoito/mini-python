#include "char_stream.h"

#include <fstream>
#include <sstream>

#include <gtest/gtest.h>

using namespace std;

namespace  {

class CharStreamTest : public testing::Test {
 protected:

  virtual void SetUp() {
    cs = NULL;
  }

  virtual void TearDown() {
    delete cs;
    cs = NULL;
  }

  void ResetStream(const string& contents) {
    delete cs;
    cs = MakeStringCharStream(contents);
  }

  void ReadNChar(int n) {
    for (int i = 0; i < n; ++i) {
      cs->NextChar();
    }
  }

  /*
  void VerifyLineString(const char* expected) {
    char* line = cs->CurrentLineString();
    EXPECT_STREQ(expected, line);
    free(line);
  }
  */

  void VerifyLineString(const string& expected) {
    EXPECT_EQ(expected, cs->CurrentLineString());
  }

  // Caller takes an ownership.
  static CharStream* MakeStringCharStream(const string& contents){
    return new CharStream(new istringstream(contents));
  }

  CharStream* cs;
};

TEST_F(CharStreamTest, ReadChar) {
  const string contents = "abc\ndef\n";
  ResetStream(contents);
  for (int i = 0; i < static_cast<int>(contents.size()); ++i) {
    cs->NextChar();
    EXPECT_EQ(contents[i], cs->CurrentChar());
  }
  cs->NextChar();
  EXPECT_EQ(EOF, cs->CurrentChar());
}

TEST_F(CharStreamTest, LineCount) {
  ResetStream("abc\nd\n");
  //EXPECT_EQ(0, cs->CurrentLine()); // NO gurantee.

  ReadNChar(1);
  EXPECT_EQ(1, cs->CurrentLine());
  EXPECT_EQ(1, cs->CurrentColumn());
  VerifyLineString("a");

  ReadNChar(3);
  EXPECT_EQ('\n', cs->CurrentChar());
  EXPECT_EQ(1, cs->CurrentLine());
  EXPECT_EQ(4, cs->CurrentColumn());
  VerifyLineString("abc\n");

  ReadNChar(1);
  EXPECT_EQ('d', cs->CurrentChar());
  EXPECT_EQ(2, cs->CurrentLine());
  EXPECT_EQ(1, cs->CurrentColumn());
  VerifyLineString("d");

  ReadNChar(1);
  EXPECT_EQ('\n', cs->CurrentChar());
  EXPECT_EQ(2, cs->CurrentLine());
  EXPECT_EQ(2, cs->CurrentColumn());
  //VerifyLineString("d\n");

  ReadNChar(1);
  EXPECT_EQ(EOF, cs->CurrentChar());
  EXPECT_EQ(2, cs->CurrentLine());
}

TEST_F(CharStreamTest, EmptyFile) {
  ResetStream("");
  ReadNChar(1);
  EXPECT_EQ(EOF, cs->CurrentChar());
  EXPECT_EQ(0, cs->CurrentLine());
}

TEST_F(CharStreamTest, EOFWithoutNewline) {
  ResetStream("a");
  ReadNChar(2);
  EXPECT_EQ(EOF, cs->CurrentChar());
  EXPECT_EQ(1, cs->CurrentLine());
}

} // namespace
