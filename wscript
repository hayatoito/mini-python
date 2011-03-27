def build(bld):
  minipy_source = [
    'base.cc',
    'char_stream.cc',
    'tokenizer.cc',
    'syntree.cc',
    'parser.cc',
    'pyvalues.cc',
    'eval.cc',
    'native.cc',
    'python.cc',
    ]

  bld.shlib(source=minipy_source, target='minipy')

  bld.program(source='tokenizer_main.cc', target='tokenizer_main', use='minipy')
  bld.program(source='parser_main.cc', target='parser_main', use='minipy')
  bld.program(source='python_main.cc', target='python_main', use='minipy')

  bld.program(features='test', source='char_stream_test.cc', target='char_stream_test',
              uselib = ['gtest', 'gtest_main'], use='minipy')
  bld.program(features='test', source='pyvalues_test.cc', target='pyvalues_test',
              uselib = ['gtest', 'gtest_main'], use='minipy')
  bld.program(features='test', source='tokenizer_test.cc', target='tokenizer_test',
              uselib = ['gtest', 'gtest_main'], use='minipy')

