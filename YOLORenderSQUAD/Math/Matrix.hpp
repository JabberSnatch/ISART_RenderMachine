#ifndef __MATRIX_HPP__
#define __MATRIX_HPP__


struct Matrix
{
	Matrix() = default;
	Matrix(const Matrix&) = default;
	Matrix(Matrix&&) = default;
	~Matrix() = default;

	static auto		Translation(float _x, float _y, float _z) -> Matrix;
	static auto		Rotation(float _x, float _y, float _z) -> Matrix;
	static auto		Scale(float _x, float _y, float _z) -> Matrix;

	static auto		mul(const float* _A, const float* _B, float* o_res) -> void;
	static auto		identity(float* o_res) -> void;
	static auto		transpose(float* o_res, float* _source) -> void;

	auto	Identity() -> void { identity(data); }
	auto	Transpose() -> void { transpose(data, data); }
	auto	Transposed() -> Matrix& { Matrix result; transpose(result.data, data); return result; }

	auto	Set(int _row, int _col, float _v) -> void { data[_row * 4 + _col] = _v; }
	auto	Get(int _row, int _col) -> float { return data[_row * 4 + _col]; }

	auto	operator * (const Matrix& _v) -> Matrix
	{
		Matrix result;
		mul(data, _v.data, result.data);
		return result;
	}

	auto	operator = (const Matrix&)->Matrix& = default;
	auto	operator = (Matrix&&)->Matrix& = default;

	auto	Print() -> void;

	float	data[16] = { 1.f, 0.f, 0.f, 0.f,
						0.f, 1.f, 0.f, 0.f,
						0.f, 0.f, 1.f, 0.f,
						0.f, 0.f, 0.f, 1.f };
};


#endif /*__MATRIX_HPP__*/
