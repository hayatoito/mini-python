#include "python.h"
#include "eval.h"
#include "parser.h"
#include "native.h"

using namespace std;

int python_run(char* file_name) {
  parser_t parser =
      new Parser(
          new Tokenizer(
              new CharStream(
                  new ifstream(file_name))));
  env_t genv = mk_env(NULL, std::cout);
  register_navite_functions(genv);
  eval_file_input(genv, parser->ParseFileInput());
  return 0;
}
