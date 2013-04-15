import gnuradio
from gnuradio import gr
from gnuradio import blocks as grblocks
import sys

if __name__ == '__main__':

    duration = float(sys.argv[1])

    tb = gr.top_block()
    src0 = gr.null_source(8)
    src1 = gr.null_source(8)
    addr01 = grblocks.add_cc()
    src2 = gr.null_source(8)
    src3 = gr.null_source(8)
    addr23 = grblocks.add_cc()
    mult03 = grblocks.multiply_cc()
    sink = gr.null_sink(8)

    tb.connect(src0, (addr01, 0))
    tb.connect(src1, (addr01, 1))
    tb.connect(src2, (addr23, 0))
    tb.connect(src3, (addr23, 1))

    tb.connect(addr01, (mult03, 0))
    tb.connect(addr23, (mult03, 1))

    tb.connect(mult03, sink)

    import time
    tb.start()
    time.sleep(duration)
    print '##RESULT##', sink.nitems_read(0)/duration
    import sys; sys.stdout.flush()
    tb.stop()
    tb.wait()
