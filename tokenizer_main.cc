#include "tokenizer.h"
#include "char_stream.h"

#include <fstream>

using namespace std;

int main(int argc, char** argv) {

  tokenizer_t tz = new Tokenizer(new CharStream(new ifstream(argv[1])));
  tz->NextToken();
  while (tz->CurrentTokenKind() != TOK_EOF) {
    cout << tz->CurrentTokenInfo() << endl;
    tz->NextToken();
  }
  return 0;
}
