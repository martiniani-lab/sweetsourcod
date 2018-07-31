**SweetSourCod**
=====================================================================

The sweet source coding library
----------------------------------------------------------------------

This library is a collection of methods to estimate the entropy of sequences by data compression (source coding).

An example of the use of this library can be found in

S. Martiniani, P. Chaikin, D. Levine, "Quantifying hidden order out of equilibrium", [arXiv preprint: 1708.04993](https://arxiv.org/abs/1708.04993)

The library implements/wraps a number of algorithms and utilities:

- *LZ77*, Lempel-Ziv 77 pattern matching code. Wraps the [https://www.cs.helsinki.fi/group/pads/lz77.html](kkp2) performing LZ77 factorization in O(n).

- *LZ78*, Lempel-Ziv incremental parsing rule.

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
