# distutils: language = c++

cpdef get_gosper_distance(pt, n):
    return gosper_coord2distance(pt, n)

