#include "int_stack.h"

#include <gtest/gtest.h>

using namespace std;

namespace  {

class IntStackTest : public testing::Test {
 protected:
  virtual void SetUp() {
  }

  virtual void TearDown() {
  }

  IntStack stack;
};

TEST_F(IntStackTest, AppendInt) {
  stack.Push(2);
  EXPECT_EQ(2, stack.Peek());

  stack.Push(3);
  EXPECT_EQ(3, stack.Peek());

  stack.Push(5);
  EXPECT_EQ(5, stack.Peek());

  for (int i = 0; i < IntStack::kInitialCapacity; ++i) {
    stack.Push(i);
    EXPECT_EQ(i, stack.Peek());
  }

  for (int i = 0; i < IntStack::kInitialCapacity; ++i) {
    EXPECT_EQ(IntStack::kInitialCapacity - 1 - i, stack.Peek());
    EXPECT_EQ(IntStack::kInitialCapacity - 1 - i, stack.Pop());
  }
}

TEST_F(IntStackTest, PopEmptyStack) {
  EXPECT_EXIT({stack.Pop();}, testing::ExitedWithCode(1), "error:.*");
  EXPECT_EXIT({stack.Peek();}, testing::ExitedWithCode(1), "error:.*");
}

} // namespace
