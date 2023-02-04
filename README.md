# SweetSourCod

## The sweet source-coding library

This library is a collection of methods to estimate the entropy of sequences by data compression (source coding).

An example of the use of this library can be found in

S. Martiniani, P. Chaikin, D. Levine, "Quantifying hidden order out of equilibrium", [Phys. Rev. X 9, 011031  (2019)](https://journals.aps.org/prx/abstract/10.1103/PhysRevX.9.011031)

S. Martiniani, Y. Lemberg, P. Chaikin, D. Levine, "Correlation lengths in the language of computable information", [Phys. Rev. Lett. 125, 170601 (2020)](https://journals.aps.org/prl/abstract/10.1103/PhysRevLett.125.170601)

A. Cavagna, P. M. Chaikin, D. Levine, S. Martiniani, A. Puglisi, and M. Viale, "Vicsek model by time-interlaced compression: A dynamical computable information density", [Phys. Rev. E 103, 062141 (2021)](https://journals.aps.org/pre/abstract/10.1103/PhysRevE.103.062141)

S. Ro, B. Guo, A. Shih, T. Phan, R. Austin, D. Levine, P. Chaikin, and S. Martiniani, "Model-Free Measurement of Local Entropy Production and Extractable Work in Active Matter", [Phys. Rev. Lett. 129, 220601 (2022)](https://journals.aps.org/prl/abstract/10.1103/PhysRevLett.129.220601)

### Description

The library implements/wraps a number of algorithms and utilities:

- *LZ77*, Lempel-Ziv 77 pattern matching code. Wraps the [kkp2](https://www.cs.helsinki.fi/group/pads/lz77.html) algorithm performing LZ77 factorization in O(n).

- *LZ78*, Lempel-Ziv 78 incremental parsing rule.

- *Cross-parsing complexity*, for computing KL divergence.

- *Block Entropy*, Shannon entropy for blocks of a given size.

- *Block Sorting* (Burrows-Wheeler Transform) based entropy estimator.

- Wraps a range of compression algorithms readily available in Python (after reducing the sequence to its minimal binary representation)
  * deflate
  * bzip2
  * lzma
  * zstd
  * brotli
  * zopfli
  
- *run-length encoding*

- *Hilbert curve*, Hilbert-Peano space filling curve for optimal compression of higher dimensional sequences on a square grid.

- *Gosper curve*, space filling curve for optimal compression of higher dimensional sequences on an hexagonal grid.

### Installation

You can install the library in place by running

```shell
python setup.py build_ext -i
```
