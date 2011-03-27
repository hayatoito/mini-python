#include "base.h"
#include "char_stream.h"
#include "parser.h"
#include "eval.h"
#include "native.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>

#include <gtest/gtest.h>

using namespace std;

class EvalTest : public testing::Test {
 protected:

  virtual void SetUp() {
  }

  virtual void TearDown() {
  }

  string Eval(const string& contents) {
    parser_ =
        new Parser(
            new Tokenizer(
                new CharStream(
                    new istringstream(contents))));
    file_input = parser_->ParseFileInput();
    ostringstream oss;
    env_t genv = mk_env(NULL, oss);
    register_navite_functions(genv);
    //cout << genv;
    eval_file_input(genv, file_input);
    return oss.str();
  }

  parser_t parser_;
  file_input_t file_input;
};

namespace  {

TEST_F(EvalTest, SimpleEvalTest) {
  LOG(INFO) << "hello logging!";
  EXPECT_EQ(
      "3\n108\n30\n",
      Eval("x = 2 + 1\n"
           "print x\n"
           "def f(a, b):\n"
           "  return a + b\n"
           "x = x + 5\n"
           "y = 100\n"
           "print x + y\n"
           "z = f(10, 20)\n"
           "print z\n"
           ));

  EXPECT_EQ(
      "2\n5\n",
      Eval("x = 10\n"
           "if x:\n"
           "  print 2\n"
           "x = 0\n"
           "if x:\n"
           "  print 3\n"
           "else:\n"
           "  print 5\n"
           ));

  ostringstream s;
  s << "x = 10" << endl;
  s << "if x < 20:" << endl;
  s << "  print 2" << endl;
  s << "else:" << endl;
  s << "  print 3" << endl;
  EXPECT_EQ("2\n", Eval(s.str()));

  s.str("");
  s << "x = 0" << endl;
  s << "while x < 3:" << endl;
  s << "  print x" << endl;
  s << "  x = x + 1" << endl;
  EXPECT_EQ("0\n1\n2\n", Eval(s.str()));


  s.str("");
  s << "print 3 and 5" << endl;
  s << "print 3 or 5" << endl;
  s << "print 0 and 7" << endl;
  s << "print 0 or 7" << endl;
  EXPECT_EQ("5\n3\n0\n7\n", Eval(s.str()));

  s.str("");
  s << "def f():" << endl;
  s << "  print 2" << endl;
  s << "  return 3" << endl;
  s << "def g():" << endl;
  s << "  print 4" << endl;
  s << "  return 5" << endl;
  s << "print 0 and f()" << endl;
  s << "print 0 or g()" << endl;
  s << "print 1 and f()" << endl;
  s << "print 1 or g()" << endl;
  EXPECT_EQ("0\n4\n5\n2\n3\n1\n", Eval(s.str()));
}

TEST_F(EvalTest, StringEvalTest) {
  ostringstream s;
  s.str("");
  s << "print 'hello'" << endl;
  EXPECT_EQ("hello\n", Eval(s.str()));

  s.str("");
  s << "x = 'hello'" << endl;
  s << "y = ' world'" << endl;
  s << "print x + y" << endl;
  EXPECT_EQ("hello world\n", Eval(s.str()));
}


}  // namespace
