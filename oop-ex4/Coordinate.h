#pragma once

#include <initializer_list>
#include <iostream>
using namespace std;

template <size_t DIM>
class Coordinate
{
public:
	//empty c'tor
	Coordinate()
	{
		for (int i = 0; i < DIM; i++)
			arr[i] = 0;
	}
	//initializer list c'tor
	Coordinate(const std::initializer_list<int>& values)
	{
		int i = 0;
		for (auto& coor : values)
			arr[i++] = coor;
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

private:
	int arr[DIM];
};