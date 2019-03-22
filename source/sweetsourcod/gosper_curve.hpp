#ifndef SSC_GOSPER_CURVE_H
#define SSC_GOSPER_CURVE_H

#include <cmath>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <vector>
#include <iterator>
#include <random>
#include <iostream>


namespace ssc 
{

static constexpr long double Pi = 3.14159265358979323846264338327950288L;
static long double const factor = 1.0L / std::sqrt(7.0L);
static long double const alpha = std::asin(std::sqrt(3.0L/7.0L) / 2.0L);
static double const orientation[7] = { -Pi * 2.0/3.0, 0.0, 0.0, -Pi * 2.0/3.0, 0.0, Pi * 2.0/3.0, 0.0 };
static bool   const idx_pattern[7] = { false, true, true, true, false, false, true };

template <class T, class U>
std::pair<T, U> operator+(const std::pair<T, U>& l, const std::pair<T, U>& r) {
	return {l.first + r.first, l.second + r.second};
}

template <class T, class U>
std::pair<T, U> operator-(const std::pair<T, U>& l, const std::pair<T, U>& r) {
	return {l.first - r.first, l.second - r.second};
}

template <class T, class U, class V>
std::pair<T, U> operator*(const V& l, const std::pair<T, U>& r) {
	return {l * r.first, l * r.second};
}


inline std::pair<double, double> hex_axial2xy(std::pair<int, int> ql, double size) {
	return std::make_pair(size * (ql.first + ql.second/2.0) * std::sqrt(3.0),
						  size * ql.second * 3.0/2.0);
}

inline std::pair<int, int> hex_xy2axial(std::pair<double, double> xy, double size) {
	// https://www.redblobgames.com/grids/hexagons/#rounding
	double cube_x = (xy.first * std::sqrt(3.0) / 3.0 - xy.second / 3.0) / size;
	double cube_z = xy.second * 2.0/3.0 / size;
	double cube_y = -cube_x - cube_z;

	int rx = std::lround(cube_x);
	int ry = std::lround(cube_y);
	int rz = std::lround(cube_z);
	double x_diff = std::abs(cube_x - rx);
	double y_diff = std::abs(cube_y - ry);
	double z_diff = std::abs(cube_z - rz);

	if ((x_diff > y_diff) && (x_diff > z_diff)) {
		rx = -ry - rz;
	}
	else if (y_diff > z_diff) {
		ry = -rx - rz;
	}
	else {
		rz = -rx - ry;
	}
	
	return std::make_pair(rx, rz);
}


inline std::pair<double, double> rotate(double theta, std::pair<double, double> r) {
	return std::make_pair(std::cos(theta)*r.first - std::sin(theta)*r.second,
						  std::sin(theta)*r.first + std::cos(theta)*r.second);
}

inline int get_y_idx(const std::pair<double, double> d_i, const std::pair<double, double> dy_i) {
	double dot = dy_i.first*d_i.first  + dy_i.second*d_i.second;
	double det = dy_i.first*d_i.second - dy_i.second*d_i.first;
	double angle = std::atan2(det, dot);
	int rd = std::lround(3.0*angle / Pi);
	int y;

	if (std::hypot(dy_i.first, dy_i.second) < 0.01 * std::hypot(d_i.first, d_i.second)) {
		y = 4;
	}
	else if (rd ==  0) { y = 0; } 
	else if (rd == -1) { y = 5; } 
	else if (rd == -2) { y = 6; } 
	else if (rd ==  1) { y = 1; } 
	else if (rd ==  2) { y = 2; } 
	else { y = 3; }

	return y;
}

inline std::vector<std::pair<double, double>> gosper_center(const std::pair<double, double> pt, const int n) {
	double s_i = std::pow(factor, n);
	std::pair<double, double> cr_i = hex_axial2xy( hex_xy2axial(pt, s_i), s_i );
	std::pair<double, double> c_i  = rotate(n*alpha, cr_i);
	std::vector<std::pair<double, double>> center;

	center.push_back(c_i);
	for (int i = n - 1; i >= 0; --i) {
		s_i /= factor;
		cr_i = hex_axial2xy( hex_xy2axial( rotate(alpha, cr_i), s_i ), s_i );
		c_i  = rotate(i*alpha, cr_i);
		center.push_back(c_i);
	}
	std::reverse(center.begin(), center.end());

	return center;
}

inline std::vector<int> gosper_index(const std::vector<std::pair<double, double>>& center, const int n) {
	int k_i = 0, y_i = 1;
	bool pattern = true;
	std::pair<double, double> d_i  = std::make_pair(-std::sqrt(3.0), 0.0);
	std::pair<double, double> dy_i = std::make_pair(0.0, 0.0);
	std::vector<int> index;

	if (std::abs(center[0].first) + std::abs(center[0].second) > 0.1) {
		index.push_back(-1);
	}
	else {
		index.push_back(k_i);
		for (int i = 1; i <= n; ++i) {
			d_i = factor * rotate(alpha, rotate(orientation[y_i], d_i));
			dy_i = center[i] - center[i - 1];

			y_i = get_y_idx(d_i, dy_i);
			if (pattern) {
				k_i = y_i;
				pattern = idx_pattern[y_i];
			}
			else {
				k_i = 6 - y_i;
				pattern = !idx_pattern[y_i];
			}

			index.push_back(k_i);
		}
	}
	
	return index;
}

// return distance along a level-n Gosper curve given input xy coordinates pt
// algorithm from doi:10.1109/CYBConf.2017.7985819
template<class T = long long>
T gosper_coord2distance(const std::pair<double, double> pt, const int n) {
	if (n < 0) { throw std::runtime_error("recursion level of Gosper curve should not be negative"); }
	std::vector<std::pair<double, double>> center = gosper_center(pt, n);
	std::vector<int> index = gosper_index(center, n);
	T distance = 0, power7 = 1;
	
	if (index[0] == 0) {
		for (size_t i = 0; i < index.size(); ++i) {
			distance += index[n - i] * power7;
			power7 *= 7;
		}
	}
	else {
		distance = -1;
	}

	return distance;
}


}
#endif // #ifndef
