import sys
import os
import time
import subprocess
import pylab as p

__path__ = os.path.dirname(__file__)

GRAS_ENV = {
    'PATH': os.path.join(os.getenv('HOME'), 'usr/gras/bin:$PATH:%s'%os.getenv('PATH')),
    'LD_LIBRARY_PATH': os.path.join(os.getenv('HOME'), 'usr/gras/lib:%s'%os.getenv('LD_LIBRARY_PATH')),
    'PYTHONPATH': os.path.join(os.getenv('HOME'), 'usr/gras/lib/python2.7/dist-packages:%s'%os.getenv('PYTHONPATH')),
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
        tokwargs(wat='GrExtras Adder', args=[sys.executable, os.path.join(__path__, 'bm_grextras_math.py'), '1e8', 'extras_add'], env=GRAS_ENV),
        tokwargs(wat='gr-blocks Adder', args=[sys.executable, os.path.join(__path__, 'bm_grextras_math.py'), '1e8', 'blocks_add'], env=GRAS_ENV),
        tokwargs(wat='GrExtras Multiplier', args=[sys.executable, os.path.join(__path__, 'bm_grextras_math.py'), '1e8', 'extras_mult'], env=GRAS_ENV),
        tokwargs(wat='gr-blocks Multiplier', args=[sys.executable, os.path.join(__path__, 'bm_grextras_math.py'), '1e8', 'blocks_mult'], env=GRAS_ENV),
    ],
    to_result = lambda t: 1e8/t
)

def time_a_single_one(args, env):
    t0 = time.time()
    p = subprocess.Popen(args=args, env=env)
    p.wait()
    t1 = time.time()
    return t1-t0

def do_a_benchmark(bm):
    results = list()
    for run in bm['tests']:
        #print run['env']
        r = time_a_single_one(args=run['args'], env=run['env'])
        results.append(bm['to_result'](r))
    print results

if __name__ == '__main__':
    do_a_benchmark(BENCHMARK_MATH_OPS)
