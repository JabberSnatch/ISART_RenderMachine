#include "Matrix.hpp"

#include <iostream>

#define PI 3.14159265359
#define Deg2Rad PI / 180. 


auto
Matrix::Translation(float _x, float _y, float _z) -> Matrix
{
	Matrix result;

	result.Set(3, 0, _x);
	result.Set(3, 1, _y);
	result.Set(3, 2, _z);

	return result;
}


auto
Matrix::Rotation(float _x, float _y, float _z) -> Matrix
{
	Matrix result;
	
	double deg2rad = Deg2Rad;
	float cx, cy, cz, sx, sy, sz;
	cx = (float)cos(_x * deg2rad);
	cy = (float)cos(_y * deg2rad);
	cz = (float)cos(_z * deg2rad);
	sx = (float)sin(_x * deg2rad);
	sy = (float)sin(_y * deg2rad);
	sz = (float)sin(_z * deg2rad);

	result.Set(0, 0, (cy * cz) - (sx * sy * sz));
	result.Set(1, 0, -(cx * sz));
	result.Set(2, 0, (cz * sy) + (sx * sz * cy));
	result.Set(0, 1, (cy * sz) + (sx * sy * cz));
	result.Set(1, 1, (cx * cz));
	result.Set(2, 1, (sy * sz) - (sx * cz * cy));
	result.Set(0, 2, -(sy * cx));
	result.Set(1, 2, sx);
	result.Set(2, 2, (cx * cy));

	return result;
}


auto
Matrix::Scale(float _x, float _y, float _z) -> Matrix
{
	Matrix result;

	result.Set(0, 0, _x);
	result.Set(1, 1, _y);
	result.Set(2, 2, _z);

	return result;
}


auto
Matrix::mul(const float* _A, const float* _B, float* o_res) -> void
{
	float result[16];

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			result[i * 4 + j] = 0.f;

			for (int k = 0; k < 4; ++k)
			{
				result[i * 4 + j] += _A[i * 4 + k] * _B[k * 4 + j];
			}
		}
	}

	memcpy(o_res, result, 16 * sizeof(float));
}


auto
Matrix::identity(float* o_res) -> void
{
	float result[16] = {
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};

	memcpy(o_res, result, 16 * sizeof(float));
}


auto
Matrix::transpose(float* o_res, float* _source) -> void
{
	float result[16];
	
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			result[i * 4 + j] = _source[j * 4 + i];
		}
	}

	memcpy(o_res, result, 16 * sizeof(float));
}


auto
Matrix::Print() -> void
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
			printf("%f, ", data[i * 4 + j]);
		printf("\n");
	}

}
