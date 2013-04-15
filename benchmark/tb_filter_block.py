#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Filter Test
# Generated: Mon Jan 14 01:39:57 2013
##################################################

from gnuradio import blks2
from gnuradio import filter
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from optparse import OptionParser

class filter_test(gr.top_block):

    def __init__(self, which=''):
        gr.top_block.__init__(self, "Filter Test")

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 32000

        ##################################################
        # Blocks
        ##################################################
        self.gr_null_source_0 = gr.null_source(gr.sizeof_gr_complex*1)
        self.gr_null_sink_0 = gr.null_sink(gr.sizeof_gr_complex*1)
        self.blks2_rational_resampler_xxx_0 = blks2.rational_resampler_ccc(
            interpolation=3,
            decimation=7,
            taps=None,
            fractional_bw=None,
        )
        self.fir_filter_xxx_0 = filter.fir_filter_ccc(7, ([.7]*23))

        if which == 'dfir': self.filter = self.fir_filter_xxx_0
        if which == 'resamp': self.filter = self.blks2_rational_resampler_xxx_0

        ##################################################
        # Connections
        ##################################################
        self.connect((self.filter, 0), (self.gr_null_sink_0, 0))
        self.connect((self.gr_null_source_0, 0), (self.filter, 0))

# QT sink close method reimplementation

    def get_num(self):
        return self.num

    def set_num(self, num):
        self.num = num

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate

if __name__ == '__main__':
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option("", "--dur", dest="dur", type="eng_float", default=eng_notation.num_to_str(1.0),
        help="Set num [default=%default]")
    parser.add_option("", "--which", dest="which", type="string", default='')
    (options, args) = parser.parse_args()
    tb = filter_test(which=options.which)
    import time
    tb.start()
    time.sleep(options.dur)
    print '##RESULT##', tb.gr_null_sink_0.nitems_read(0)/options.dur
    import sys; sys.stdout.flush()
    tb.stop()
    tb.wait()

