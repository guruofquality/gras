# benchmark registry

import os

def tokwargs(**kwargs): return kwargs

INSTALL_PREFIX = '/opt/usr'

DURATION = '1.3' #seconds

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

BENCHMARK_LINEAR_CHAIN = tokwargs(
    wat='Benchmark the schedulers with linear chain topology',
    moar='''\
- Topology is a linear chain of one input/one output blocks.
- GRAS will use only the buffer pool allocator,
and every work will fully consume available buffers.''',
    tests = [
        tokwargs(wat='GRAS',     args=['tb_linear_chain.py', DURATION], env=GRAS_ENV, expand=True),
        tokwargs(wat='GRSS',     args=['tb_linear_chain.py', DURATION], env=GR_ENV),
    ],
)

BENCHMARK_COMBINER_ARRAY = tokwargs(
    wat='Benchmark the schedulers with combiner array topology',
    moar='''\
- Topology is a tower of two input math blocks.
- GRAS will use only the buffer pool allocator,
and every work will fully consume available buffers.''',
    tests = [
        tokwargs(wat='GRAS',     args=['tb_combiner_array.py', DURATION], env=GRAS_ENV, expand=True),
        tokwargs(wat='GRSS',     args=['tb_combiner_array.py', DURATION], env=GR_ENV),
    ],
)

BENCHMARK_MANY_RATE_BLOCKS = tokwargs(
    wat='Benchmark the schedulers with many rate changing blocks',
    moar='''\
- Compare simultaneous changing ratio blocks in each scheduler.
- GRAS will use only the buffer pool allocator,
and every work will fully consume available buffers.''',
    tests = [
        tokwargs(wat='GRAS',     args=['tb_many_rate_changes.py', '--dur', DURATION], env=GRAS_ENV, expand=True),
        tokwargs(wat='GRSS',     args=['tb_many_rate_changes.py', '--dur', DURATION], env=GR_ENV),
    ],
)

BENCHMARK_DFIR_BLOCK = tokwargs(
    wat='Benchmark the schedulers with a FIR block',
    moar='''\
- Compare filter blocks in each scheduler.
- Shows both schedulers using circular buffer.''',
    tests = [
        tokwargs(wat='GRAS',     args=['tb_filter_block.py', '--dur', DURATION, '--which', 'dfir'], env=GRAS_ENV, expand=True),
        tokwargs(wat='GRSS',     args=['tb_filter_block.py', '--dur', DURATION, '--which', 'dfir'], env=GR_ENV),
    ],
)

BENCHMARK_RESAMP_BLOCK = tokwargs(
    wat='Benchmark the schedulers with a resampler block',
    moar='''\
- Compare filter blocks in each scheduler.
- Shows both schedulers using circular buffer.''',
    tests = [
        tokwargs(wat='GRAS',     args=['tb_filter_block.py', '--dur', DURATION, '--which', 'resamp'], env=GRAS_ENV, expand=True),
        tokwargs(wat='GRSS',     args=['tb_filter_block.py', '--dur', DURATION, '--which', 'resamp'], env=GR_ENV),
    ],
)

BENCHMARK_ADD_OPS = tokwargs(
    wat='Benchmark GrExtras vs gr-blocks adder blocks',
    moar='''\
- Compare math block implementations using GRAS.
- All blocks are using vector optimization.
- GrExtras math blocks avoid an unnecessary memcpy.
- GrExtras math blocks enable automatic bufer in-placing.''',
    tests = [
        tokwargs(wat='GrExtras\n(GRAS)',        args=['tb_grextras_math.py', DURATION, 'extras_add'], env=GRAS_ENV, expand=True),
        tokwargs(wat='gr-blocks\n(GRAS)',       args=['tb_grextras_math.py', DURATION, 'blocks_add'], env=GRAS_ENV, expand=True),
        tokwargs(wat='gr-blocks\n(GRSS)',       args=['tb_grextras_math.py', DURATION, 'blocks_add'], env=GR_ENV),
    ],
)

BENCHMARK_MULT_OPS = tokwargs(
    wat='Benchmark GrExtras vs gr-blocks multiplier blocks',
    moar='''\
- Compare math block implementations using GRAS.
- All blocks are using vector optimization.
- GrExtras math blocks avoid an unnecessary memcpy.
- GrExtras math blocks enable automatic bufer in-placing.''',
    tests = [
        tokwargs(wat='GrExtras\n(GRAS)',       args=['tb_grextras_math.py', DURATION, 'extras_mult'], env=GRAS_ENV, expand=True),
        tokwargs(wat='gr-blocks\n(GRAS)',      args=['tb_grextras_math.py', DURATION, 'blocks_mult'], env=GRAS_ENV, expand=True),
        tokwargs(wat='gr-blocks\n(GRSS)',      args=['tb_grextras_math.py', DURATION, 'blocks_mult'], env=GR_ENV),
    ],
)

BENCHMARK_DELAY_BLOCKS = tokwargs(
    wat='Benchmark GrExtras vs gr-core delay block',
    moar='''\
- Compare delay block implementations using GRAS.
- The GrExtras implementation uses zero-copy.''',
    tests = [
        tokwargs(wat='GrExtras\n(GRAS)',          args=['tb_grextras_delay.py', DURATION, 'extras_delay'], env=GRAS_ENV, expand=True),
        tokwargs(wat='gr-core\n(GRAS)',           args=['tb_grextras_delay.py', DURATION, 'core_delay'], env=GRAS_ENV, expand=True),
        tokwargs(wat='gr-core\n(GRSS)',           args=['tb_grextras_delay.py', DURATION, 'core_delay'], env=GR_ENV),
    ],
)

BENCHMARKS = (
    BENCHMARK_LINEAR_CHAIN,
    BENCHMARK_COMBINER_ARRAY,
    BENCHMARK_MANY_RATE_BLOCKS,
    BENCHMARK_DFIR_BLOCK,
    BENCHMARK_RESAMP_BLOCK,
    BENCHMARK_ADD_OPS,
    BENCHMARK_MULT_OPS,
    BENCHMARK_DELAY_BLOCKS,
)
