#ifndef __MATERIAL_DATA_HPP__
#define __MATERIAL_DATA_HPP__

#include <string>


struct MaterialData
{
	enum TEXTURE_ID
	{
		MAP_KA,
		MAP_KD,
		MAP_KS,
		MAP_NS,
		MAP_N,
		DISP,
		DECAL,
		BUMP,

		TEX_ID_COUNT
	};

	float Ka[3] = { 0.f, 0.f, 0.f };
	float Kd[3] = { 0.f, 0.f, 0.f };
	float Ks[3] = { 0.f, 0.f, 0.f };
	float Tf[3] = { 0.f, 0.f, 0.f };
	float illum = 0.f;
	float d = 0.f;
	float Ns = 0.f;
	float sharpness = 0.f;
	float Ni = 0.f;

	std::string tex_maps[TEX_ID_COUNT];

	auto static TexIDToString(TEXTURE_ID _id) -> std::string;
	auto static StringToTexID(const std::string& _source) -> TEXTURE_ID;

	auto	GetComponent(const std::string& _source) -> float*;

	auto	Serialize(std::fstream& _stream) -> void;
	auto	Deserialize(std::fstream& _stream) -> void;

private:
	auto	WriteStringToStream(const std::string& _str, std::fstream& _stream) -> void;
	auto	ReadStringFromStream(std::string& _str, std::fstream& _stream) -> void;
};

#endif /*__MATERIAL_DATA_HPP__*/