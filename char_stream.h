#ifndef MINIPY_CHAR_STREAM_H__
#define MINIPY_CHAR_STREAM_H__

#include "base.h"
// #include "char_buffer.h"

#include <string>
#include <iostream>

using namespace std;

class CharStream {
 public:
  CharStream(istream* istream_);
  char CurrentChar();
  int CurrentLine();
  int CurrentColumn();
  void NextChar();
  virtual ~CharStream() {
    //istream_->close(); // We should use template specialization.
    delete istream_;
  };

  const char* FileName();
  const string& CurrentLineString();

 private:
  istream* istream_;
  char cur_char_;
  int cur_line_;
  int cur_column_;
  //CharBuffer cur_line_string_;
  string cur_line_string_;
  DISALLOW_IMPLICIT_CONSTRUCTORS(CharStream);
};

typedef CharStream* char_stream_t;

#endif // MINIPY_CHAR_STREAM_H__
