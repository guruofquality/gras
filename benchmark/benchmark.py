import sys
import os
import time
import subprocess
import pylab as p
import copy

__path__ = os.path.dirname(__file__)

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

def tokwargs(**kwargs):
    return kwargs

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
    wat='Benchmark the schedulers with many 1:1 blocks',
    moar='''TODO''',
    tests = [
        tokwargs(wat='Many 1:1 GRAS - Default',     args=['tb_many_1_to_1_blocks.py', '1e8'], env=GRAS_ENV),
        #tokwargs(wat='Many 1:1 GRAS - TPP',         args=['tb_many_1_to_1_blocks.py', '1e8'], env=GRAS_ENV, envextra=tokwargs(GRAS_TPP='1')),
        tokwargs(wat='Many 1:1 GR',                 args=['tb_many_1_to_1_blocks.py', '1e8'], env=GR_ENV),
    ],
    to_result = lambda t: 1e8/t
)

def time_a_single_one(args, env):
    print env
    t0 = time.time()
    p = subprocess.Popen(args=args, env=env)
    p.wait()
    t1 = time.time()
    return t1-t0

def do_a_benchmark(bm):
    results = list()
    for run in bm['tests']:
        args = run['args']
        args[0] = os.path.join(__path__, args[0])
        args = [sys.executable] + args
        env = run['env']
        env = copy.copy(env)
        if run.has_key('envextra'):
            env.update(run['envextra'])
        r = time_a_single_one(args=args, env=env)
        results.append(bm['to_result'](r))
    print results

if __name__ == '__main__':
    #do_a_benchmark(BENCHMARK_MATH_OPS)
    do_a_benchmark(BENCHMARK_MANY_11_BLOCKS)
