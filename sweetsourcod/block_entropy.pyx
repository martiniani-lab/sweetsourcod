# distutils: language = c++

def block_entropy(sequence, blocksize=6):
    return block_entropy_cpp(sequence, blocksize + 1) - block_entropy_cpp(sequence, blocksize)