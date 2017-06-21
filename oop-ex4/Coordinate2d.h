#pragma once


class Coordinate2d
{
public:
	int arr[2];
	Coordinate2d(int row, int col)
	{
		arr[0] = row;
		arr[1] = col;
	}
	const int* begin() const
	{
		return arr;
	}
	const int* end() const
	{
		return arr + 2;
	}
	bool operator < (const Coordinate2d& other) const
	{
		if (arr[0] < other.arr[0])
			return true;
		if (arr[0] > other.arr[0])
			return false;
		return (arr[1] <= other.arr[1]);		
	}

	const int& operator [] (int index) const
	{
		return arr[index];
	}

};