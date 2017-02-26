def options(opt):
    opt.add_option('--release', action='store_true', dest='release',
                   help='configure release build')
    opt.load('compiler_cxx')


def configure(conf):
    # e.g.
    # CC=clang CXX=clang++ waf configure --out ~/src/build/c
    conf.load('compiler_cxx')
    conf.load('waf_unit_test')
    conf.load('clang_compilation_database')

    conf.env.append_unique('CXXFLAGS', ['-Wall', '-std=c++11'])
    conf.env.append_unique('RPATH', ['$ORIGIN'])

    print('is_release:', conf.options.release)
    configure_cc(conf, conf.options.release)


def add_cflags(env, flags):
    # env.derive() is a shallow copy. So assingn a new list.
    env.CFLAGS = env.CFLAGS[:]
    env.CXXFLAGS = env.CXXFLAGS[:]

    env.append_unique('CFLAGS', flags)
    env.append_unique('CXXFLAGS', flags)


def configure_cc(conf, release=False):
    if release:
        add_cflags(conf.env, ['-O2', '-DNDEBUG'])
        # conf.define('NDEBUG', 1)
        # conf.write_config_header('config.h')
    else:
        add_cflags(conf.env, ['-O0', '-g'])


def build(bld):
    bld(export_includes=['./googletest/googletest',
                         './googletest/googletest/include'],
        name='gtest_includes')
    bld.stlib(source='./googletest/googletest/src/gtest-all.cc',
              target='gtest',
              use='gtest_includes')

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

    bld.program(source='tokenizer_main.cc', target='tokenizer_main',
                use='minipy')
    bld.program(source='parser_main.cc', target='parser_main',
                use='minipy')
    bld.program(source='python_main.cc', target='python_main',
                use='minipy')

    bld.program(source='char_stream_test.cc', target='char_stream_test',
                features='test',
                lib=['pthread'],
                use='minipy gtest')
    bld.program(source='pyvalues_test.cc', target='pyvalues_test',
                features='test',
                lib=['pthread'],
                use='minipy gtest')
    bld.program(source='tokenizer_test.cc', target='tokenizer_test',
                features='test',
                lib=['pthread'],
                use='minipy gtest')
