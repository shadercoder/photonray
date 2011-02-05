#pragma once
#include <stdlib.h>
#include <stdexcept>

class DenseField
{
private:
	float* data;
	int size;
public:
	int xSize, ySize, zSize;

	DenseField(int xSize, int ySize, int zSize)
	{
		this->xSize = xSize;
		this->ySize = ySize;
		this->zSize = zSize;
		size = xSize * ySize * zSize;
		data = new float[size];
		memset(data, 0, sizeof(float) * size);
	}
	
	bool isInside(int i, int j, int k)
	{
		if (i >= 0 && j >= 0 && k >= 0 && i < xSize && j < ySize && k < zSize)
		{
			return true;
		}
		return false;
		/*int res = arrayIndexFromCoordinate(i, j, k);
		if (res >= 0 && res < size)
		{
			return true;
		}
		return false;
		*/
	}

	int getDataSize() const
	{
		return size;
	}
		
	float* getData() const
	{
		return data;
	}

	const float& value(int i, int j, int k)
	{
		return data[arrayIndexFromCoordinate(i, j, k)];
	}

	float& lvalue(int i, int j, int k)
	{
		int index = arrayIndexFromCoordinate(i, j, k);
		if (index >= 0 && index < size)
		{
			return data[index];
		}
		else
		{
			throw std::out_of_range("out of range");
		}
	}
	
	void set(int i, int j, int k, float val)
	{
		int index = arrayIndexFromCoordinate(i, j, k);
		if (index >= 0 && index < size)
		{
			data[index] = val;
		}
	}

	int arrayIndexFromCoordinate(int i, int j, int k)
	{
		return (k * xSize * ySize + i * ySize + j);
	}

	void clear()
	{
		memset(data, 0, sizeof(float) * size);
	}

	DenseField(void)
	{
	}

	~DenseField(void)
	{
		//delete[] data;
		SAFE_DELETE_ARRAY(data);
	}
};
