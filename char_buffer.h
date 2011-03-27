#ifndef MINIPY_CHAR_BUFFER_H__
#define MINIPY_CHAR_BUFFER_H__

using namespace std;

class CharBuffer {
 public:
  CharBuffer();
  ~CharBuffer();
  void Reset();
  void Append(char c);
  int Size();
  // Caller takes an ownership.
  char* GetString();

  static const int kInitialCapacity;

 private:
  char* a_;
  int capacity_;
  int size_;
};

#endif // MINIPY_CHAR_BUFFER_H__
