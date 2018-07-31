from __future__ import division
import numpy as np
import zlib
import bz2
import lzma
import zstd
import brotli
import zopfli.zlib
import copy
import string
from itertools import groupby
from sweetsourcod.utils import get_size_bytes, get_mbr_bytes


def get_comp_size_bytes(seq, complevel='HIGHEST_PROTOCOL', dtype='uint8', algorithm='deflate', rle=False):
    assert dtype == 'uint8', 'get_comp_size_bytes requires integer sequence seq'
    assert seq.dtype == dtype
    raw_str = get_mbr_bytes(seq)
    return get_comp_size_bytes_raw(raw_str, complevel=complevel, algorithm=algorithm, rle=rle)


def get_comp_size_bytes_raw(raw_str, complevel='HIGHEST_PROTOCOL', algorithm='deflate', rle=False):
    if rle:
        raw_str = encode_rle(raw_str)
    # compress once
    compr_str1 = zipper_compress(raw_str, complevel=complevel, algorithm=algorithm)
    compr_size1 = get_size_bytes(compr_str1)
    compr_size = get_size_bytes(compr_str1)
    nc = 1
    # try to compress further
    compr_str2 = copy.deepcopy(compr_str1)
    while True:
        compr_str2 = zipper_compress(compr_str2, complevel=complevel, algorithm=algorithm)
        if get_size_bytes(compr_str2) >= compr_size:
            break
        else:
            compr_size = get_size_bytes(compr_str2)
            nc += 1
    return compr_size, nc, compr_size1


def zipper_compress(raw_str, complevel='HIGHEST_PROTOCOL', algorithm='deflate'):
    if algorithm == 'deflate':
        complevel = 9 if complevel == 'HIGHEST_PROTOCOL' else complevel
        return _deflate_compress(raw_str, complevel=complevel)
    elif algorithm == 'bzip2':
        complevel = 9 if complevel == 'HIGHEST_PROTOCOL' else complevel
        return _bzip2_compress(raw_str, complevel=complevel)
    elif algorithm == 'lzma':
        complevel = 9 if complevel == 'HIGHEST_PROTOCOL' else complevel
        return _lzma_compress(raw_str, complevel=complevel)
    elif algorithm == 'zstd':
        complevel = 22 if complevel == 'HIGHEST_PROTOCOL' else complevel
        return _zstd_compress(raw_str, complevel=complevel)
    elif algorithm == 'brotli':
        complevel = 11 if complevel == 'HIGHEST_PROTOCOL' else complevel
        return _brotli_compress(raw_str, complevel=complevel)
    elif algorithm == 'zopfli':
        return _zopfli_compress(raw_str)
    elif algorithm == 'deflate+bzip2':
        complevel = 9 if complevel == 'HIGHEST_PROTOCOL' else complevel
        return _bzip2_compress(_deflate_compress(raw_str, complevel=complevel), complevel=complevel)
    else:
        raise NotImplementedError


def zipper_decompress(bin_str, dtype='uint8', algorithm='deflate'):
    if algorithm == 'deflate':
        return _deflate_decompress(bin_str, dtype=dtype)
    elif algorithm == 'bzip2':
        return _bzip2_decompress(bin_str, dtype=dtype)
    elif algorithm == 'lzma':
        return _lzma_decompress(bin_str, dtype=dtype)
    elif algorithm == 'zstd':
        return _zstd_decompress(bin_str, dtype=dtype)
    elif algorithm == 'brotli':
        return _brotli_decompress(bin_str, dtype=dtype)
    elif algorithm == 'zopfli':
        return _zopfli_decompress(bin_str, dtype=dtype)
    else:
        raise NotImplementedError


def _deflate_compress(raw_str, complevel=9):
    zipper = zlib.compressobj(complevel, zlib.DEFLATED, -15, 9, zlib.Z_DEFAULT_STRATEGY)
    deflated = zipper.compress(raw_str)
    deflated += zipper.flush()
    return deflated


def _deflate_decompress(bin_str, dtype='uint8'):
    intlist = map(ord, zlib.decompress(bin_str, -15))
    return np.array(intlist, dtype=dtype)


def _bzip2_compress(raw_str, complevel=9):
    return bz2.compress(raw_str, complevel)


def _bzip2_decompress(bin_str, dtype='uint8'):
    intlist = map(ord, bz2.decompress(bin_str))
    return np.array(intlist, dtype=dtype)


def _lzma_compress(raw_str, complevel=9):
    zipper = lzma.LZMACompressor(options={'format': 'alone', 'level': complevel, 'extreme': False,
                                          'lc': 3, 'lp': 0, 'pb': 2, 'mode': 2, 'mf': 'bt4',
                                          'nice_len': 273, 'depth': 0, 'dict_size': 67108864})
    compressed = zipper.compress(raw_str)
    compressed += zipper.flush()
    return compressed


def _lzma_decompress(bin_str, dtype='uint8'):
    intlist = map(ord, lzma.decompress(bin_str))
    return np.array(intlist, dtype=dtype)


def _zstd_compress(raw_str, complevel=22):
    cctx = zstd.ZstdCompressor(level=complevel, write_checksum=False, write_content_size=False)
    return cctx.compress(raw_str)


def _zstd_decompress(bin_str, dtype='uint8'):
    dctx = zstd.ZstdDecompressor()
    intlist = map(ord, dctx.decompress(bin_str))
    return np.array(intlist, dtype=dtype)


def _brotli_compress(raw_str, complevel=11, lgwin=24, lgblock=24):
    zipper = brotli.Compressor(mode=brotli.MODE_GENERIC, quality=complevel, lgwin=lgwin, lgblock=lgblock)
    compressed = zipper.process(raw_str)
    compressed += zipper.finish()
    return compressed


def _brotli_decompress(bin_str, dtype='uint8'):
    intlist = map(ord, brotli.decompress(bin_str))
    return np.array(intlist, dtype=dtype)


def _zopfli_compress(raw_str, numiterations=15, blocksplitting=0, blocksplittinglast=1, blocksplittingmax=15):
    compressed = zopfli.zlib.compress(raw_str, numiterations=numiterations, blocksplitting=blocksplitting,
                                      blocksplittinglast=blocksplittinglast, blocksplittingmax=blocksplittingmax)
    return compressed


def _zopfli_decompress(bin_str, dtype='uint8'):
    intlist = map(ord, zlib.decompress(bin_str))
    return np.array(intlist, dtype=dtype)


def encode_rle(input_string):
    """
    run length encoding
    https://www.google.com/patents/EP0734126A1?cl=en
    """
    s = set(input_string)
    assert len(s) <= len(string.ascii_lowercase)
    d = {e: x for x, e in zip(string.ascii_lowercase, sorted(s))}
    lt = [(len(list(g)), d[k]) for k, g in groupby(input_string)]
    rle = ''
    for t in lt:
        rle += str(t[0]) + str(t[1])
    return rle


def decode_rle(compr_string):
    """
    run length decoding
    https://www.google.com/patents/EP0734126A1?cl=en
    """
    lst = []
    d = dict((k, v) for k, v in zip(string.ascii_lowercase, range(len(string.ascii_lowercase))))
    for i in xrange(0, len(compr_string), 2):
        lst.append((int(compr_string[i]), str(d[compr_string[i + 1]])))
    return ''.join(c * n for n, c in lst)
