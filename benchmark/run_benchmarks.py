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

NUM_RUNS_PER_TEST = 3

BAD_BOOST_KILL_DURATION = 5.0 #seconds

__path__ = os.path.dirname(__file__)

import threading

#because boost interrupts are broken in half the versions of boost
#this stupidity makes stock gnuradio apps exit with bad boost
def kill_after_timeout(p):
    time.sleep(BAD_BOOST_KILL_DURATION)
    try: p.kill()
    except: pass

def run_a_single_one(args, env):
    print env
    p = subprocess.Popen(args=args, env=env, stdout=subprocess.PIPE)
    t = threading.Thread(target=kill_after_timeout, args = (p,))
    t.daemon = True
    t.start()
    p.wait()
    out = p.stdout.read()
    #print out
    for line in out.splitlines():
        if line.startswith('##RESULT##'):
            return float(line[len('##RESULT##'):].strip())
    raise Exception, 'no result found!'
    #return t1-t0

def expand_tests(bm):
    for run in bm['tests']:
        if run.has_key('expand') and run['expand']:
            import copy
            new_run = copy.deepcopy(run)
            new_run['wat'] += '\n(Block)'
            new_run['env']['GRAS_YIELD'] = 'BLOCKING'
            yield new_run
            new_run = copy.deepcopy(run)
            new_run['wat'] += '\n(Spin)'
            new_run['env']['GRAS_YIELD'] = 'STRONG'
            yield new_run
            new_run = copy.deepcopy(run)
            new_run['wat'] += '\n(TPB)'
            new_run['env']['GRAS_YIELD'] = 'BLOCKING'
            new_run['env']['GRAS_TPP'] = '1'
            yield new_run
        else: yield run

def do_a_benchmark(bm):
    title = bm['wat']
    print '#'*(len(title)+25)
    print '## running benchmark:', title
    print '#'*(len(title)+25)
    result_means = list()
    result_stddevs = list()
    test_names = list()
    for run in expand_tests(bm):
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
            res = run_a_single_one(args=args, env=env)
            print 'Result:', res
            run_results.append(res)
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
        label=bm['moar']
    )
    ax.set_xlim(-width, max(len(ind), 4))
    ax.set_ylim(0, max(*bogomips)*1.1 + max(*bogomips_dev)*2)
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x()+rect.get_width()/2.0, 1.01*height, '%.3f'%height, horizontalalignment='center')
    ax.grid(True)

    fontP = FontProperties()
    fontP.set_size('small')
    handles, labels = ax.get_legend_handles_labels()
    ax.legend(handles, labels, prop=fontP, loc="upper right")

    print '\n'
    return fig


if __name__ == '__main__':
    ####################################################################
    ## create pdf generator
    ####################################################################
    pdf_pages = PdfPages(sys.argv[1])

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
