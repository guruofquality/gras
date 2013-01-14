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

BENCHMARKS = (
    BENCHMARK_MATH_OPS,
    BENCHMARK_MANY_11_BLOCKS,
)
