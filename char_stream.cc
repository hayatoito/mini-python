#include "char_stream.h"

#include <stdio.h>  // For EOF

using namespace std;

CharStream::CharStream(istream* istream)
    : istream_(istream),
      cur_char_('\n'),
      cur_line_(0),
      cur_column_(0),
      cur_line_string_() {
}

char CharStream::CurrentChar() {
  return cur_char_;
}

int CharStream::CurrentLine() {
  return cur_line_;
}

int CharStream::CurrentColumn() {
  return cur_column_;
}

void CharStream::NextChar() {
  char old_char = cur_char_;
  cur_char_ = istream_->get();
  if (old_char == '\n') {
    cur_column_ = 1;
    if (cur_char_ != EOF) {
      ++cur_line_;
    }
    //cur_line_string_.Reset();
    cur_line_string_.clear();
  } else {
    ++cur_column_;
  }
  //cur_line_string_.Append(cur_char_);
  cur_line_string_.push_back(cur_char_);
}

const char* CharStream::FileName() {
  return "UnknownSource";
}

const string& CharStream::CurrentLineString() {
  return cur_line_string_;
}
