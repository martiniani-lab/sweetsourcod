# SweetSourCod

## The sweet source coding library

This library is a collection of methods to estimate the entropy of sequences by data compression (source coding).

An example of the use of this library can be found in

S. Martiniani, P. Chaikin, D. Levine, "Quantifying hidden order out of equilibrium", [Phys. Rev. X 9, 011031  (2019)](https://journals.aps.org/prx/abstract/10.1103/PhysRevX.9.011031)

S. Martiniani, Y. Lemberg, P. Chaikin, D. Levine, "Correlation lengths in the language of computable information", [ 	arXiv:2004.03502 (2020)](https://arxiv.org/abs/2004.03502)

### Description

The library implements/wraps a number of algorithms and utilities:

- *LZ77*, Lempel-Ziv 77 pattern matching code. Wraps the [kkp2](https://www.cs.helsinki.fi/group/pads/lz77.html) algorithm performing LZ77 factorization in O(n).

- *LZ78*, Lempel-Ziv 78 incremental parsing rule.

- *Block Entropy*, Shannon entropy for blocks of a given size.

- Wraps a range of compression algorithms readily available in Python (after reducing the sequence to its minimal binary representation)
  * deflate
  * bzip2
  * lzma
  * zstd
  * brotli
  * zopfli
  
- *run-length encoding*

- *Hilbert curve*, Hilbert-Peano space filling curve for optimal compression of higher dimensional sequences.

### Installation

You can install the library in place by running

```shell
python setup.py build_ext -i
```
