from libcpp cimport bool as cbool
from libcpp.vector cimport vector
from libcpp.pair cimport pair
cimport cython
cimport numpy as np
import numpy as np

cdef extern from "sweetsourcod/lempel_ziv.hpp" namespace "ssc":
    cpdef size_t lempel_ziv_complexity78(const vector[long long] lattice) except +
    cpdef size_t lempel_ziv_complexity76(const vector[long long] lattice) except +
    cpdef size_t lempel_ziv_complexity77_kkp(const vector[long long] lattice) except +
    cpdef pair[size_t, double] lempel_ziv_complexity77_sumlog_kkp(const vector[long long] lattice) except +
    cdef vector[vector[int]] get_lz77_factors(const vector[long long] lattice) except +
    
    cpdef size_t cross_parsing(const vector[long long] lattice1, const vector[long long] lattice2) except +
    cpdef pair[size_t, double] cross_parsing_complexity_sumlog(const vector[long long] lattice1, const vector[long long] lattice2) except +
    cdef vector[vector[int]] get_cross_parsing_factors(const vector[long long] lattice1, const vector[long long] lattice2) except +
