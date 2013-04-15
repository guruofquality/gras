try:
    import gras
    import grextras
except ImportError: pass
import gnuradio
from gnuradio import gr
import sys

if __name__ == '__main__':

    duration = float(sys.argv[1])
    what = sys.argv[2]

    tb = gr.top_block()
    src0 = gr.null_source(8)
    sink = gr.null_sink(8)

    if what == 'extras_delay': delay_block = grextras.Delay(8)
    if what == 'core_delay': delay_block = gr.delay(8, 0)
    delay_block.set_delay(1000)

    tb.connect(src0, (delay_block, 0))
    tb.connect(delay_block, sink)

    import time
    tb.start()
    time.sleep(duration)
    print '##RESULT##', sink.nitems_read(0)/duration
    import sys; sys.stdout.flush()
    tb.stop()
    tb.wait()
