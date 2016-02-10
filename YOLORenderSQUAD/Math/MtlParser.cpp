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
			if (*ite == "map_Ka")
			{
				m_CurrentMaterial->map_Ka = *++ite; 
				success = true;
			}
			if (*ite == "map_Kd")
			{
				m_CurrentMaterial->map_Kd = *++ite;
				success = true;
			}
			if (*ite == "map_Ks")
			{
				m_CurrentMaterial->map_Ks = *++ite;
				success = true;
			}
			if (*ite == "map_Ns")
			{
				m_CurrentMaterial->map_Ns = *++ite;
				success = true;
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
