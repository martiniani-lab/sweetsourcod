# distutils: language = c++
cimport numpy as np
import numpy as np
from libc.stdlib cimport strtoul
from libcpp.string cimport string
from libcpp.vector cimport vector

"""
on hilbert curves
https://corte.si/posts/code/hilbert/portrait/index.html
https://people.sc.fsu.edu/~jburkardt/cpp_src/hilbert_curve/hilbert_curve.html

code adapted from https://github.com/galtay/hilbert_curve/blob/master/hilbert.py

This is a module to convert between one dimensional distance along a
`Hilbert curve`_, :math:`h`, and N-dimensional coordinates,
:math:`(x_0, x_1, ... x_N)`.  The two important parameters are :math:`N`
(the number of dimensions, must be > 0) and :math:`p` (the number of
iterations used in constructing the Hilbert curve, must be > 0).
We consider an N-dimensional `hypercube`_ of side length :math:`2^p`.
This hypercube contains :math:`2^{N p}` unit hypercubes (:math:`2^p` along
each dimension).  The number of unit hypercubes determine the possible
discrete distances along the Hilbert curve (indexed from :math:`0` to
:math:`2^{N p} - 1`).  The image below illustrates the situation for
:math:`N=2` and :math:`p=3`.
.. figure:: nD=2_p=3.png
   This is the third iteration (:math:`p=3`) of the Hilbert curve in two
   (:math:`N=2`) dimensions.  Distances, :math:`h`, along the curve are
   labeled from 0 to 63 (i.e. from 0 to :math:`2^{N p}-1`).  The provided
   functions translate between N-dimensional coordinates and the one
   dimensional distance.  For example, between (:math:`x_0=4, x_1=6`) and
   :math:`h=36`.
Reference
=========
This module is based on the C code provided in the 2004 article
"Programming the Hilbert Curve" by John Skilling,
  - http://adsabs.harvard.edu/abs/2004AIPC..707..381S
I was also helped by the discussion in the following stackoverflow post,
  - `mapping-n-dimensional-value-to-a-point-on-hilbert-curve`_
which points out a typo in the source code of the paper.  The Skilling code
provides two functions ``TransposetoAxes`` and ``AxestoTranspose``.  In this
case, Transpose refers to a specific packing of the integer that represents
distance along the Hilbert curve (see below for details) and
Axes refer to the N-dimensional coordinates.  Below is an excerpt of the docs
from that code that appears in the paper by Skilling, ::
//+++++++++++++++++++++++++++ PUBLIC-DOMAIN SOFTWARE ++++++++++++++++++++++++++
// Functions: TransposetoAxes  AxestoTranspose
// Purpose:   Transform in-place between Hilbert transpose and geometrical axes
// Example:   b=5 bits for each of n=3 coordinates.
//            15-bit Hilbert integer = A B C D E F G H I J K L M N O is stored
//            as its Transpose
//                   X[0] = A D G J M                X[2]|
//                   X[1] = B E H K N    <------->       | /X[1]
//                   X[2] = C F I L O               axes |/
//                          high  low                    0------ X[0]
//            Axes are stored conveniently as b-bit integers.
// Author:    John Skilling  20 Apr 2001 to 11 Oct 2003
.. _Hilbert curve: https://en.wikipedia.org/wiki/Hilbert_curve
.. _hypercube: https://en.wikipedia.org/wiki/Hypercube
.. _mapping-n-dimensional-value-to-a-point-on-hilbert-curve: http://stackoverflow.com/questions/499166/mapping-n-dimensional-value-to-a-point-on-hilbert-curve/10384110#10384110
"""

cdef _binary_repr(int num, int width):
    """Return a binary string representation of `num` zero padded to `width`
    bits."""
    return format(num, 'b').zfill(width)

cdef _hilbert_integer_to_transpose(int h, int p, int N):
    """Store a hilbert integer (`h`) as its transpose (`x`).
    :param h: integer distance along hilbert curve
    :type h: ``int``
    :param p: number of iterations in Hilbert curve
    :type p: ``int``
    :param N: number of dimensions
    :type N: ``int``
    """
    cdef int i
    cdef bytes h_bit_str = _binary_repr(h, p*N)
    # x = [int(h_bit_str[i::N], 2) for i in range(N)]
    cdef np.ndarray[int, ndim=1] x = np.empty(N, dtype='int32')
    for i in xrange(N):
        x[i] = int(h_bit_str[i::N], 2)
    return x

def _transpose_to_hilbert_integer(np.ndarray[int, ndim=1] x, int p, int N):
    """Restore a hilbert integer (`h`) from its transpose (`x`).
    :param x: the transpose of a hilbert integer (N components of length p)
    :type x: ``list`` of ``int``
    :param p: number of iterations in hilbert curve
    :type p: ``int``
    :param N: number of dimensions
    :type N: ``int``
    """
    cdef int i, j, h
    cdef string y, hstr
    # x_bit_str = [_binary_repr(x[i], p) for i in xrange(N)]
    # h = int(''.join([y[i] for i in xrange(p) for y in x_bit_str]), 2)
    cdef vector[string] x_bit_str
    for i in xrange(N):
        x_bit_str.push_back(_binary_repr(x[i], p))
    hstr = ''
    for i in xrange(p):
        for j in xrange(N):
            y = x_bit_str[j]
            hstr += y[i]
    h = strtoul(hstr.c_str(), NULL, 2)
    return h

def coordinates_from_distance(int h, int p, int N):
    """Return the coordinates for a given hilbert distance.
    :param h: integer distance along the curve
    :type h: ``int``
    :param p: side length of hypercube is 2^p
    :type p: ``int``
    :param N: number of dimensions
    :type N: ``int``
    """
    cdef int i, t, Q, P, Z
    cdef np.ndarray[int, ndim=1] x = _hilbert_integer_to_transpose(h, p, N)
    Z = 2 << (p-1)

    # Gray decode by H ^ (H/2)
    t = x[N-1] >> 1
    for i in xrange(N-1, 0, -1):
        x[i] ^= x[i-1]
    x[0] ^= t

    # Undo excess work
    Q = 2
    while Q != Z:
        P = Q - 1
        for i in xrange(N-1, -1, -1):
            if x[i] & Q:
                # invert
                x[0] ^= P
            else:
                # excchange
                t = (x[0] ^ x[i]) & P
                x[0] ^= t
                x[i] ^= t
        Q <<= 1
    # done
    return x

def distance_from_coordinates(xlist, int p, int N):
    """Return the hilbert distance for a given set of coordinates.
    :param x: coordinates len(x) = N
    :type x: ``list`` of ``int``
    :param p: side length of hypercube is 2^p
    :type p: ``int``
    :param N: number of dimensions
    :type N: ``int``
    """
    cdef int i, M, Q, P, t, h
    cdef np.ndarray[int, ndim=1] x = np.array(xlist, dtype='int32')
    M = 1 << (p - 1)

    # Inverse undo excess work
    Q = M
    while Q > 1:
        P = Q - 1
        for i in xrange(N):
            if x[i] & Q:
                x[0] ^= P
            else:
                t = (x[0] ^ x[i]) & P
                x[0] ^= t
                x[i] ^= t
        Q >>= 1

    # Gray encode
    for i in xrange(1, N):
        x[i] ^= x[i-1]
    t = 0
    Q = M
    while Q > 1:
        if x[N-1] & Q:
            t ^= Q - 1
        Q >>= 1
    for i in xrange(N):
        x[i] ^= t

    h = _transpose_to_hilbert_integer(x, p, N)
    return h


def is_power2(num):
    """states if a number is a power of two"""
    return num != 0 and ((num & (num - 1)) == 0)


def get_hilbert_scan(lattice, lattice_boxv):
    cdef int ndim, size, distance, p, x, y, i, j
    ndim = lattice_boxv.size
    cdef np.ndarray[int, ndim=1] lattice_shape = np.array(lattice_boxv[::-1], dtype='int32')
    cdef np.ndarray[int, ndim=1] scanned_lattice = np.empty(lattice.size, dtype='int32')
    if not is_power2(np.amax(lattice_boxv)):
        raise NotImplementedError, "Max lattice size is not a power of 2,"
    size = np.amax(lattice_boxv)
    p = int(np.log2(size))
    distance = int(size ** ndim)
    rlattice = lattice.reshape(lattice_shape)
    j = 0
    if ndim == 2:
        for i in xrange(distance):
            x, y = coordinates_from_distance(i, p, ndim)
            if x < lattice_shape[1] and y < lattice_shape[0]:
                scanned_lattice[j] = rlattice[y, x]
                j += 1
    elif ndim == 3:
        for i in xrange(distance):
            x, y, z = coordinates_from_distance(i, p, ndim)
            if x < lattice_shape[2] and y < lattice_shape[1] and z < lattice_shape[0]:
                scanned_lattice[j] = rlattice[z, y, x]
                j += 1
    else:
        raise NotImplementedError
    return scanned_lattice


def get_hilbert_mask(lattice_boxv):
    cdef int ndim, size, distance, p, x, y, i, j
    ndim = lattice_boxv.size
    cdef np.ndarray[int, ndim=1] lattice_shape = np.array(lattice_boxv[::-1], dtype='int32')
    cdef np.ndarray[int, ndim=1] mask = np.empty(np.prod(lattice_shape), dtype='int32')
    if not is_power2(np.amax(lattice_boxv)):
        raise NotImplementedError, "Max lattice size is not a power of 2,"
    size = np.amax(lattice_boxv)
    p = int(np.log2(size))
    distance = int(size ** ndim)
    j = 0
    if ndim == 2:
        for i in xrange(distance):
            x, y = coordinates_from_distance(i, p, ndim)
            if x < lattice_shape[1] and y < lattice_shape[0]:
                mask[j] = x + y * lattice_boxv[0]
                j += 1
    elif ndim == 3:
        for i in xrange(distance):
            x, y, z = coordinates_from_distance(i, p, ndim)
            if x < lattice_shape[2] and y < lattice_shape[1] and z < lattice_shape[0]:
                mask[j] = x + y * lattice_boxv[0] + z * lattice_boxv[0] * lattice_boxv[1]
                j += 1
    else:
        raise NotImplementedError
    return mask