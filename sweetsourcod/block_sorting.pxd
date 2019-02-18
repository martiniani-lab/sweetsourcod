from libcpp cimport bool as cbool
from libcpp.vector cimport vector
cimport cython
cimport numpy as np
import numpy as np

cdef extern from "sweetsourcod/block_sorting.hpp" namespace "ssc":
    cpdef double block_sorting_estimator_uniform(const vector[long long] sequence) except +
