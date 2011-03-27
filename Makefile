CC = g++
CXX = g++

HOME = /home/hayato
INCLUDE_FLAGS = -I$(HOME)/opt/include

CFLAGS = -g -O2 -Wall -Wextra $(INCLUDE_FLAGS)
CXXFLAGS = $(CFLAGS)

LIBPATH = $(HOME)/opt/lib
LDFLAGS = -L$(LIBPATH) -Wl,-rpath=$(LIBPATH)

TEST_LIBS = -lgtest -lgtest_main
LDFLAGS_TEST = $(LDFLAGS) $(TEST_LIBS)

base_objs        = base.o
char_stream_objs = char_stream.o $(base_objs)

tokenizer_objs   = tokenizer.o $(char_stream_objs)
tokenizer_main_objs   = tokenizer_main.o $(tokenizer_objs)
tokenizer_test_objs   = tokenizer_test.o $(tokenizer_objs)

syntree_objs     = syntree.o $(tokenizer_objs)
parser_objs      = parser.o $(syntree_objs)
parser_main_objs = parser_main.o $(parser_objs)
pyvalues_objs    = pyvalues.o $(syntree_objs)
eval_objs        = eval.o $(pyvalues_objs)
native_objs      = native.o $(pyvalues_objs)

eval_test_objs   = eval_test.o $(eval_objs) $(native_objs)


# $@
#     The file name of the target.
# $%
#     The target member name, when the target is an archive member.
# $<
#     The name of the first prerequisite.
# $?
#     The names of all the prerequisites that are newer than the target, with spaces between them. For prerequisites which are archive members, only the member named is used (see Archives).
# $^
# $+
#     The names of all the prerequisites, with spaces between them. For prerequisites which are archive members, only the member named is used (see Archives). The value of $^ omits duplicate prerequisites, while $+ retains them and preserves their order.
# $*
#     The stem with which an implicit rule matches (see How Patterns Match). 

.PHONY: clean

parser_main: $(parser_main_objs)

eval_test : $(eval_test_objs)
	$(CXX) $(LDFLAGS_TEST) $^ -o $@


clean :
	rm -f eval_test parser_main *.o


