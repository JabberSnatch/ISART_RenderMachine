#ifndef __REF_POINT_HPP__
#define __REF_POINT_HPP__

#include <vector>
#include <utility>
#include <cstdint>


// TODO: After refactor is complete, don't forget to remove both the namespace and REF_ in the header guard
namespace refactor {

struct Point
{
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
	using VertexComponentVector_t = std::vector<std::pair<VertexComponent, std::vector<float>>>;

	Point(int32_t _pos, int32_t _tex, int32_t _norm);
	Point(std::initializer_list<std::pair<VertexComponent, int32_t>>) {};
	Point(const Point& _other) = default;
	~Point() = default;

	void		Reserve(VertexComponent _comp, int32_t _size);

	void		Set(VertexComponent _comp, float* _data); // Sets the whole component using data
	void		Set(VertexComponent _comp, int _index, float _data) {}; // Sets _comp[_index]
	void		Set(VertexComponent _comp, int32_t _size, float* _data) {}; // Sets the whole component according to _size using _data

	int32_t		ComponentSize(VertexComponent _comp) const {};
	int32_t		Size() const {};

	bool					operator == (const Point& _other) const;
	bool					operator = (const Point& _other) const {};

	std::vector<float>*		operator [] (VertexComponent _index);

	VertexComponentVector_t		_Components;

};


Point::Point(int32_t _pos, int32_t _tex, int32_t _norm)
{
	Reserve(TEXTURE, _tex);
	Reserve(NORMAL, _norm);
	Reserve(POSITION, _pos);
}


void
Point::Reserve(Point::VertexComponent _comp, int32_t _size)
{
	std::vector<float>* vec = (*this)[_comp];

	// Emplace the new component before resizing it
	if (!vec)
	{
		_Components.resize(_Components.size() + 1);

		// Keep the vector sorted by increasing VertexComponent index
		int cursor = _Components.size() - 1;
		while (cursor > 0)
		{
			if (_Components[cursor - 1].first > _comp)
				_Components[cursor] = _Components[cursor - 1];
			else break;
			cursor--;
		}

		_Components[cursor].first = _comp;
		vec = &_Components[cursor].second;
	}

	// Resize the selected component
	vec->resize(_size);
	memset(vec->data(), 0, sizeof(float) * _size);
}


void
Point::Set(VertexComponent _comp, float* _data)
{
	auto vec = (*this)[_comp];
	memcpy(vec->data(), _data, sizeof(float) * vec->size());
}


bool
Point::operator == (const Point& _other) const
{
	bool same = _Components.size() == _other._Components.size();

	for (size_t i = 0; i < _Components.size() && same; ++i)
		if (same = _Components[i].second.size() == _other._Components[i].second.size())
			//same = !memcmp(_Components[i].second.data(), _other._Components[i].second.data(), sizeof(float) * _Components[i].second.size());
			same = _Components[i].second == _other._Components[i].second;

	return same;
}


std::vector<float>*
Point::operator [] (VertexComponent _index)
{
	// TODO: Implement binary search
	std::vector<float>* element = nullptr;

	for (size_t i = 0; i < _Components.size() && element == nullptr; ++i)
		if (_Components[i].first == _index)
			element = &_Components[i].second;

	return element;
}


}


#endif // __REF_POINT_HPP__
