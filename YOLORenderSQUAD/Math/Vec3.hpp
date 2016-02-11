#ifndef __VEC3_HPP__
#define __VEC3_HPP__

#include <cmath>
#include <vector>


struct Vec3
{
	Vec3(float _v = 0.f)
		:x(_v), y(_v), z(_v) {}
	Vec3(float _x, float _y, float _z = 0.f)
		:x(_x), y(_y), z(_z) {}
	Vec3(const Vec3& _other)
		:x(_other.x), y(_other.y), z(_other.z) {}

	Vec3(Vec3&&) = default;
	~Vec3() = default;

	static auto	Zero() -> Vec3 { Vec3 result; return result; }
	static auto Right() -> Vec3 { return Vec3(1.f, 0.f, 0.f); }
	static auto Up() -> Vec3 { return Vec3(0.f, 1.f, 0.f); }
	static auto Forward() -> Vec3 { return Vec3(0.f, 0.f, 1.f); }

	auto	dot(const Vec3& _v) -> float { return x * _v.x + y * _v.y + z * _v.z; }
	auto	cross(const Vec3& _v) -> Vec3& 
	{
		Vec3 result(y * _v.z - z * _v.y,
					z * _v.x - x * _v.z,
					x * _v.y - y * _v.x); 
		return result;
	}
	auto	sqrMagnitude() -> float { return x * x + y * y + z * z; }
	auto	magnitude() -> float { return std::sqrt(sqrMagnitude()); }
	auto	normalized() -> Vec3& { return *this / magnitude(); }
	auto	normalize() -> void 
	{ 
		float mag = magnitude(); 
		x /= mag; y /= mag; z /= mag;
	}


	auto	operator + (const Vec3& _other) -> Vec3&
	{
		return Vec3(x + _other.x, y + _other.y, z + _other.z);
	}
	auto	operator += (const Vec3& _other) -> Vec3&
	{
		x += _other.x; y += _other.y; z += _other.z;
		return *this;
	}
	auto	operator * (float _v) -> Vec3&
	{
		x *= _v; y *= _v; z *= _v;
		return *this;
	}
	auto	operator / (float _v) -> Vec3&
	{
		x /= _v; y /= _v; z /= _v;
		return *this;
	}

	auto	operator = (const Vec3&)->Vec3& = default;
	auto	operator = (Vec3&&)->Vec3& = default;
	auto	ToStdVec() -> std::vector<float> 
	{
		std::vector<float> result;
		result.push_back(x);
		result.push_back(y);
		result.push_back(z);
		return result;
	}

	float x;
	float y;
	float z;
};


#endif /*__VEC3_HPP__*/