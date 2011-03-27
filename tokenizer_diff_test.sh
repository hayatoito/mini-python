#!/bin/zsh

BUILD_DIR=../build/debug/minipy
PROG=${BUILD_DIR}/tokenizer_main
TEST_DATA_DIR=tokenizer

for i in 1 2 3 4 5 6 ; do
  echo "Using ${TEST_DATA_DIR}/prob_${i}.py..."
  diff -u ${TEST_DATA_DIR}/ans_${i} =(${PROG} ${TEST_DATA_DIR}/prob_${i}.py)
done

# for i in 1 2 3 4 ; do
#   echo ${TEST_DIR}/err_prob_${i}.py
#   echo Example
#   cat ${TEST_DIR}/err_ans_${i}
#   echo Actual
#   ${PROG} ${TEST_DIR}/err_prob_${i}.py
# done
