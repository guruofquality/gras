import gras
import grextras
import gnuradio
from gnuradio import gr
from gnuradio import blocks as grblocks
import sys

if __name__ == '__main__':

    num = long(float(sys.argv[1]))
    what = sys.argv[2]

    tb = gras.TopBlock()
    src0 = gr.null_source(8)
    src1 = gr.null_source(8)
    sink = gr.null_sink(8)
    head = gr.head(8, num)

    if what == 'extras_add': math_op = grextras.Add.fc32_fc32()
    if what == 'blocks_add': math_op = grblocks.add_cc()
    if what == 'extras_mult': math_op = grextras.Multiply.fc32_fc32()
    if what == 'blocks_mult': math_op = grblocks.multiply_cc()

    tb.connect(src0, (math_op, 0))
    tb.connect(src1, (math_op, 1))
    tb.connect(math_op, head, sink)

    tb.run()
    tb.wait()
