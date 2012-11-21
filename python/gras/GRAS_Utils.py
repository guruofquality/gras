# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import numpy

def pointer_to_ndarray(addr, nitems, dtype=numpy.dtype(numpy.uint8), readonly=False):
    class array_like:
        __array_interface__ = {
            'data' : (addr, readonly),
            'typestr' : dtype.base.str,
            'descr' : dtype.base.descr,
            'shape' : (nitems,) + dtype.shape,
            'strides' : None,
            'version' : 3,
        }
    return numpy.asarray(array_like()).view(dtype.base)
