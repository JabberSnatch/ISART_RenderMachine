#include "MaterialData.hpp"

#include <fstream>


auto
MaterialData::Serialize(std::fstream& _stream) -> void
{
	_stream.write((char*)&Ka, 3 * sizeof(float));
	_stream.write((char*)&Kd, 3 * sizeof(float));
	_stream.write((char*)&Ks, 3 * sizeof(float));
	_stream.write((char*)&Tf, 3 * sizeof(float));

	_stream.write((char*)&illum, sizeof(float));
	_stream.write((char*)&Ns, sizeof(float));
	_stream.write((char*)&sharpness, sizeof(float));
	_stream.write((char*)&Ni, sizeof(float));

	WriteStringToStream(map_Ka, _stream);
	WriteStringToStream(map_Kd, _stream);
	WriteStringToStream(map_Ks, _stream);
	WriteStringToStream(map_Ns, _stream);
	WriteStringToStream(map_d, _stream);
	WriteStringToStream(disp, _stream);
	WriteStringToStream(decal, _stream);
	WriteStringToStream(bump, _stream);
}


auto
MaterialData::Deserialize(std::fstream& _stream) -> void
{
	_stream.read((char*)&Ka, 3 * sizeof(float));
	_stream.read((char*)&Kd, 3 * sizeof(float));
	_stream.read((char*)&Ks, 3 * sizeof(float));
	_stream.read((char*)&Tf, 3 * sizeof(float));

	_stream.read((char*)&illum, sizeof(float));
	_stream.read((char*)&Ns, sizeof(float));
	_stream.read((char*)&sharpness, sizeof(float));
	_stream.read((char*)&Ni, sizeof(float));

	ReadStringFromStream(map_Ka, _stream);
	ReadStringFromStream(map_Kd, _stream);
	ReadStringFromStream(map_Ks, _stream);
	ReadStringFromStream(map_Ns, _stream);
	ReadStringFromStream(map_d, _stream);
	ReadStringFromStream(disp, _stream);
	ReadStringFromStream(decal, _stream);
	ReadStringFromStream(bump, _stream);
}


auto	
MaterialData::WriteStringToStream(const std::string& _str, std::fstream& _stream) -> void
{
	size_t strSize = _str.size() + 1;
	_stream.write((char*)&strSize, sizeof(size_t));
	const char* cStr = _str.c_str() + '\0';
	_stream.write(cStr, strSize * sizeof(char));
}


auto	
MaterialData::ReadStringFromStream(std::string& _str, std::fstream& _stream) -> void
{
	size_t strSize = 0;
	_stream.read((char*)&strSize, sizeof(size_t));
	char* cStr = new char[strSize];
	_stream.read(cStr, strSize * sizeof(char));
	_str = std::string(cStr);
	delete[] cStr;
}

