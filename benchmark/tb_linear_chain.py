import gnuradio
from gnuradio import gr
from gnuradio import blocks as grblocks
import sys

if __name__ == '__main__':

    duration = float(sys.argv[1])

    tb = gr.top_block()
    src = gr.null_source(8)
    b0 = gr.copy(8)
    b1 = grblocks.sub_cc()
    b2 = gr.copy(8)
    b3 = grblocks.divide_cc()
    b4 = gr.copy(8)
    sink = gr.null_sink(8)

    tb.connect(src, b0, b1, b2, b3, b4, sink)

    import time
    tb.start()
    time.sleep(duration)
    print '##RESULT##', sink.nitems_read(0)/duration
    import sys; sys.stdout.flush()
    tb.stop()
    tb.wait()
