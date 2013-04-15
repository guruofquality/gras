try:
    import gras
    import grextras
except ImportError: pass
import gnuradio
from gnuradio import gr
from gnuradio import blocks as grblocks
import sys

if __name__ == '__main__':

    duration = float(sys.argv[1])
    what = sys.argv[2]

    tb = gr.top_block()
    src0 = gr.null_source(8)
    src1 = gr.null_source(8)
    sink = gr.null_sink(8)

    if what == 'extras_add': math_op = grextras.Add.fc32_fc32()
    if what == 'blocks_add': math_op = grblocks.add_cc()
    if what == 'extras_mult': math_op = grextras.Multiply.fc32_fc32()
    if what == 'blocks_mult': math_op = grblocks.multiply_cc()

    tb.connect(src0, (math_op, 0))
    tb.connect(src1, (math_op, 1))
    tb.connect(math_op, sink)

    import time
    tb.start()
    time.sleep(duration)
    print '##RESULT##', sink.nitems_read(0)/duration
    import sys; sys.stdout.flush()
    tb.stop()
    tb.wait()
