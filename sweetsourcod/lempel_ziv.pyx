# distutils: language = c++

cpdef lempel_ziv_complexity(lattice, version='lz77'):
    """
    lattice: array of ints
    version: "lz76", "lz77", "lz78"
    """
    if version == 'lz76':
        return lempel_ziv_complexity76(lattice)
    elif version == 'lz77': #unrestricted
        return lempel_ziv_complexity77_sumlog_kkp(lattice)
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
    

cpdef cross_parsing_complexity(lattice1, lattice2):
    return cross_parsing_complexity_sumlog(lattice1, lattice2)

def cross_parsing_factors(lattice1, lattice2):
    return get_cross_parsing_factors(lattice1, lattice2)

