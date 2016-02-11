#include "MtlParser.hpp"

#include <fstream>



auto
MtlParser::ParseFile(const std::string& _path) -> void
{
	std::fstream stream(_path, std::fstream::in);

	std::string line;
	int lineIndex = 0;
	bSeparator space(" ");

	while (std::getline(stream, line))
	{
		bTokenizer lineTokenizer(line, space);
		auto ite = lineTokenizer.begin();
		bool success = false;

		if (!ite.at_end())
		{
			if (*ite == "newmtl")
				success = m_NewMaterial(++ite);
			if (*ite == "map_Ka" || 
				*ite == "map_Kd" ||
				*ite == "map_Ks" ||
				*ite == "map_Ns" ||
				*ite == "map_N" ||
				*ite == "map_d" ||
				*ite == "disp" ||
				*ite == "decal" ||
				*ite == "bump")
			{
				MaterialData::TEXTURE_ID texID = MaterialData::StringToTexID(*ite);
				m_CurrentMaterial->tex_maps[texID] = *++ite; 
				success = true;
			}
			if (*ite == "Ka" ||
				*ite == "Kd" ||
				*ite == "Ks" ||
				*ite == "Tf")
			{
				float* component = m_CurrentMaterial->GetComponent(*ite);
				success = m_ParseColor(component, ++ite);
			}
			if (*ite == "illum" ||
				*ite == "d" ||
				*ite == "Ns" ||
				*ite == "sharpness" ||
				*ite == "Ni")
			{
				float* component = m_CurrentMaterial->GetComponent(*ite);
				success = m_ParseFloat(component, ++ite);
			}
		}

		if (!success)
			printf("MtlParser : Line %d did not match.\n", lineIndex);

		lineIndex++;
	}
}


auto
MtlParser::Get(const std::string& _material, MaterialData& o_Material) -> bool
{
	auto result = m_Materials.find(_material);
	if (result == m_Materials.end())
		return false;
	else
	{
		o_Material = result->second;
		return true;
	}
}


auto
MtlParser::m_NewMaterial(bIterator _ite) -> bool
{
	MaterialData nextData;
	auto pair = m_Materials.emplace(*_ite, nextData);
	if (pair.second)
		m_CurrentMaterial = &pair.first->second;

	return pair.second;
}


auto
MtlParser::m_ParseColor(float* _colorArray, bIterator _ite) -> bool
{
	for (int i = 0; i < 3 && !_ite.at_end(); ++i)
		_colorArray[i] = std::stof(*_ite++);

	return true;
}


auto
MtlParser::m_ParseFloat(float* _floatField, bIterator _ite) -> bool
{
	*_floatField = std::stof(*_ite++);

	return true;
}

