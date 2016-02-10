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
		DISP,
		DECAL,
		BUMP,

		TEX_ID_COUNT
	};

	float Ka[3] = { 1.f, 1.f, 1.f };
	float Kd[3] = { 1.f, 1.f, 1.f };
	float Ks[3] = { 1.f, 1.f, 1.f };
	float Tf[3] = { 1.f, 1.f, 1.f };
	float illum = 1.f;
	float Ns = 10;
	float sharpness = 1;
	float Ni = 1;

	std::string map_Ka = "";
	std::string map_Kd = "";
	std::string map_Ks = "";
	std::string map_Ns = "";
	std::string map_d = "";
	std::string disp = "";
	std::string decal = "";
	std::string bump = "";

	auto	Serialize(std::fstream& _stream) -> void;
	auto	Deserialize(std::fstream& _stream) -> void;

private:
	auto	WriteStringToStream(const std::string& _str, std::fstream& _stream) -> void;
	auto	ReadStringFromStream(std::string& _str, std::fstream& _stream) -> void;
};

#endif /*__MATERIAL_DATA_HPP__*/