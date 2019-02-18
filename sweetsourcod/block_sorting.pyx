# distutils: language = c++

cpdef block_sorting(sequence, seg='uniform'):
    if seg == 'uniform':
        return block_sorting_estimator_uniform(sequence)
    else:
        raise NotImplementedError
