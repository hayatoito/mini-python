#include "parser.h"

#include <fstream>

using namespace std;

int main(int argc, char** argv) {
  parser_t parser = new Parser(new Tokenizer(new CharStream(new ifstream(argv[1]))));
  file_input_t file_input = parser->ParseFileInput();
  Printer p;
  p.PrintFileInput(file_input);
  return 0;
}
