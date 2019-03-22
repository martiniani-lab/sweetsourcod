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
	std::shared_ptr<unsigned char> text(new unsigned char[length], std::default_delete<unsigned char[]>());
	std::shared_ptr<unsigned char>    u(new unsigned char[length], std::default_delete<unsigned char[]>());
	
	memcpy(text.get(), sequence.c_str(), length);
	divbwt(text.get(), u.get(), NULL, length);

	double entropy = 0;
	std::string str_seg;
	std::string str_bwt(reinterpret_cast<char const*>(u.get()), length);
	
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
	std::reverse(sequence.begin(), sequence.end());
	
	return block_sorting_estimator_uniform(sequence);
}

}
#endif // #ifndef
