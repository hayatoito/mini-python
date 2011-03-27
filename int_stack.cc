#include "int_stack.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace std;

const int IntStack::kInitialCapacity = 8;

IntStack::IntStack()
    : a_((int*)malloc(kInitialCapacity * sizeof(int))),
      capacity_(kInitialCapacity),
      size_(0) {
}

IntStack::~IntStack() {
  free(a_);
}

void IntStack::Push(int c) {
  if (size_ == capacity_) {
    int* b = static_cast<int*>(malloc(capacity_ * 2 * sizeof(int)));
    memcpy(b, a_, capacity_ * sizeof(int));
    free(a_);
    a_ = b;
    capacity_ *= 2;
  }
  a_[size_++] = c;
}

int IntStack::Peek() {
  if (size_ == 0) {
    fprintf(stderr, "error: Try to Peek() againt an empty stack.\n");
    exit(1);
  }
  return a_[size_ - 1];
}

int IntStack::Pop() {
  if (size_ == 0) {
    fprintf(stderr, "error: Try to Pop() againt an empty stack.\n");
    exit(1);
  }
  return a_[--size_];
}
