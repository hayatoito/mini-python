#include "eval.h"
#include "parser.h"
#include "native.h"

#include <fstream>

using namespace std;

int main(int argc, char** argv) {
  parser_t parser =
      new Parser(
          new Tokenizer(
              new CharStream(
                  new ifstream(argv[1]))));
  env_t genv = mk_env(NULL, std::cout);
  register_navite_functions(genv);
  eval_file_input(genv, parser->ParseFileInput());
  return 0;
}
