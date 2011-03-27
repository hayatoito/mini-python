#include "char_buffer.h"

#include <cstdlib>
#include <cstring>

using namespace std;

const int CharBuffer::kInitialCapacity = 8;

CharBuffer::CharBuffer()
    : a_((char*)malloc(kInitialCapacity)),
      capacity_(kInitialCapacity),
      size_(0) {
  memset(a_, 0, kInitialCapacity);
}

CharBuffer::~CharBuffer() {
  free(a_);
}

void CharBuffer::Reset() {
  size_ = 0;
  memset(a_, 0, capacity_);
}

void CharBuffer::Append(char c) {
  if (size_ + 1 == capacity_) {
    char* b = static_cast<char*>(malloc(capacity_ * 2));
    memset(b, 0, capacity_ * 2);
    memcpy(b, a_, capacity_);
    free(a_);
    a_ = b;
    capacity_ *= 2;
  }
  a_[size_++] = c;
}

char* CharBuffer::GetString() {
  // char* b = static_cast<char*>(malloc(size_ + 1));
  // memcpy(b, a_, size_ + 1);
  // return b;
  return strdup(a_);
}

int CharBuffer::Size() {
  return size_;
}
