# distutils: language = c++

cpdef lempel_ziv_complexity(lattice, version='lz77'):
    """
    lattice: array of ints
    version: "lz76", "lz77", "lz78"
    """
    if version == 'lz76':
        return lempel_ziv_complexity76(lattice)
    elif version == 'lz77': #unrestricted
        return lempel_ziv_complexity77_mlwl_kkp(lattice)
    elif version == 'lz78':
        return lempel_ziv_complexity78(lattice)
    else:
        raise NotImplementedError


def lempel_ziv_factors(lattice, version='lz77'):
    if version == 'lz77':
            factors = get_lz77_factors(lattice)
    else:
        raise NotImplementedError
    return factors