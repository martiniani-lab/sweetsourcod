#ifndef SSC_BLOCK_SORTING_H
#define SSC_BLOCK_SORTING_H

#include <cmath>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <vector>
#include <iterator>
#include <random>
#include <iostream>
#include <sstream>
#include <cstring>
#include <unordered_map>

#include "kkp/divsufsort.h"
#include "sweetsourcod/lempel_ziv.hpp"

namespace ssc 
{

inline std::string burrows_wheeler_transform(std::string& sequence) {
	size_t length = sequence.size();
	std::shared_ptr<unsigned char> text(new unsigned char[length], std::default_delete<unsigned char[]>());
	std::shared_ptr<unsigned char>    u(new unsigned char[length], std::default_delete<unsigned char[]>());

	std::reverse(sequence.begin(), sequence.end());
	memcpy(text.get(), sequence.c_str(), length);
	divbwt(text.get(), u.get(), NULL, length);

	std::string str_bwt(reinterpret_cast<char const*>(u.get()), length);

	return str_bwt;
}
	
// return the Burrows-Wheeler transform of the reversed sequence 
template<class T = long long>
std::vector<unsigned char> burrows_wheeler_transform(const std::vector<T> lattice) {
	std::string sequence = int_vector_to_string<T>(lattice);
	std::string str_bwt = burrows_wheeler_transform(sequence);
	std::vector<unsigned char> vec_bwt(str_bwt.begin(), str_bwt.end());

	return vec_bwt;
}

inline double sumlogp_segment(const std::string& sequence) {
	std::unordered_map<char, std::shared_ptr<size_t>> umap;
	size_t length = sequence.size();

	for (size_t i = 0; i < length; ++i) {
		const char symbol = sequence[i];
		auto it = umap.find(symbol);
		if (it != umap.end()) {
			*(it -> second) += 1;
		} else {
			umap.insert({ symbol, std::make_shared<size_t>(1) });
		}
	}

	double sumlogp = 0;
	for (const auto& symbol : umap) {
		double p = (double)*symbol.second / (double)length;
		sumlogp += (double)*symbol.second * std::log2(p);
	}
	
	return sumlogp;
}

inline double block_sorting_estimator_uniform(std::string& sequence) {
	size_t length = sequence.size();
	size_t seg_len = std::ceil(std::sqrt(length));
	std::string str_seg;
	std::string str_bwt = burrows_wheeler_transform(sequence);

	double entropy = 0;
	
	for (size_t i = 0; i < length; i += seg_len) {
		str_seg.assign(str_bwt, i, seg_len);
		entropy -= sumlogp_segment(str_seg);
	}
	entropy /= length;

	return entropy;
}

//reverse sequence -> BWT -> uniform segmentation -> estimate entropy
template<class T = long long>
double block_sorting_estimator_uniform(const std::vector<T> lattice) {
	std::string sequence = int_vector_to_string<T>(lattice);
	return block_sorting_estimator_uniform(sequence);
}

}
#endif // #ifndef
