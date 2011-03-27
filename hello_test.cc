// #include "pyvalues.h"

#include <iostream>
#include <gtest/gtest.h>

using namespace std;

class HelloTest : public testing::Test {
 protected:

  virtual void SetUp() {
  }

  virtual void TearDown() {
  }
};

namespace  {

class Trivial {
 public:
  Trivial() {}
  int a;
  long b;
};

union myunion {
  int ui;
  Trivial ut;
};

TEST_F(HelloTest, uniontest) {
  myunion u;
  cout << "u.ui: " << u.ui << endl;
  cout << "u.ut.a: " << u.ut.a << endl;
}


}  // namespace
