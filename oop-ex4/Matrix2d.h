#pragma once

#include <iostream>
#include <memory>
#include <list>
#include <map>
#include "Coordinate.h"

using namespace std;


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

	// DIMENSIONS == 1
	// We want here a ctor with this signature:
	//    Matrix(const std::initializer_list<T>& values) {
	// but SFINAE is needed to block this ctor from being used by Matrix with DIMENSIONS > 1
	// The SFINAE results with the exact signature we want, but only for cases DIMENSIONS == 1
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
	// DIMENSIONS > 1
	// We want here a ctor with this signature:
	//    Matrix(const std::initializer_list<Matrix<T, DIMENSIONS-1>>& values) {
	// although this ctor is not useful and won't be used by Matrix<T, 1> it will still be in class
	// and thus would compile as part of Matrix<T, 1> resulting with a parameter of type:
	//          const std::initializer_list< Matrix<T, 0> >& values
	// having Matrix<T, 0> as a parameter - even for a function that is not in use, inside a class that is used
	// would require the compiler to instantiate Matrix<T, 0> class which results with a warning
	// the SFINAE below solves this warning.
	// The SFINAE results with the exact signature we want, but only for cases DIMENSIONS > 1
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

	Matrix(Matrix&& m) {
		*this = std::move(m);
	}

	auto& operator=(Matrix&& m) {
		std::swap(_array, m._array);
		std::swap(const_cast<size_t&>(_size), const_cast<size_t&>(m._size));
		std::swap(_dimensions, m._dimensions);
		return *this;
	}

	size_t getDimension(size_t i) const {
		return _dimensions[i];
	}

	std::map<GroupingType, std::list<list<Coordinates>>> groups;
	std::function mapping;
	matrix2d ezer;

	void buidGroup(list& group, Coordinate coor) {
		if ((mapping(coor.x, coor.y) == (mapping(coor.x - 1, coor.y)) && (ezer[coor.x - 1, coor.y] == 0)) {
			group.emplace_back(coor.x - 1, coor.y);
			ezer[coor.x - 1, coor.y] = 1;
			buidGroup(group, Coordinate(coor.x - 1, coor.y));
		}
		if ((mapping(coor.x, coor.y) == (mapping(coor.x + 1, coor.y)) && (ezer[coor.x + 1, coor.y] == 0)) {
			group.emplace_back(coor.x + 1, coor.y);
			ezer[coor.x + 1, coor.y] = 1;
			buidGroup(group, Coordinate(coor.x + 1, coor.y));
		}
		if ((mapping(coor.x, coor.y) == (mapping(coor.x, coor.y - 1)) && (ezer[coor.x, coor.y - 1] == 0)) {
			group.emplace_back(coor.x, coor.y - 1);
			ezer[coor.x, coor.y - 1] = 1;
			buidGroup(group, Coordinate(coor.x, coor.y - 1));
		}
		if ((mapping(coor.x, coor.y) == (mapping(coor.x, coor.y + 1)) && (ezer[coor.x, coor.y + 1] == 0)) {
			group.emplace_back(coor.x, coor.y + 1);
			ezer[coor.x, coor.y + 1] = 1;
			buidGroup(group, Coordinate(coor.x, coor.y + 1));
		}
	}

	friend std::ostream& operator<<(std::ostream& out, const Matrix& m) {
		MatrixPrinter<T, DIMENSIONS>::print(m._array.get(), m._size, m._dimensions, out);
		return out;
	}
};