#include "MaterialData.hpp"

#include <fstream>
#include <cstdint>


auto
MaterialData::TexIDToString(MaterialData::TEXTURE_ID _id) -> std::string
{
	switch (_id)
	{
	case MAP_KA:
		return "map_Ka";
	case MAP_KD:
		return "map_Kd";
	case MAP_KS:
		return "map_Ks";
	case MAP_NS:
		return "map_Ns";
	case MAP_N:
		return "map_N";
	case DISP:
		return "disp";
	case DECAL:
		return "decal";
	case BUMP:
		return "bump";
	default:
		return "";
	}
}


auto
MaterialData::StringToTexID(const std::string& _source) -> MaterialData::TEXTURE_ID
{
	if (_source == "map_Ka")
		return MAP_KA;
	if (_source == "map_Kd")
		return MAP_KD;
	if (_source == "map_Ks")
		return MAP_KS;
	if (_source == "map_Ns")
		return MAP_NS;
	if (_source == "map_N")
		return MAP_N;
	if (_source == "disp")
		return DISP;
	if (_source == "decal")
		return DECAL;
	if (_source == "bump")
		return BUMP;

	return MaterialData::TEX_ID_COUNT;
}


auto
MaterialData::GetComponent(const std::string& _source) -> float*
{
	if (_source == "Ka")
		return Ka;
	if (_source == "Kd")
		return Kd;
	if (_source == "Ks")
		return Ks;
	if (_source == "Tf")
		return Tf;
	if (_source == "illum")
		return &illum;
	if (_source == "d")
		return &d;
	if (_source == "Ns")
		return &Ns;
	if (_source == "sharpness")
		return &sharpness;
	if (_source == "Ni")
		return &Ni;

	return nullptr;
}


auto
MaterialData::Serialize(std::fstream& _stream) -> void
{
	_stream.write((char*)&Ka, 3 * sizeof(float));
	_stream.write((char*)&Kd, 3 * sizeof(float));
	_stream.write((char*)&Ks, 3 * sizeof(float));
	_stream.write((char*)&Tf, 3 * sizeof(float));

	_stream.write((char*)&illum, sizeof(float));
	_stream.write((char*)&d, sizeof(float));
	_stream.write((char*)&Ns, sizeof(float));
	_stream.write((char*)&sharpness, sizeof(float));
	_stream.write((char*)&Ni, sizeof(float));

	for (int i = 0; i < TEX_ID_COUNT; ++i)
		WriteStringToStream(tex_maps[i], _stream);
}


auto
MaterialData::Deserialize(std::fstream& _stream) -> void
{
	_stream.read((char*)&Ka, 3 * sizeof(float));
	_stream.read((char*)&Kd, 3 * sizeof(float));
	_stream.read((char*)&Ks, 3 * sizeof(float));
	_stream.read((char*)&Tf, 3 * sizeof(float));

	_stream.read((char*)&illum, sizeof(float));
	_stream.read((char*)&d, sizeof(float));
	_stream.read((char*)&Ns, sizeof(float));
	_stream.read((char*)&sharpness, sizeof(float));
	_stream.read((char*)&Ni, sizeof(float));

	for (int i = 0; i < TEX_ID_COUNT; ++i)
		ReadStringFromStream(tex_maps[i], _stream);
}


auto	
MaterialData::WriteStringToStream(const std::string& _str, std::fstream& _stream) -> void
{
	uint64_t strSize = _str.size() + 1;
	_stream.write((char*)&strSize, sizeof(uint64_t));
	const char* cStr = _str.c_str() + '\0';
	_stream.write(cStr, strSize * sizeof(char));
}


auto	
MaterialData::ReadStringFromStream(std::string& _str, std::fstream& _stream) -> void
{
	uint64_t strSize = 0;
	_stream.read((char*)&strSize, sizeof(uint64_t));
	char* cStr = new char[(unsigned int)strSize];
	_stream.read(cStr, strSize * sizeof(char));
	_str = std::string(cStr);
	delete[] cStr;
}

