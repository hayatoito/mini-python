#ifndef MINIPY_INT_STACK_H__
#define MINIPY_INT_STACK_H__

class IntStack {
 public:
  IntStack();
  ~IntStack();
  void Push(int c);
  int Peek();
  int Pop();

  static const int kInitialCapacity;

 private:
  int* a_;
  int capacity_;
  int size_;
};

#endif // MINIPY_INT_STACK_H__
