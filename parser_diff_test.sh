#!/bin/zsh

BUILD_DIR=../build/debug/minipy
PROG=${BUILD_DIR}/parser_main
TEST_DATA_DIR=tokenizer

${PROG} ${TEST_DATA_DIR}/prob_6.py >| ${BUILD_DIR}/myans_parser_6.1.txt
${PROG} ${BUILD_DIR}/myans_parser_6.1.txt >| ${BUILD_DIR}/myans_parser_6.2.txt

diff -u ${BUILD_DIR}/myans_parser_6.1.txt ${BUILD_DIR}/myans_parser_6.2.txt
