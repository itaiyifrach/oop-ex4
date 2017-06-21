#pragma once

#include <initializer_list>
#include <iostream>
using namespace std;

template <size_t DIM>
class Coordinate
{
public:
	int arr[DIM];
	Coordinate(const std::initializer_list<int>& values)
	{
		int i = 0;
		for (auto& cor : values)
			arr[i++] = cor;
	}

	Coordinate(int values[])
	{
		for (size_t i = 0; i<DIM; i++)
			arr[i] = values[i];
	}


	Coordinate<DIM> copyAndChangeCor(int cor, int value)
	{
		Coordinate<DIM> newCor(arr);
		newCor[cor] = value;
		return newCor;
	}

	const int* begin() const
	{
		return arr;
	}

	const int* end() const
	{
		return arr + DIM;
	}

	bool operator < (const Coordinate<DIM> & other) const
	{
		for (size_t i = 0; i < DIM; i++)
		{
			if (arr[i] < other.arr[i])
				return true;
			if (arr[i] > other.arr[i])
				return false;
		}
		return true;
	}

	int& operator [] (int index)
	{

		return arr[index];
	}

	const int& operator [] (int index) const
	{
		return arr[index];
	}


	friend std::ostream& operator<<(std::ostream& out, const Coordinate<DIM>& cor)
	{
		size_t i = 0;
		for (; i<DIM - 1; i++)
			out << cor.arr[i] << ",";
		out << cor.arr[i];
		return out;
	}

};