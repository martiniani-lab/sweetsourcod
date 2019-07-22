from __future__ import division, print_function
import numpy as np
from numba import jit
from sweetsourcod.lempel_ziv import lempel_ziv_complexity
from sweetsourcod.hilbert import get_hilbert_mask
from sweetsourcod.zipper_compress import get_comp_size_bytes
from sweetsourcod.block_entropy import block_entropy
from PIL import Image

"""
ssimple.py: A simple example on how to use sweetsourcod
"""

__author__ = "Stefano Martiniani"
__copyright__ = "Copyright 2019, sweetsourcod"
__license__ = "GPL"
__maintainer__ = "Stefano Martiniani"
__email__ = "sm7683@nyu.edu"


@jit(nopython=True)
def mask_array(lattice, mask):
    return np.array([lattice[i] for i in mask])


def _get_entropy_rate(c, nsites, norm=1, alphabetsize=2, method='lz77'):
    """
    :param c: number of longest previous factors (lz77) or unique words (lz78)  
    :param norm: normalization constant, usually the filesize per character of a random binary sequence of the same length
    :param method: lz77 or lz78
    :return: entropy rate h
    """
    if method == 'lz77':
        h = (c * np.log2(c) + 2 * c * np.log2(nsites / c)) / nsites
        h /= norm
    elif method == 'lz78':
        h = c * (np.log2(alphabetsize) + np.log2(c)) / nsites
        h /= norm
    else:
        raise NotImplementedError
    return h


def get_entropy_rate_lz77(x, extrapolate=True):
    # now with LZ77 we compute the number of longest previous factors c, and the entropy rate h
    # note that lz77 also outputs the sum of the logs of the factors which is approximately 
    # equal to the compressed size of the file
    nsites = len(x)
    if extrapolate:
        random_binary_sequence = np.random.randint(0, 2, nsites, dtype='uint8')
        c_bin, sumlog_bin = lempel_ziv_complexity(random_binary_sequence, 'lz77')
        h_bound_bin = _get_entropy_rate(c_bin, nsites, norm=1, alphabetsize=np.unique(x).size, method='lz77')
        h_sumlog_bin = sumlog_bin
    else:
        h_bin = 1
        h_sumlog_bin = nsites
    c, h_sumlog = lempel_ziv_complexity(x, 'lz77')
    h_bound = _get_entropy_rate(c, nsites, norm=h_bound_bin, alphabetsize=np.unique(x).size, method='lz77')
    h_sumlog /= h_sumlog_bin
    return h_bound, h_sumlog


def get_entropy_rate_lz78(x, extrapolate=True):
    nsites = len(x)
    if extrapolate:
        random_binary_sequence = np.random.randint(0, 2, nsites, dtype='uint8')
        c_bin = lempel_ziv_complexity(random_binary_sequence, 'lz78')
        h_bound_bin = _get_entropy_rate(c_bin, nsites, norm=1, alphabetsize=np.unique(x).size, method='lz78')
    else:
        h_bin = 1
    c = lempel_ziv_complexity(x, 'lz78')
    h_bound = _get_entropy_rate(c, nsites, norm=h_bound_bin, alphabetsize=np.unique(x).size, method='lz78')
    return h_bound


def get_entropy_rate_bbox(x, extrapolate=True, algorithm='deflate', **kwargs):
    nsites = len(x)
    if extrapolate:
        random_binary_sequence = np.random.randint(0, 2, nsites, dtype='uint8')
        enc, _, _ = get_comp_size_bytes(random_binary_sequence, algorithm=algorithm, **kwargs)
        h_bin = 8 * enc / nsites
    else:
        h_bin = 1
    enc, _, _ = get_comp_size_bytes(x, algorithm=algorithm, **kwargs)
    h = 8 * enc / nsites
    return h / h_bin


if __name__ == "__main__":
    # image taken from wikipedia
    image = Image.open("Samantha_Cristoforetti_official_portrait_in_an_EMU_spacesuit.jpg").convert('1') # open colour image, convert to b&w
    image = np.asarray(image, dtype='uint8')
    # if you want to see what you are compressing uncomment the next line
    # Image.fromarray(image*255).show()

    # lattice_boxv is the size of the lattice in each direction [Lx, Ly, ...]
    # note that lattice_boxv = image.shape[::-1]
    lattice_boxv = np.asarray(image.shape[::-1])
    n = np.prod(lattice_boxv) 
    
    # we need to flatten the image, choose between raster and hilbert
    scan = 'hilbert'

    if scan == 'raster':
        # the raster scan is the simplest option
        image_flat = image.ravel()
    elif scan == 'hilbert':
        # alternatively we can use a hilbert space filling curve, this requires
        # that each side length is a power of 2, so take a square section of the image so that we can scan it using a hilbert scan
        image = image[:2**9, :2**9]
        lattice_boxv = np.asarray(image.shape[::-1])
        n = np.prod(lattice_boxv)    
        hilbert_mask = get_hilbert_mask(lattice_boxv)
        image_flat = mask_array(image.ravel(), hilbert_mask).astype('uint8')
    else:
        raise NotImplementedError

    # get entropy rate from lz77 factorization
    h_lz77_bound, h_lz77_sumlog = get_entropy_rate_lz77(image_flat)
    # get entropy rate from lz78 factorization
    h_lz78 = get_entropy_rate_lz78(image_flat)
    # get entropy rate from black box utilities
    h_bbox = get_entropy_rate_bbox(image_flat, algorithm='deflate', complevel='HIGHEST_PROTOCOL')
    # or the block entropy with block size 6 (computed as be(7) - be(6))
    h_be = block_entropy(image_flat, blocksize=6)

    # print the extrapolated values for the entropy rates
    print("h_lz77_sumlog: {}, h_lz77: {}, h_lz78: {}, h_bbox: {}, h_be: {}".format(h_lz77_sumlog, h_lz77_bound, h_lz78, h_bbox, h_be))
