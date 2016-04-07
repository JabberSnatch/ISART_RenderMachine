#include "Point.hpp"


Point::Point(int32_t _pos, int32_t _tex, int32_t _norm)
{
	Reserve(POSITION, _pos);
	Reserve(TEXTURE, _tex);
	Reserve(NORMAL, _norm);
}


float*
Point::Reserve(Point::VertexComponent _comp, int32_t _size)
{
	float* vec = (*this)[_comp];

	// Emplace the new component with the expected size
	if (!vec)
	{
		_Components.resize(_Components.size() + 1);
		_Data.resize(_Data.size() + _size);

		// Keep the vector sorted by increasing VertexComponent index
		int cursor = (int)_Components.size() - 1;
		int dataCursor = (int)_Data.size() - _size;
		while (cursor > 0)
		{
			if (_Components[cursor - 1].first > _comp)
			{
				// Move data around to keep it consistent
				int32_t sizeToMove = _Components[cursor - 1].second;
				for (int32_t i = 0; i < sizeToMove; ++i)
				{
					dataCursor--;
					_Data[dataCursor + _size] = _Data[dataCursor];
				}
				
				_Components[cursor] = _Components[cursor - 1];
			}
			else break;
			cursor--;
		}

		_Components[cursor].first = _comp;
		_Components[cursor].second = _size;

		vec = &_Data[dataCursor];
		memset(vec, 0, sizeof(float) * _size);

		_Size += _size;
	}
	// NOTE: For now it doesn't handle shrinking or expanding a component size.

	return vec;
}


float*
Point::Set(VertexComponent _comp, float* _data)
{
	float* vec = (*this)[_comp];
	memcpy(vec, _data, sizeof(float) * ComponentSize(_comp));
	return vec;
}


float*
Point::Set(VertexComponent _comp, int _index, float _data)
{
	float* vec = (*this)[_comp];
	vec[_index] = _data;
	return vec;
}


float*
Point::Set(VertexComponent _comp, int32_t _size, float* _data)
{
	float* vec = Reserve(_comp, _size);
	memcpy(vec, _data, sizeof(float) * _size);
	return vec;
}


int32_t
Point::ComponentSize(VertexComponent _comp) const
{
	int32_t size = -1;

	// TODO: Implement binary search
	for (size_t i = 0; i < _Components.size() && size < 0; ++i)
		if (_Components[i].first == _comp)
			size = _Components[i].second;

	return size;
}


bool
Point::operator == (const Point& _other) const
{
	return !memcmp(_Data.data(), _other._Data.data(), _Size * sizeof(float));
}


Point&
Point::operator = (const Point& _other)
{
	_Size = _other._Size;
	_Data.resize(_Size);
	_Components.resize(_other._Components.size());
	
	for (int i = 0; i < _Components.size(); ++i)
		_Components[i] = _other._Components[i];

	memcpy(_Data.data(), _other._Data.data(), _Size * sizeof(float));

	return *this;
}


float*
Point::operator [] (VertexComponent _index)
{
	// TODO: Implement binary search
	float* element = nullptr;

	int index = 0;
	for (size_t i = 0; i < _Components.size() && element == nullptr; ++i)
	{
		if (_Components[i].first == _index)
			element = &_Data[index];
		index += _Components[i].second;
	}

	return element;
}


const float*
Point::operator [] (VertexComponent _index) const
{
	// TODO: Implement binary search
	const float* element = nullptr;

	int index = 0;
	for (size_t i = 0; i < _Components.size() && element == nullptr; ++i)
	{
		if (_Components[i].first == _index)
			element = &_Data[index];
		index += _Components[i].second;
	}

	return element;
}


void
Point::Serialize(std::fstream& _stream)
{
	int32_t componentCount = static_cast<int32_t>(_Components.size());
	_stream.write((char*)&componentCount, sizeof(componentCount));
	
	for (int i = 0; i < componentCount; ++i)
	{
		int16_t componentName = static_cast<int16_t>(_Components[i].first);
		int32_t componentSize = _Components[i].second;

		_stream.write((char*)&componentName, sizeof(componentName));
		_stream.write((char*)&componentSize, sizeof(componentSize));
	}

	_stream.write((char*)_Data.data(), _Size * sizeof(float));
}


void
Point::Deserialize(std::fstream& _stream)
{
	int32_t componentCount;
	_stream.read((char*)&componentCount, sizeof(componentCount));

	for (int i = 0; i < componentCount; ++i)
	{
		int16_t componentName;
		int32_t componentSize;

		_stream.read((char*)&componentName, sizeof(componentName));
		_stream.read((char*)&componentSize, sizeof(componentSize));

		Reserve((VertexComponent)componentName, componentSize);
	}

	_stream.read((char*)_Data.data(), _Size * sizeof(float));
}


