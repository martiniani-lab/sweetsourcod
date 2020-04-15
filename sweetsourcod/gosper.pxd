from libcpp cimport bool as cbool
from libcpp.pair cimport pair
from libcpp.vector cimport vector
cimport cython
cimport numpy as np
import numpy as np

cdef extern from "sweetsourcod/gosper_curve.hpp" namespace "ssc":
    cpdef long long gosper_coord2distance(const pair[double, double] pt, const int n) except +

