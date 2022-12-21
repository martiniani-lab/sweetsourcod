#ifndef SSC_LEMPEL_ZIV_H
#define SSC_LEMPEL_ZIV_H

#include <cmath>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <vector>
#include <iterator>
#include <random>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <cstring>

#include "kkp/kkp.h"
#include "kkp/divsufsort.h"

namespace ssc
{

template<class T=long long>
std::string int_vector_to_string(const std::vector<T>& v){
    std::string ss;
    ss.reserve(v.size());
    for (const auto& x: v){
        if (x < 0) {throw std::runtime_error("int_vector_to_string only accepts sequences of positive values");}
        if (x > 255) {throw std::runtime_error("x>255, exceeded ascii table");}
        ss.push_back(static_cast<int>(x));
    }
    return ss;
}

template<class T=long long>
std::string int_vector_to_string_cp(const std::vector<T>& v){
    std::string ss;
    ss.reserve(v.size());
    for (const auto& x: v){
        if (x < 0) {throw std::runtime_error("int_vector_to_string_cp only accepts sequences of positive values");}
        if (x > 254) {throw std::runtime_error("x>254, exceeded maximum value for cross-parsing");}
        ss.push_back(static_cast<int>(x+1));
    }
    return ss;
}

inline size_t lempel_ziv_complexity78(const std::string& sequence){
    bool remainder = false;
    std::unordered_set<std::string> lz_factors;
    size_t i = 0;

    for (size_t j=1; j<=sequence.size(); ++j){
        std::string factor = sequence.substr(i, j-i);
        remainder = true;
        if (lz_factors.count(factor) == 0){
            lz_factors.insert(factor);
            i = j;
            remainder = false;
        }
    }

    if (remainder){
        return lz_factors.size() + 1;
    }
    else{
        return lz_factors.size();
    }
}

template<class T=long long>
size_t lempel_ziv_complexity78(const std::vector<T> lattice){
    std::string sequence = int_vector_to_string<T>(lattice);
    return lempel_ziv_complexity78(sequence);
}

inline size_t lempel_ziv_complexity76(const std::string& sequence){
    std::string pastseq, factor;
    std::unordered_set<std::string> lz_factors;
    size_t i = 1;
    lz_factors.insert(sequence.substr(0, 1));

    for (size_t j=2; j<=sequence.size(); ++j){
        pastseq = sequence.substr(0, j-1);
        factor = sequence.substr(i, j-i);
        if (pastseq.find(factor) == std::string::npos){
            lz_factors.insert(factor);
            i = j;
        }
    }

    if (pastseq.find(factor) != std::string::npos){
        return lz_factors.size() + 1;
    }
    else{
        return lz_factors.size();
    }
}

template<class T=long long>
size_t lempel_ziv_complexity76(const std::vector<T> lattice){
    std::string sequence = int_vector_to_string<T>(lattice);
    return lempel_ziv_complexity76(sequence);
}

inline size_t lempel_ziv_complexity77_kkp(std::string& sequence, std::vector<std::pair<int, int>>* factorsp=NULL){
    // https://www.cs.helsinki.fi/group/pads/lz77.html#ref1
    // https://stackoverflow.com/questions/43631415/using-shared-ptr-with-char
    const int length = sequence.size();
    std::shared_ptr<unsigned char> text(new unsigned char[length], std::default_delete<unsigned char[]>());
    std::shared_ptr<int> sa(new int[length+2], std::default_delete<int[]>());
    memcpy(text.get(), sequence.c_str(), length);
    divsufsort(text.get(), sa.get(), length);
    int nphrases = kkp2(text.get(), sa.get(), length, factorsp); //kkp3 has isssues with large arrays
    return nphrases;
}

template<class T=long long>
size_t lempel_ziv_complexity77_kkp(const std::vector<T> lattice, std::vector<std::pair<int, int>> &factors){
    std::string sequence = int_vector_to_string<T>(lattice);
    return lempel_ziv_complexity77_kkp(sequence, &factors);
}

template<class T=long long>
size_t lempel_ziv_complexity77_kkp(const std::vector<T> lattice){
    std::string sequence = int_vector_to_string<T>(lattice);
    return lempel_ziv_complexity77_kkp(sequence, NULL);
}

template<class T=long long>
std::vector<std::vector<int>> get_lz77_factors(const std::vector<T> lattice){
    std::string sequence = int_vector_to_string<T>(lattice);
    std::vector<std::pair<int, int>> factors;
    size_t nfactors = lempel_ziv_complexity77_kkp(sequence, &factors);
    if (nfactors != factors.size()){throw std::runtime_error("nfactors and factors.size do no match");}
    std::vector<std::vector<int>> v;
    for (const auto &x : factors){
        v.push_back({x.first, x.second});
    }
    return v;
}

//returns complexity and compressed file size up to loglog corrections
template<class T=long long>
std::pair<size_t, double> lempel_ziv_complexity77_sumlog_kkp(const std::vector<T> lattice){
    std::string sequence = int_vector_to_string<T>(lattice);
    std::vector<std::pair<int, int>> factors;
    size_t nfactors = lempel_ziv_complexity77_kkp(sequence, &factors);
    if (nfactors != factors.size()){throw std::runtime_error("nfactors and factors.size do no match");}
    double sumlog = 0;
    for (const auto &x : factors){
        sumlog += std::log2(static_cast<double>(std::max(2, x.first))) + std::log2(static_cast<double>(std::max(2, x.second)));
    }
    return std::pair<size_t, double>(nfactors, sumlog);
}


// Ziv-Merhav method for estimating relative entropy by cross parsing:

inline size_t cross_parsing(std::string& sequence1, std::string& sequence2, std::vector<std::pair<int, int>>* factorsp=NULL) {
    std::string sequence = sequence1 + char(0) + sequence2;
    const int length = sequence.size();
    std::shared_ptr<unsigned char> text(new unsigned char[length], std::default_delete<unsigned char[]>());
    std::shared_ptr<int> sa(new int[length], std::default_delete<int[]>());
    std::shared_ptr<int> isa(new int[length], std::default_delete<int[]>());

    memcpy(text.get(), sequence.c_str(), length);
    divsufsort(text.get(), sa.get(), length);
    for (int i = 0; i < length; ++i) {
        isa.get()[sa.get()[i]] = i;
    }

    const int length1 = sequence1.size();
    int nfactors = 0;
    int next = length1 + 1;
    int nsv_lex, psv_lex, nsv, psv;
    
    while (next < length) {
        int next_lex = isa.get()[next];

        psv_lex = next_lex - 1;
        while (psv_lex >= 0 && sa.get()[psv_lex] >= length1) --psv_lex;
        if (psv_lex == -1) {
            psv = -1;
        } else {
            psv = sa.get()[psv_lex];
        }
        nsv_lex = next_lex + 1;
        while (nsv_lex < length && sa.get()[nsv_lex] >= length1) ++nsv_lex;
        if (nsv_lex == length) {
            nsv = -1;
        } else {
            nsv = sa.get()[nsv_lex];
        }

        next = parse_phrase(text.get(), length, next, psv, nsv, factorsp);
        ++nfactors;
    }

    return nfactors;
}

template<class T = long long>
size_t cross_parsing(const std::vector<T> lattice1, const std::vector<T> lattice2, std::vector<std::pair<int, int>> &factors) {
    std::string sequence1 = int_vector_to_string_cp<T>(lattice1);
    std::string sequence2 = int_vector_to_string_cp<T>(lattice2);
    return cross_parsing(sequence1, sequence2, &factors);
}

template<class T = long long>
size_t cross_parsing(const std::vector<T> lattice1, const std::vector<T> lattice2) {
    std::string sequence1 = int_vector_to_string_cp<T>(lattice1);
    std::string sequence2 = int_vector_to_string_cp<T>(lattice2);
    return cross_parsing(sequence1, sequence2, NULL);
}

template<class T = long long>
std::vector<std::vector<int>> get_cross_parsing_factors(const std::vector<T> lattice1, const std::vector<T> lattice2) {
    std::string sequence1 = int_vector_to_string_cp<T>(lattice1);
    std::string sequence2 = int_vector_to_string_cp<T>(lattice2);
    std::vector<std::pair<int, int>> factors;
    size_t nfactors = cross_parsing(sequence1, sequence2, &factors);
    if (nfactors != factors.size()) { throw std::runtime_error("nfactors and factors.size do no match"); }
    std::vector<std::vector<int>> v;
    for (const auto& x : factors) {
        v.push_back({ x.first, x.second });
    }
    return v;
}

template<class T = long long>
std::pair<size_t, double> cross_parsing_complexity_sumlog(const std::vector<T> lattice1, const std::vector<T> lattice2) {
    std::string sequence1 = int_vector_to_string_cp<T>(lattice1);
    std::string sequence2 = int_vector_to_string_cp<T>(lattice2);
    std::vector<std::pair<int, int>> factors;
    size_t nfactors = cross_parsing(sequence1, sequence2, &factors);
    if (nfactors != factors.size()) { throw std::runtime_error("nfactors and factors.size do no match"); }
    double sumlog = 0;
    for (const auto& x : factors) {
        sumlog += std::log2(static_cast<double>(std::max(2, x.first))) + std::log2(static_cast<double>(std::max(2, x.second)));
    }
    return std::pair<size_t, double>(nfactors, sumlog);
}

}
#endif // #ifndef
