#pragma once

#include <iostream>
#include <memory>
#include <list>
#include <map>
#include "Coordinate.h"

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
/************************************************************************************************************/
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
		setSize(values.size());
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

	size_t getDimension(size_t i) const {
		return _dimensions[i];
	}

	friend std::ostream& operator<<(std::ostream& out, const Matrix& m) {
		MatrixPrinter<T, DIMENSIONS>::print(m._array.get(), m._size, m._dimensions, out);
		return out;
	}

	const T* begin() const
	{
		// return &_array[0];
		return _array.get();
	}

	const T* end() const
	{
		// return &_array[_size];
		return _array.get() + _size;
	}

	T& operator[] (Coordinate<DIMENSIONS> coor)
	{
		size_t offset = 0;
		size_t curr_size = _size;
		for (int i = 0; i < DIMENSIONS; i++) {
			curr_size = curr_size / _dimensions[i];
			offset += coor[i]*curr_size;
		}
		return _array[offset];
	}

	const T& operator [] (Coordinate<DIMENSIONS> coor) const
	{
		size_t offset = 0;
		size_t curr_size = _size;
		for (int i = 0; i < DIMENSIONS; i++) {
			curr_size = curr_size / _dimensions[i];
			offset += coor[i] * curr_size;
		}
		return _array[offset];
	}

	Coordinate<DIMENSIONS> pointer2Coordinate (T* point)
	{
		Coordinate<DIMENSIONS> result;
		size_t curr_size = _size;
		for (int i = 0; i < DIMENSIONS; i++) {
			curr_size = curr_size / _dimensions[i];
			result[i] = static_cast<int>((point - begin()) / curr_size);
			point = point - (result[i]* curr_size);
		}
		return result;
	}

	template<class GroupingFunc>
	auto groupValues(GroupingFunc groupingFunc) {
		using GroupingType = std::result_of_t<GroupingFunc(T&)>;
		std::map<GroupingType, list<list<Coordinate<DIMENSIONS>>>> all_groups;
		
		// Intialize a matrix of zeroes, the size of "this" matrix.
		// Will be used to "remember" if we've alreay attached some coordinate
		// to some group.
		Matrix<int, DIMENSIONS> markChecked;
		initMarked(markChecked);
		
		for (const T* it = begin(); it < end(); it++) {
			Coordinate<DIMENSIONS> coor = pointer2Coordinate(const_cast<T*>(it));
			if (markChecked[coor] == 0) {
				list<Coordinate<DIMENSIONS>> group;
				group.push_back(coor);
				markChecked[coor] = 1;
				buildGroup(group, coor, groupingFunc, markChecked);
				all_groups[groupingFunc(this->operator[](coor))].push_back(group);
			}
		}

		// Sort all created groups as expected in the Targil
		for (auto& category : all_groups) {
			for(auto& group : category.second) {
				group.sort();
			}
			category.second.sort(&Matrix::compareGroups);
		}
		return all_groups;
	}

	// Used to sort groups
	static bool compareGroups(const list<Coordinate<DIMENSIONS>> &a, const list<Coordinate<DIMENSIONS>> &b) {
		return (a.front() < b.front());
	}
	
	// A recursive functions that discovers the whole group to which "coor" belongs to
	// according "groupingFunc". The group is being constructed in "group" which is 
	// passed by ref fom call to call.
	template<class GroupingFunc>
	void buildGroup(list<Coordinate<DIMENSIONS>>& group,
		Coordinate<DIMENSIONS> coor,
		const GroupingFunc& groupingFunc,
		Matrix<int, DIMENSIONS>& markChecked)
	{
		for (int dim = 0; dim < DIMENSIONS; dim++) 
		{
			Coordinate<DIMENSIONS> next = coor;
			if (coor[dim] < static_cast<int>(_dimensions[dim]-1)) {
				++next[dim];
				if ((markChecked[next] == 0) &&
					(groupingFunc(this->operator[](coor)) ==
					groupingFunc(this->operator[](next))))
				{
					group.push_back(next);
					markChecked[next] = 1;
					buildGroup(group, next, groupingFunc, markChecked);
				}
				--next[dim];
			}
			if (coor[dim] > 0) {
				--next[dim];
				if ((markChecked[next] == 0) &&
					(groupingFunc(this->operator[](coor)) ==
					groupingFunc(this->operator[](next))))
				{
					group.push_back(next);
					markChecked[next] = 1;
					buildGroup(group, next, groupingFunc, markChecked);
				}
				++next[dim];
			}
		}
	}

	// Initialize a "marked" matrix of ints (zeroes) with size of "this" matrix.
	// Use private functions to set attributes of "marked" matrix
	// to ensure they are not abused outside of classs.
	void initMarked(Matrix<int, DIMENSIONS>& marked)
	{
		for (int i = 0; i < DIMENSIONS; i++) {
			marked.setDim(i, _dimensions[i]);
		}
		marked.setSize(_size);
		marked.initMatrixData();
	}

	void setSize(size_t newSize) {
		const_cast<size_t&>(_size) = newSize;
	}

	void setDim(int dim, size_t dim_size) {
		_dimensions[dim] = dim_size;
	}

	void initMatrixData() {
		_array.reset();
		_array = std::make_unique<int[]>(_size);
	}
};

//Global function to print Groups (from the Targil4 Doc)
template<typename Groups>
static void print(const Groups& all_groups) {
	for (const auto& groupType : all_groups) {
		cout << groupType.first << ":" << endl;
		for (const auto& groupOfType : groupType.second) {
			for (const auto& coord : groupOfType) {
				cout << "{ ";
				for (auto i : coord) {   // was: for(int i : coord)
					cout << i << ' ';
				}
				cout << "} ";
			}
			cout << endl;
		}
	}
}

