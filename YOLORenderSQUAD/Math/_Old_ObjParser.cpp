#include "_Old_ObjParser.hpp"

#include <boost/tokenizer.hpp>

#include <fstream>
#include <algorithm>
namespace old {

const std::regex
ObjParser::Object("o ((\\w* ?)+)");
const std::regex
ObjParser::Face("f (-?\\d+)\\/(-?\\d*)\\/(-?\\d*) (-?\\d+)\\/(-?\\d*)\\/(-?\\d*) (-?\\d+)\\/(-?\\d*)\\/(-?\\d*)");
const std::regex
ObjParser::VertexLine("v [\\s\\S]*");
const std::regex
ObjParser::TextureLine("vt [\\s\\S]*");
const std::regex
ObjParser::NormalLine("vn [\\s\\S]*");
const std::regex
ObjParser::FloatComponent("(-?\\d+(\\.\\d+(E-?\\d+)?)?)");


auto
ObjParser::ParseFile(std::string const& _path) -> void
{
	m_CurrentOBJ.Clear();
	m_WorkingData.m_Meshes.clear();
	m_WorkingData.m_Name = "";

	std::ifstream file(_path);
	// NOTE: file should be closed right away and its content put into a buffer

	std::string line;
	int lineIndex = 0;
	bSeparator separator(" ");

	m_NewMeshGroup();
	bool inFirstGroup = true;
	while (std::getline(file, line))
	{
		bTokenizer parser(line, separator);
		auto ite = parser.begin();
		bool success = false;

		if (!ite.at_end())
		{
			if (*ite == "v")
				success = m_ParseFloatGroup(m_CurrentOBJ, m_CurrentOBJ.m_PositionSize, m_CurrentOBJ.m_Positions, ++ite);
			if (*ite == "vt")
				success = m_ParseFloatGroup(m_CurrentOBJ, m_CurrentOBJ.m_TexCoordSize, m_CurrentOBJ.m_TexCoords, ++ite);
			if (*ite == "vn")
				success = m_ParseFloatGroup(m_CurrentOBJ, m_CurrentOBJ.m_NormalSize, m_CurrentOBJ.m_Normals, ++ite);
			if (*ite == "f")
			{
				success = m_ParseFace(m_CurrentOBJ, ++ite);
				if (success)
					m_ComputeFaceIndices(m_CurrentOBJ, *m_CurrentMeshData);
			}
			if (*ite == "o")
				success = m_ParseName(m_CurrentOBJ, ++ite);
			if (*ite == "g")
			{
				if (inFirstGroup) inFirstGroup = false;
				else
				{
					m_FinishCurrentGroup(_path);
					m_NewMeshGroup();
				}
				success = true;
			}
			if (*ite == "mtllib")
			{
				m_MtlParser.ParseFile(ExtractFolderFromPath(_path) + *++ite);
				success = true;
			}
			if (*ite == "usemtl")
				success = m_MtlParser.Get(*++ite, m_CurrentMeshData->m_Material);
		}

		if (!success)
		{
			//printf("ObjParser : Line %d did not match.\n", lineIndex);
		}
		if (lineIndex % 5000 == 0)
		{
			printf("ObjParser : Line %d \n", lineIndex);
		}

		lineIndex++;
	}
	m_FinishCurrentGroup(_path);

	file.close();
}


auto
ObjParser::GenerateMeshData(bool _computeNormalSpaces) -> MultiMeshData&
{
	if (_computeNormalSpaces)
		for (auto& mesh : m_WorkingData.m_Meshes)
			mesh.ComputeNormalSpaces();

	return m_WorkingData;
}


auto
ObjParser::ExtractFolderFromPath(std::string const& _path) -> std::string
{
	std::string result;

	bSeparator slash("", "\\/");
	bTokenizer parser(_path, slash);
	int tokenCount = 0;
	{
		bIterator ite = parser.begin();
		while (!ite.at_end())
		{
			tokenCount++;
			ite++;
		}
	}
	{
		result = "";
		bIterator ite = parser.begin();
		for (int i = 0; i < tokenCount - 1; ++i)
			result += *ite++;
	}

	return result;
}


auto
ObjParser::m_NewMeshGroup() -> void
{
	MeshData nextMeshData;

	m_WorkingData.m_Meshes.push_back(nextMeshData);
	m_CurrentMeshData = &m_WorkingData.m_Meshes[m_WorkingData.m_Meshes.size() - 1];
}


auto
ObjParser::m_FinishCurrentGroup(const std::string& _path) -> void
{
	auto positionSize = m_CurrentOBJ.m_PositionSize;
	auto texSize = m_CurrentOBJ.m_TexCoordSize;
	auto normalSize = m_CurrentOBJ.m_NormalSize;

	m_CurrentMeshData->m_VertexSize = m_CurrentMeshData->m_Points[0].m_Size;
	m_CurrentMeshData->m_VerticesCount = static_cast<int32_t>(m_CurrentMeshData->m_Points.size());
	m_CurrentMeshData->m_PolyCount = static_cast<int32_t>(m_CurrentMeshData->m_Indices.size()) / 3;
	m_CurrentMeshData->m_AttribSizes.push_back(positionSize);
	m_CurrentMeshData->m_AttribSizes.push_back(texSize);
	m_CurrentMeshData->m_AttribSizes.push_back(normalSize);
	m_CurrentMeshData->ExtractFolder(_path);
}


auto
ObjParser::m_SetPointData(float* _pointArray, int _index, int _componentSize, std::vector<float>& _componentArray) -> void
{
	if (_index != UNDEFINED_VALUE)
	{
		int localIndex = _index;
		if (localIndex <= 0) localIndex += (static_cast<int32_t>(_componentArray.size()) / _componentSize) + 1;

		int realIndex = (localIndex - 1) * _componentSize;
		memcpy(_pointArray, &_componentArray[realIndex], _componentSize * sizeof(float));
	}
}


auto
ObjParser::m_ComputeFaceIndices(OBJ& _obj, MeshData& _mesh) -> void
{
	auto indices = &_obj.m_Indices;
	auto positionSize = _obj.m_PositionSize;
	auto texSize = _obj.m_TexCoordSize;
	auto normalSize = _obj.m_NormalSize;

	int faceSize = 9;
	int faceIndex = ((int)(*indices).size() / faceSize) - 1;
	int valueIndex = faceSize * faceIndex;

	for (int i = valueIndex; i < valueIndex + faceSize; i += 3)
	{
		Point candidate(positionSize, texSize, normalSize);

		m_SetPointData(candidate.m_Position, (*indices)[i], positionSize, _obj.m_Positions);
		m_SetPointData(candidate.m_Texture, (*indices)[i + 1], texSize, _obj.m_TexCoords);
		m_SetPointData(candidate.m_Normal, (*indices)[i + 2], normalSize, _obj.m_Normals);

		int index;
		bool found = false;
		for (index = 0; index < (int)_mesh.m_Points.size() && !found; ++index)
		{
			found = (_mesh.m_Points[index] == candidate);
		}

		if (found)
			_mesh.m_Indices.push_back(index - 1);
		else
		{
			_mesh.m_Points.push_back(candidate);
			_mesh.m_Indices.push_back(static_cast<uint32_t>(_mesh.m_Points.size()) - 1);
		}
	}
}


auto
ObjParser::m_ParseName(OBJ& _currentOBJ, bIterator _ite) -> bool
{
	std::string name("");

	while (!_ite.at_end())
		name += *_ite++;

	_currentOBJ.m_Name = name;

	return true;
}


auto
ObjParser::m_ParseFace(OBJ& _currentOBJ, bIterator _ite) -> bool
{
	int i = 0;
	while (!_ite.at_end() && i < 3)
	{
		bSeparator slashSep("/", "", boost::keep_empty_tokens);
		bTokenizer faceParser(*_ite, slashSep);

		/*
		bIterator index = faceParser.begin();
		for (int j = 0; j < 3; ++j)
			_currentOBJ.m_Indices.push_back(std::stoi(*index));
		*/
		for (auto& index : faceParser)
		{
			if (index == "")
				_currentOBJ.m_Indices.push_back(UNDEFINED_VALUE);
			else
				_currentOBJ.m_Indices.push_back(std::stoi(index));
		}

		_ite++;
		++i;
	}

	return true;
}


auto
ObjParser::m_ParseFloatGroup(OBJ& _currentOBJ, int& _componentSize, std::vector<float>& _componentArray, bIterator _ite) -> bool
{
	/*
	int match_count = 0;
	{
		auto itecpy(_ite);
		while (!itecpy.at_end())
		{
			itecpy++; match_count++;
		}
	}

	if (_componentSize == UNDEFINED_VALUE)
		_componentSize = (int)match_count;
	else
		if ((int)match_count != _componentSize)
			return false;
	*/

	int i = 0;
	while (!_ite.at_end() && i < _componentSize)
	{
		_componentArray.push_back(std::stof(*_ite++));
		++i;
	}

	while (i < _componentSize)
	{
		_componentArray.push_back(0.f);
		++i;
	}

	return true;
}


auto
ObjParser::m_Regex_ParseName(std::smatch const& _match) -> bool
{
	m_CurrentOBJ.m_Name = _match[1].str();

	return true;
}


auto
ObjParser::m_Regex_ParseFace(std::smatch const& _match) -> bool
{
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 1; j < 4; ++j)
		{
			std::string matchString = _match[j + i * 3].str();
			if (matchString == "")
				m_CurrentOBJ.m_Indices.push_back(UNDEFINED_VALUE);
			else
				m_CurrentOBJ.m_Indices.push_back(std::stoi(matchString));
		}
	}

	return true;
}


auto
ObjParser::m_Regex_ParseVertex(std::string const& _line) -> bool
{
	std::ptrdiff_t match_count(std::distance(std::sregex_iterator(_line.begin(), _line.end(), FloatComponent),
		std::sregex_iterator()));
	if (m_CurrentOBJ.m_PositionSize == UNDEFINED_VALUE)
		m_CurrentOBJ.m_PositionSize = (int)match_count;
	else
	{
		if ((int)match_count != m_CurrentOBJ.m_PositionSize)
			return false;
	}

	for (auto ite = std::sregex_iterator(_line.begin(), _line.end(), FloatComponent);
	ite != std::sregex_iterator(); ++ite)
	{
		m_CurrentOBJ.m_Positions.push_back(std::stof(ite->str()));
	}

	return true;
}


auto
ObjParser::m_Regex_ParseTexture(std::string const& _line) -> bool
{
	std::ptrdiff_t match_count(std::distance(std::sregex_iterator(_line.begin(), _line.end(), FloatComponent),
		std::sregex_iterator()));
	if (m_CurrentOBJ.m_TexCoordSize == UNDEFINED_VALUE)
		m_CurrentOBJ.m_TexCoordSize = (int)match_count;
	else
	{
		if ((int)match_count != m_CurrentOBJ.m_TexCoordSize)
			return false;
	}

	for (auto ite = std::sregex_iterator(_line.begin(), _line.end(), FloatComponent);
	ite != std::sregex_iterator(); ++ite)
	{
		m_CurrentOBJ.m_TexCoords.push_back(std::stof(ite->str()));
	}

	return true;
}


auto
ObjParser::m_Regex_ParseNormal(std::string const& _line) -> bool
{
	std::ptrdiff_t match_count(std::distance(std::sregex_iterator(_line.begin(), _line.end(), FloatComponent),
		std::sregex_iterator()));
	if (m_CurrentOBJ.m_NormalSize == UNDEFINED_VALUE)
		m_CurrentOBJ.m_NormalSize = (int)match_count;
	else
	{
		if ((int)match_count != m_CurrentOBJ.m_NormalSize)
			return false;
	}

	for (auto ite = std::sregex_iterator(_line.begin(), _line.end(), FloatComponent);
	ite != std::sregex_iterator(); ++ite)
	{
		m_CurrentOBJ.m_Normals.push_back(std::stof(ite->str()));
	}

	return true;
}


} // old