try:
    import gras
    import grextras
except ImportError: pass
import gnuradio
from gnuradio import gr
import sys

if __name__ == '__main__':

    num = long(float(sys.argv[1]))
    what = sys.argv[2]

    tb = gr.top_block()
    src0 = gr.null_source(8)
    sink = gr.null_sink(8)
    head = gr.head(8, num)

    if what == 'extras_delay': delay_block = grextras.Delay(8)
    if what == 'core_delay': delay_block = gr.delay(8, 0)
    delay_block.set_delay(1000)

    tb.connect(src0, (delay_block, 0))
    tb.connect(delay_block, head, sink)

    tb.run()
    tb.wait()
