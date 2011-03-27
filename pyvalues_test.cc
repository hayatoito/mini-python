#include "pyvalues.h"

#include <gtest/gtest.h>

using namespace std;

class PyValuesTest : public testing::Test {
 protected:

  virtual void SetUp() {
  }

  virtual void TearDown() {
  }
};

namespace  {

TEST_F(PyValuesTest, mk_py_val) {
  py_val_t a = mk_py_int(5);
  EXPECT_TRUE(py_is_int(a));
  EXPECT_FALSE(py_is_float(a));
  EXPECT_EQ(5, py_val_int(a, NULL, NULL));

  a = mk_py_float(5.0);
  EXPECT_TRUE(py_is_float(a));
  EXPECT_EQ(5.0, py_val_float(a, NULL, NULL));

  a = mk_py_none();
  EXPECT_TRUE(py_is_none(a));

  // a = mk_py_string("abc");
  // EXPECT_TRUE(py_is_string(a));
  // EXPECT_STREQ("abc", py_val_string(a, NULL, NULL));
}


}  // namespace
