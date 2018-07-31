from __future__ import division, print_function
import numpy as np
from numba import jit
from sweetsourcod.lempel_ziv import lempel_ziv_complexity
from sweetsourcod.hilbert import get_hilbert_mask
from sweetsourcod.zipper_compress import get_comp_size_bytes
from sweetsourcod.block_entropy import block_entropy

"""
ssimple.py: A simple example on how to use sweetsourcod
"""

__author__ = "Stefano Martiniani"
__copyright__ = "Copyright 2018, sweetsourcod"
__license__ = "GPL"
__maintainer__ = "Stefano Martiniani"
__email__ = "sm7683@nyu.edu"


@jit(nopython=True)
def mask_array(lattice, mask):
    return np.array([lattice[i] for i in mask])


def get_entropy_rate(c, nsites, a=2, method='lz77'):
    """
    :param c: number of longest previous factors (lz77) or unique words (lz78)  
    :param a: convergence rate parameter, need be <= 2
    :param method: lz77 or lz78
    :return: entropy rate h
    """
    if method == 'lz77':
        h = (c * np.log2(c) + 2 * c * np.log2(nsites / c)) / nsites
    elif method == 'lz78':
        h = c * (1. + np.log2(c)) / nsites
    else:
        raise NotImplementedError
    h *= np.log2(nsites) / (np.log2(nsites) + a * np.log2(np.log2(nsites)))
    return h


if __name__ == "__main__":
    # lattice_boxv is the size of the lattice in each direction [Lx, Ly, ...]
    # note that lattice_boxv = image.shape[::-1]
    lattice_boxv = np.array([2 ** 7, 2 ** 7])
    n = np.prod(lattice_boxv)
    image = np.random.randint(0, 2, lattice_boxv[::-1])

    # we need to flatten the image, the raster scan is the simplest option
    image_raster = image.ravel()
    # alternatively we can use a hilbert space filling curve, this requires
    # that each side length is a power of 2
    hilbert_mask = get_hilbert_mask(lattice_boxv)
    image_hilbert = mask_array(image_raster, hilbert_mask).astype('uint8')

    # now with LZ77 we compute the number of longest previous factors c, and the entropy rate h
    # note that lz77 also outputs the mean log word length, that we don't need here
    c_lz77, _ = lempel_ziv_complexity(image_hilbert, 'lz77')
    h_lz77 = get_entropy_rate(c_lz77, n, a=1.5, method='lz77')
    # or the number of unique words using lz78 c, and the entropy rate h
    c_lz78 = lempel_ziv_complexity(image_hilbert, 'lz78')
    h_lz78 = get_entropy_rate(c_lz78, n, a=1, method='lz78')
    # or the length of the encoding obtained by a commercial algorithm, e.g. deflate
    defl_enc, _, _ = get_comp_size_bytes(image_hilbert, complevel='HIGHEST_PROTOCOL', algorithm='deflate')
    h_defl = 8 * defl_enc / image_hilbert.size
    # or the block entropy with block size 6 (computed as be(7) - be(6))
    h_be = block_entropy(image_hilbert, blocksize=6)

    print("h_lz77: {}, h_lz78: {}, h_deflate: {}, h_be: {}".format(h_lz77, h_lz78, h_defl, h_be))
