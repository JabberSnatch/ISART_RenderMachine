#ifndef __OLD_POINT_HPP__
#define __OLD_POINT_HPP__

#include <cstdint>
#include <string>
namespace old {


struct Point
{
	Point(int32_t _pos, int32_t _tex, int32_t _norm)
		:m_Size(_pos + _tex + _norm)
	{
		m_Position = new float[m_Size];
		m_Texture = m_Position + _pos;
		m_Normal = m_Texture + _tex;
	}
	Point(const Point& other)
	{
		m_Size = other.m_Size;
		m_Position = new float[m_Size];
		m_Texture = m_Position + (other.m_Texture - other.m_Position);
		m_Normal = m_Texture + (other.m_Normal - other.m_Texture);
		memcpy(m_Position, other.m_Position, m_Size * sizeof(float));
	}
	~Point()
	{
		delete[] m_Position;
	}

	float*		m_Position;
	float*		m_Texture;
	float*		m_Normal;
	int32_t		m_Size;

	auto	GetPositionSize() const -> int32_t { return static_cast<int32_t>(m_Texture - m_Position); }
	auto	GetTextureSize() const -> int32_t { return static_cast<int32_t>(m_Normal - m_Texture); }
	auto	GetNormalSize() const -> int32_t { return m_Size - static_cast<int32_t>(m_Normal - m_Position); }

	auto	operator == (const Point& other) const -> bool
	{
		return (!memcmp(this->m_Position, other.m_Position, m_Size * sizeof(float)));
	}
	auto	operator = (const Point& _other) -> Point&
	{
		delete[] m_Position;

		m_Size = _other.m_Size;
		m_Position = new float[m_Size];

		memcpy(m_Position, _other.m_Position, m_Size * sizeof(float));
		m_Texture = m_Position + _other.GetPositionSize();
		m_Normal = m_Texture + _other.GetTextureSize();

		return *this;
	}
};

} // old


#endif // __OLD_POINT_HPP__
