#ifndef __POINT_HPP__
#define __POINT_HPP__

#include <vector>
#include <utility>
#include <cstdint>
#include <fstream>


struct Point
{
	// TODO: Take this enum into account when generating shader attributes
	enum VertexComponent
	{
		POSITION = 0,
		TEXTURE,
		NORMAL,
		TANGENT,
		BITANGENT,
		COLOR,

		VC_COUNT
	};
	using VertexComponentVector_t = std::vector<std::pair<VertexComponent, int32_t>>;

	Point() = default;
	Point(int32_t _pos, int32_t _tex, int32_t _norm);
	Point(const Point& _other) = default;
	~Point() = default;

	// TODO: Reserve won't do anything if the component already exists.
	float*		Reserve(VertexComponent _comp, int32_t _size);

	// Shortcut to Reserve and Set a component. It might be the fastest way to set a component value.
	float*		Set(VertexComponent _comp, int32_t _size, float* _data); 
	// Copies given data in the vertex array according to the component size. No overflow.
	float*		Set(VertexComponent _comp, float* _data);
	// Sets the component at the specified index. Doesn't check for size, so it could overflow.
	float*		Set(VertexComponent _comp, int _index, float _data);

	int32_t		ComponentSize(VertexComponent _comp) const;

	bool		operator == (const Point& _other) const;
	Point&		operator = (const Point& _other);

	float*			operator [] (VertexComponent _index);
	const float*	operator [] (VertexComponent _index) const;

	VertexComponentVector_t		_Components;
	std::vector<float>			_Data;
	int32_t						_Size = 0;


	void	Serialize(std::fstream& _stream);
	void	Deserialize(std::fstream& _stream);
};


#endif // __POINT_HPP__
