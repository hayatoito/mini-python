#include "cs.h"

#include <cstdio>
#include <cstdlib>

void line_count(char * filename) {
  char_stream_t cs = mk_char_stream(filename); /* char_stream を作る */
  if (cs == 0) {
    fprintf(stderr, "could not open %s\n", filename);
    exit(1);
  }
  while (char_stream_cur_char(cs) != EOF) { /* EOF が出るまで読んでいく */
    char_stream_next_char(cs);
  }
  /* char_stream の仕様により，EOF の行番号がファイルの行数 */
  printf("%d lines\n", char_stream_cur_line(cs));
}

int main(int argc, char ** argv) {
  if (argc != 2) {
    fprintf(stderr, "usage: %s filename\n", argv[0]);
    exit(1);
  }
  line_count(argv[1]);
  return 0;
}
