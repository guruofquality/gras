# benchmark registry

import os

def tokwargs(**kwargs): return kwargs

INSTALL_PREFIX = '/opt/usr'

GRAS_ENV = {
    'PATH': os.path.join(INSTALL_PREFIX, 'gras/bin:$PATH:%s'%os.getenv('PATH')),
    'LD_LIBRARY_PATH': os.path.join(INSTALL_PREFIX, 'gras/lib:%s'%os.getenv('LD_LIBRARY_PATH')),
    'PYTHONPATH': os.path.join(INSTALL_PREFIX, 'gras/lib/python2.7/dist-packages:%s'%os.getenv('PYTHONPATH')),
}

GR_ENV = {
    'PATH': os.path.join(INSTALL_PREFIX, 'gr/bin:$PATH:%s'%os.getenv('PATH')),
    'LD_LIBRARY_PATH': os.path.join(INSTALL_PREFIX, 'gr/lib:%s'%os.getenv('LD_LIBRARY_PATH')),
    'PYTHONPATH': os.path.join(INSTALL_PREFIX, 'gr/lib/python2.7/dist-packages:%s'%os.getenv('PYTHONPATH')),
}

BENCHMARK_MATH_OPS = tokwargs(
    wat='Benchmark GrExtras vs gr-blocks math blocks',
    moar='''
        All blocks are using vector optimization.
        - GrExtras math blocks avoid an unnecessary memcpy.
        - GrExtras math blocks enable automatic bufer in-placing.
    ''',
    tests = [
        tokwargs(wat='GrExtras Adder',          args=['tb_grextras_math.py', '1e8', 'extras_add'], env=GRAS_ENV),
        tokwargs(wat='gr-blocks Adder',         args=['tb_grextras_math.py', '1e8', 'blocks_add'], env=GRAS_ENV),
        tokwargs(wat='GrExtras Multiplier',     args=['tb_grextras_math.py', '1e8', 'extras_mult'], env=GRAS_ENV),
        tokwargs(wat='gr-blocks Multiplier',    args=['tb_grextras_math.py', '1e8', 'blocks_mult'], env=GRAS_ENV),
    ],
    to_result = lambda t: 1e8/t
)

BENCHMARK_MANY_11_BLOCKS = tokwargs(
    wat='Benchmark the schedulers with many 1:1 ratio blocks',
    moar='''TODO''',
    tests = [
        tokwargs(wat='Many 1:1 GRAS',     args=['tb_many_1_to_1_blocks.py', '1e8'], env=GRAS_ENV),
        #tokwargs(wat='Many 1:1 GRAS - TPP',         args=['tb_many_1_to_1_blocks.py', '1e8'], env=GRAS_ENV, envextra=tokwargs(GRAS_TPP='1')),
        tokwargs(wat='Many 1:1 GR',                 args=['tb_many_1_to_1_blocks.py', '1e8'], env=GR_ENV),
    ],
    to_result = lambda t: 1e8/t
)

BENCHMARK_FILTER_BLOCK = tokwargs(
    wat='Benchmark the schedulers with a filter block',
    moar='''TODO''',
    tests = [
        tokwargs(wat='GRAS decim FIR',     args=['tb_filter_block.py', '--num', '1e7', '--which', 'dfir'], env=GRAS_ENV),
        tokwargs(wat='GR decim FIR',       args=['tb_filter_block.py', '--num', '1e7', '--which', 'dfir'], env=GR_ENV),
        tokwargs(wat='GRAS resampler',     args=['tb_filter_block.py', '--num', '1e7', '--which', 'resamp'], env=GRAS_ENV),
        tokwargs(wat='GR resampler',       args=['tb_filter_block.py', '--num', '1e7', '--which', 'resamp'], env=GR_ENV),
    ],
    to_result = lambda t: 1e7/t
)

BENCHMARK_DELAY_BLOCKS = tokwargs(
    wat='Benchmark GrExtras vs gr-core delay block',
    moar='''TODO''',
    tests = [
        tokwargs(wat='GrExtras Delay\n(zero-copy)',          args=['tb_grextras_delay.py', '1e8', 'extras_delay'], env=GRAS_ENV),
        tokwargs(wat='gr-core Delay\n(mem-copy)',            args=['tb_grextras_delay.py', '1e8', 'core_delay'], env=GRAS_ENV),
    ],
    to_result = lambda t: 1e8/t
)

BENCHMARKS = (
    BENCHMARK_MATH_OPS,
    BENCHMARK_MANY_11_BLOCKS,
    BENCHMARK_FILTER_BLOCK,
    BENCHMARK_DELAY_BLOCKS,
)
