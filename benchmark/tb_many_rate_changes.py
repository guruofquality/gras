#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Many Rate Changing
# Generated: Sun Jan 20 15:53:13 2013
##################################################

from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from optparse import OptionParser
import numpy

class many_rate_changing(gr.top_block):

	def __init__(self):
		gr.top_block.__init__(self, "Many Rate Changing")

		##################################################
		# Variables
		##################################################
		self.samp_rate = samp_rate = 32000

		##################################################
		# Blocks
		##################################################
		self.random_source_x_0 = gr.vector_source_b(map(int, numpy.random.randint(0, 256, 1000)), True)
		self.gr_unpacked_to_packed_xx_0 = gr.unpacked_to_packed_bb(2, gr.GR_LSB_FIRST)
		self.gr_packed_to_unpacked_xx_0 = gr.packed_to_unpacked_bb(2, gr.GR_MSB_FIRST)
		self.gr_null_sink_0_2 = gr.null_sink(gr.sizeof_char*1)
		self.blocks_float_to_char_0 = blocks.float_to_char(1, 1)
		self.blocks_char_to_float_0 = blocks.char_to_float(1, 1)

		##################################################
		# Connections
		##################################################
		self.connect((self.blocks_char_to_float_0, 0), (self.blocks_float_to_char_0, 0))
		self.connect((self.blocks_float_to_char_0, 0), (self.gr_packed_to_unpacked_xx_0, 0))
		self.connect((self.gr_unpacked_to_packed_xx_0, 0), (self.blocks_char_to_float_0, 0))
		self.connect((self.random_source_x_0, 0), (self.gr_unpacked_to_packed_xx_0, 0))
		self.connect((self.gr_packed_to_unpacked_xx_0, 0), (self.gr_null_sink_0_2, 0))


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
	(options, args) = parser.parse_args()
	tb = many_rate_changing()
	import time
	tb.start()
	time.sleep(options.dur)
	print '##RESULT##', tb.gr_null_sink_0_2.nitems_read(0)/options.dur
	import sys; sys.stdout.flush()
	tb.stop()
	tb.wait()

