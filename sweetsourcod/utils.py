from __future__ import division, absolute_import, print_function
import numpy as np
import sys
import bitarray


def seq_to_str(seq):
    return ''.join(map(str, seq))


def get_size_bytes(s):
    return sys.getsizeof(s) - sys.getsizeof('')


def get_bytes(seq, nbits):
    """
    get minimal binary representation (mbr) string of bytes
    https://stackoverflow.com/questions/13676183/python-choose-number-of-bits-to-represent-binary-number
    """
    assert nbits >= np.ceil(np.log2(np.amax(seq) + 1)), "nbits not sufficient to represent configuration"
    fsyntax = '0{}b'.format(int(nbits))
    ba = bitarray.bitarray(''.join([format(x, fsyntax) for x in seq]))
    return ba.tobytes()


def get_mbr_bytes(seq):
    """
    get minimal binary representation (mbr) string of bytes
    https://stackoverflow.com/questions/13676183/python-choose-number-of-bits-to-represent-binary-number
    """
    nbits = np.ceil(np.log2(np.amax(seq) + 1))
    return get_bytes(seq, nbits)
