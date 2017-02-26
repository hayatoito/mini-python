#include "char_buffer.h"

#include <gtest/gtest.h>

using namespace std;

namespace  {

class CharBufferTest : public testing::Test {
 protected:
  virtual void SetUp() {
  }

  virtual void TearDown() {
  }

  void VerifyBuffer(const char* expected) {
    char* actual = cb.GetString();
    EXPECT_STREQ(expected, actual);
    EXPECT_EQ(strlen(expected), cb.Size());
    free(actual);
  }

  CharBuffer cb;
};

TEST_F(CharBufferTest, AppendChar) {
  VerifyBuffer("");

  cb.Append('a');
  VerifyBuffer("a");

  cb.Append('b');
  VerifyBuffer("ab");

  for (int i = 0; i < 10; ++i) {
    cb.Append('a' + i);
  }

  VerifyBuffer("ababcdefghij");
}

TEST_F(CharBufferTest, Reset) {
  cb.Append('a');

  cb.Reset();
  VerifyBuffer("");
}

} // namespace

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
