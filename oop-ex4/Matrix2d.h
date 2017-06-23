#pragma once

#include <iostream>
#include <memory>
#include <list>
#include <map>
#include "GenericCoor.h"

using namespace std;

// helper classes
template<class T, size_t DIMENSIONS>
struct MatrixCopier {
	static void copy(T* dest, size_t dest_size, const size_t* dest_dimensions, const T* source, size_t source_size, const size_t* source_dimensions) {
		size_t dest_size0 = dest_dimensions[0] ? dest_size / dest_dimensions[0] : 0;
		size_t source_size0 = source_dimensions[0] ? source_size / source_dimensions[0] : 0;
		for (size_t i = 0; i < source_dimensions[0]; ++i) {
			MatrixCopier<T, DIMENSIONS - 1>::copy(dest + (i * dest_size0), dest_size0, dest_dimensions + 1, source + (i * source_size0), source_size0, source_dimensions + 1);
		}
	}
};

template<class T>
struct MatrixCopier<T, 1> {
	static void copy(T* dest, size_t dest_size, const size_t* dest_dimensions, const T* source, size_t source_size, const size_t* source_dimensions) {
		for (size_t i = 0; i < source_size; ++i) {
			dest[i] = source[i];
		}
	}
};

template<class T, size_t DIMENSIONS>
struct MatrixPrinter {
	static void print(const T* values, size_t size, const size_t* dimensions, std::ostream& out = cout) {
		out << '{';
		size_t size0 = size / dimensions[0];
		MatrixPrinter<T, DIMENSIONS - 1>::print(values, size0, dimensions + 1, out);
		for (size_t i = 1; i < dimensions[0]; ++i) {
			out << ',';
			MatrixPrinter<T, DIMENSIONS - 1>::print(values + (i*size0), size0, dimensions + 1, out);
		}
		out << '}';
	}
};

template<class T>
struct MatrixPrinter<T, 1> {
	static void print(const T* values, size_t size, const size_t* dimensions, std::ostream& out = cout) {
		out << '{';
		out << values[0];
		for (size_t i = 1; i < size; ++i) {
			out << ',' << values[i];
		}
		out << '}';
	}
};


//generic Matrix class
template<class T, size_t DIMENSIONS>
class Matrix {
	constexpr static size_t NUM_DIMENSIONS = DIMENSIONS;
	std::unique_ptr<T[]> _array = nullptr;
	size_t _dimensions[DIMENSIONS] = {};
	const size_t _size = 0;
	friend class Matrix<T, DIMENSIONS + 1>;
public:
	size_t size() const { return _size; }
	Matrix() {}

	template<typename G = T>
	Matrix(const std::initializer_list<typename std::enable_if_t<DIMENSIONS == 1, G>>& values) {
		const_cast<size_t&>(_size) = values.size();
		_dimensions[0] = _size;
		_array = std::make_unique<T[]>(_size);
		size_t i = 0;
		for (auto& val : values) {
			_array[i++] = val;
		}
	}

	template<typename G = T>
	Matrix(const std::initializer_list<Matrix<typename std::enable_if_t<(DIMENSIONS > 1), G>, DIMENSIONS - 1>>& values) {
		_dimensions[0] = values.size();
		for (auto& m : values) {
			for (size_t dim = 0; dim < DIMENSIONS - 1; ++dim) {
				if (m._dimensions[dim] > _dimensions[dim + 1]) {
					_dimensions[dim + 1] = m._dimensions[dim];
				}
			}
		}
		size_t size = 1;
		for (size_t dim = 0; dim < DIMENSIONS; ++dim) {
			size *= _dimensions[dim];
		}

		const_cast<size_t&>(_size) = size;
		_array = std::make_unique<T[]>(_size); // "zero initialized" - T()
		size_t i = 0;
		size_t dest_size = _size / _dimensions[0];
		for (auto& m : values) {
			MatrixCopier<T, DIMENSIONS - 1>::copy(&(_array[i * dest_size]), dest_size, _dimensions + 1, m._array.get(), m._size, m._dimensions);
			++i;
		}
	}

	/*Matrix(Matrix&& m) {
		*this = std::move(m);
	}

	auto& operator=(Matrix&& m) {
		std::swap(_array, m._array);
		std::swap(const_cast<size_t&>(_size), const_cast<size_t&>(m._size));
		std::swap(_dimensions, m._dimensions);
		return *this;
	}*/

	size_t getDimension(size_t i) const {
		return _dimensions[i];
	}

	/*std::map<GroupingType, std::list<list<Coordinates>>> groups;
	std::function mapping;
	matrix2d ezer;

	void buidGroup(list& group, Coordinate2d coor) {
		if ((mapping(coor.x, coor.y) == (mapping(coor.x - 1, coor.y)) && (ezer[coor.x - 1, coor.y] == 0)) {
			group.emplace_back(coor.x - 1, coor.y);
			ezer[coor.x - 1, coor.y] = 1;
			buidGroup(group, Coordinate2d(coor.x - 1, coor.y));
		}
		if ((mapping(coor.x, coor.y) == (mapping(coor.x + 1, coor.y)) && (ezer[coor.x + 1, coor.y] == 0)) {
			group.emplace_back(coor.x + 1, coor.y);
			ezer[coor.x + 1, coor.y] = 1;
			buidGroup(group, Coordinate2d(coor.x + 1, coor.y));
		}
		if ((mapping(coor.x, coor.y) == (mapping(coor.x, coor.y - 1)) && (ezer[coor.x, coor.y - 1] == 0)) {
			group.emplace_back(coor.x, coor.y - 1);
			ezer[coor.x, coor.y - 1] = 1;
			buidGroup(group, Coordinate2d(coor.x, coor.y - 1));
		}
		if ((mapping(coor.x, coor.y) == (mapping(coor.x, coor.y + 1)) && (ezer[coor.x, coor.y + 1] == 0)) {
			group.emplace_back(coor.x, coor.y + 1);
			ezer[coor.x, coor.y + 1] = 1;
			buidGroup(group, Coordinate2d(coor.x, coor.y + 1));
		}


		next_coor = curr_coor;
		for (int& dim_val : next_coor) {
			dim_val++;
			if ((mapping(curr_coor) == (mapping(next_coor) && (ezer[next_coor] == 0)) {
				group.emplace_back(next_coor);
				ezer[next_coor] = 1;
				buildGroup(group, next_coor);
			}
			dim_val = dim_val - 2;
			if ((mapping(curr_coor) == (mapping(coor[i].set(coor[i] - 1)) && (ezer[coor.x - 1, coor.y] == 0)) {
				group.emplace_back(coor.x - 1, coor.y);
				ezer[coor.x - 1, coor.y] = 1;
				buidGroup(group, Coordinate2d(coor.x - 1, coor.y));
			}
			dim_val++;
		}




		for (i:1..n))
		{
			if ((mapping(coor) == (mapping(coor[i].set(coor[i]-1)) && (ezer[coor.x - 1, coor.y] == 0)) {
				group.emplace_back(coor.x - 1, coor.y);
				ezer[coor.x - 1, coor.y] = 1;
				buidGroup(group, Coordinate2d(coor.x - 1, coor.y));
			}
		}	*/

	friend std::ostream& operator<<(std::ostream& out, const Matrix& m) {
		MatrixPrinter<T, DIMENSIONS>::print(m._array.get(), m._size, m._dimensions, out);
		return out;
	}
};





