#pragma once



class Coordinate3d
{
public:
	int arr[3];
	Coordinate3d(int depth, int row,int col)
	{
		arr[0] = depth;
		arr[1] = row;
		arr[2] = col;
	}
	const int* begin() const
	{
		return arr ;
	}
	const int* end() const
	{
		return arr + 3;
	}
	bool operator < (const Coordinate3d& other) const
	{
		if (arr[0] < other.arr[0])
			return true;
		else if (arr[0] > other.arr[0])
			return false;
		else
		{
			if (arr[1] < other.arr[1])
				return true;
			else if (arr[1] > other.arr[1])
				return false;
			return (arr[2] <= other.arr[2]);
		}

	}

	const int& operator [] (int index) const
	{
		return arr[index];
	}

};