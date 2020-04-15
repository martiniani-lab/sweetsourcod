# distutils: language = c++
import numpy as np

cpdef block_sorting(sequence, seg='uniform'):
    if seg == 'uniform':
        return block_sorting_estimator_uniform(sequence)
    else:
        raise NotImplementedError

def bwt_sequence(sequence):
    return np.array(burrows_wheeler_transform(sequence)).astype('uint8')
