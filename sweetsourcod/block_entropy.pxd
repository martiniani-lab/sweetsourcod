from libcpp cimport bool as cbool
from libcpp.vector cimport vector
cimport cython
cimport numpy as np
import numpy as np

cdef extern from "sweetsourcod/block_entropy.hpp" namespace "ssc":
    cpdef double block_entropy_cpp(const vector[long long] sequence, size_t blocksize) except +