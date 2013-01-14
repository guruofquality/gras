import sys
import os
import time
import subprocess
import copy
import numpy

from matplotlib.backends.backend_pdf import PdfPages
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
from matplotlib.figure import Figure
from matplotlib.font_manager import FontProperties

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

def time_a_single_one(args, env):
    print env
    t0 = time.time()
    p = subprocess.Popen(args=args, env=env)
    p.wait()
    t1 = time.time()
    return t1-t0

def do_a_benchmark(bm):
    results = list()
    test_names = list()
    for run in bm['tests']:
        test_names.append(run['wat'])
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

    bogomips = numpy.array(results)/1e6
    ind = numpy.arange(len(test_names))
    width = 0.35
    fig = Figure()
    fig.set_size_inches((11,8.5))
    FigureCanvas(fig)
    ax = fig.add_subplot(1, 1, 1,
        ylabel='Performance (BogoMips)', title=bm['wat'],
        xlabel='', xticks=ind+width/2., xticklabels=test_names
    )
    rects = ax.bar(ind, bogomips, width, color='blue')
    ax.set_xlim(-width, max(len(ind), 4))
    ax.set_ylim(0, max(*bogomips) + numpy.std(bogomips))
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x()+rect.get_width()/2.0, 1.03*height, '%.3f'%height, horizontalalignment='center')
    ax.grid(True)
    return fig


if __name__ == '__main__':
    ####################################################################
    ## create pdf generator
    ####################################################################
    pdf_pages = PdfPages('gras_benchmark.pdf')

    ####################################################################
    ## loop through tests
    ####################################################################
    for bm in BENCHMARKS:
        fig = do_a_benchmark(bm)
        pdf_pages.savefig(fig)

    ####################################################################
    ## done
    ####################################################################
    print 'make pdf...'
    pdf_pages.close()
    print 'done!'
