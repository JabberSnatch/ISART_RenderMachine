#ifndef __QUATERNION_HPP__
#define __QUATERNION_HPP__

#include "Vec3.hpp"
#include "Matrix.hpp"
#include "Constants.hpp"


struct Quaternion
{
	Quaternion(float _x = 0.f, float _y = 0.f, float _z = 0.f, float _w = 1.f)
		:x(_x), y(_y), z(_z), w(_w)
	{ normalize(); }
	Quaternion(const Vec3& _eulerAngles)
	{
		float cyaw =	cos(_eulerAngles.x / 2.f);
		float cpitch =	cos(_eulerAngles.y / 2.f);
		float croll =	cos(_eulerAngles.z / 2.f);
		float syaw =	sin(_eulerAngles.x / 2.f);
		float spitch =	sin(_eulerAngles.y / 2.f);
		float sroll =	sin(_eulerAngles.z / 2.f);

		x = syaw*spitch*croll + cyaw*cpitch*sroll;
		y = syaw*cpitch*croll + cyaw*spitch*sroll;
		z = cyaw*spitch*croll - syaw*cpitch*sroll;
		w = cyaw*cpitch*croll - syaw*spitch*sroll;

		normalize();
	}
	Quaternion(float _angle, const Vec3& _axis)
	{
		Vec3 axis = _axis.normalized();

		float sinA = (float)sin(MC::Deg2Rad() * (_angle / 2.f));

		x = axis.x * sinA;
		y = axis.y * sinA;
		z = axis.z * sinA;
		w = (float)cos(MC::Deg2Rad() * (_angle / 2.f));

		normalize();
	}
	Quaternion(const Quaternion&) = default;


	Quaternion	conjugate() const { return Quaternion(-x, -y, -z, w); }
	float		sqrMagnitude() const { return x*x + y*y + z*z + w*w; }
	float		magnitude() const { return sqrt(sqrMagnitude()); }
	Quaternion	normalized() const { return *this / magnitude(); }
	void		normalize() { float mag = magnitude(); x /= mag; y /= mag; z /= mag; w /= mag; }

	Matrix		getMatrix() const
	{
		Matrix result; result.Identity();

		result.Set(0, 0, 1.f - 2.f*y*y - 2.f*z*z);
		result.Set(0, 1, 2.f*x*y + 2.f*w*z);
		result.Set(0, 2, 2.f*x*z - 2.f*w*y);
	
		result.Set(1, 0, 2.f*x*y - 2.f*w*z);
		result.Set(1, 1, 1.f - 2.f*x*x - 2.f*z*z);
		result.Set(1, 2, 2.f*y*z + 2.f*w*x);
	
		result.Set(2, 0, 2.f*x*z + 2.f*w*y);
		result.Set(2, 1, 2.f*y*z - 2.f*w*x);
		result.Set(2, 2, 1.f - 2.f*x*x - 2.f*y*y);

		return result;
	}

	// TODO: Fix. It yields results ranging from -50deg to 50deg
	Vec3		toEuler() const
	{
		Vec3 result;
		result.x = asin(2.f * x*y + 2.f * z*w);

		if (abs(x * y + z * w - 0.5f) < 0.001f) // north pole
		{
			result.y = 2.f * atan2(x, w);
			result.z = 0.f;
		}
		else if (abs(x * y + z * w + 0.5f) < 0.001f) // south pole
		{
			result.y = -2.f * atan2(x, w);
			result.z = 0.f;
		}
		else
		{
			result.y = (2.f * y*w - 2.f * x*z, 1.f - 2.f * y * y - 2.f * z * z);
			result.z = (2.f * x*w - 2.f * y*z, 1.f - 2.f * x *x - 2.f * z * z);
		}

		return result * (float)MC::Rad2Deg();
	}

	Quaternion	operator * (const Quaternion& _other) const
	{
		Quaternion result;

		result.w = (w * _other.w) - (x * _other.x) - (y * _other.y) - (z * _other.z);
		result.x = (w * _other.x) + (x * _other.w) + (y * _other.z) - (z * _other.y);
		result.y = (w * _other.y) - (x * _other.z) + (y * _other.w) + (z * _other.x);
		result.z = (w * _other.z) + (x * _other.y) - (y * _other.x) + (z * _other.w);

		return result;
	}

	Vec3		operator * (const Vec3& _vec) const
	{
		// Courtesy of http://blog.molecular-matters.com/2013/05/24/a-faster-quaternion-vector-multiplication/
		Vec3 vectPart(x, y, z);
		Vec3 t = vectPart.cross(_vec) * 2.f;
		Vec3 result = _vec + t * w + vectPart.cross(t);

		return result;
	}

	Quaternion	operator / (const float _v) const 
	{ return Quaternion(x / _v, y / _v, z / _v, w / _v); }

	float x; // *I <=> b
	float y; // *J <=> c
	float z; // *K <=> d
	float w; // *1 <=> a
};


#endif /*__QUATERNION_HPP__*/