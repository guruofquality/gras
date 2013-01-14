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

import multiprocessing
cpu_count = multiprocessing.cpu_count()

from bm_registry import BENCHMARKS

NUM_RUNS_PER_TEST = 5

__path__ = os.path.dirname(__file__)

def time_a_single_one(args, env):
    print env
    t0 = time.time()
    p = subprocess.Popen(args=args, env=env)
    p.wait()
    t1 = time.time()
    return t1-t0

def do_a_benchmark(bm):
    title = bm['wat']
    print '#'*(len(title)+25)
    print '## running benchmark:', title
    print '#'*(len(title)+25)
    result_means = list()
    result_stddevs = list()
    test_names = list()
    for run in bm['tests']:
        test_name = run['wat']
        print '-'*(len(test_name)+25)
        print '-- running test:', test_name.replace('\n', ' ')
        print '-'*(len(test_name)+25)
        test_names.append(test_name)
        args = run['args']
        args[0] = os.path.join(__path__, args[0])
        args = [sys.executable] + args
        env = run['env']
        env = copy.copy(env)
        if run.has_key('envextra'):
            env.update(run['envextra'])
        run_results = list()
        for num_runs in range(NUM_RUNS_PER_TEST):
            t = time_a_single_one(args=args, env=env)
            run_results.append(bm['to_result'](t))
        result_means.append(numpy.average(run_results))
        result_stddevs.append(numpy.std(run_results))

    print 'result_means', result_means
    print 'result_stddevs', result_stddevs

    bogomips = numpy.array(result_means)/1e6
    bogomips_dev = numpy.array(result_stddevs)/1e6
    ind = numpy.arange(len(test_names))
    width = 0.35
    fig = Figure()
    fig.set_size_inches((11,8.5))
    FigureCanvas(fig)
    ax = fig.add_subplot(1, 1, 1,
        ylabel='Performance (BogoMips)', title=title,
        xlabel='', xticks=ind+width/2., xticklabels=test_names
    )
    rects = ax.bar(ind, bogomips, width, color='blue',
        yerr=bogomips_dev,
        error_kw=dict(elinewidth=6, ecolor='pink'),
    )
    ax.set_xlim(-width, max(len(ind), 4))
    ax.set_ylim(0, max(*bogomips) + max(*bogomips_dev)*3)
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x()+rect.get_width()/2.0, 1.01*height, '%.3f'%height, horizontalalignment='center')
    ax.grid(True)
    print '\n'
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
