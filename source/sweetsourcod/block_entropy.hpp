#ifndef SSC_BLOCK_ENTROPY_H
#define SSC_BLOCK_ENTROPY_H

#include <cmath>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <vector>
#include <iterator>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "sweetsourcod/lempel_ziv.hpp"

namespace ssc{

inline double block_entropy_cpp(const std::string& sequence, const size_t blocksize=6){
    std::unordered_map<std::string, std::shared_ptr<size_t>> umap;
    size_t nblocks = sequence.size() - blocksize + 1;
    
    for (size_t i=0; i<nblocks; ++i){
        const std::string block = sequence.substr(i, blocksize);
        auto it = umap.find(block);
        if (it != umap.end()){
            *(it->second) += 1;
        }
        else{
            umap.insert({block, std::make_shared<size_t>(1)});
        }
    }

    double ent = 0.;
    for(const auto& block : umap){
        double p = (double) *block.second / (double) nblocks;
        ent -= p * std::log2(p);
    }
    return ent;
}

template<class T=long long>
double block_entropy_cpp(const std::vector<T> lattice, const size_t blocksize=6){
    std::string sequence = int_vector_to_string<T>(lattice);
    return block_entropy_cpp(sequence, blocksize);
}

}
#endif // #ifndef