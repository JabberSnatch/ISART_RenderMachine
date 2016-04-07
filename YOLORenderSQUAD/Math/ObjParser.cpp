#include "ObjParser.hpp"

#include <fstream>
#include <sstream>
#include <array>

#include "Point.hpp"


void
ObjParser::ParseFile(std::string const& _path)
{
	m_OBJStorage.Clear();
	m_WorkingData.m_Meshes.clear();
	m_WorkingData.m_Name = "";

	std::ifstream file(_path);
	if (!file.good()) return;

	std::stringstream contents;
	{
		int length = 0;
		file.seekg(0, std::ios::end);
		length = (int)file.tellg();
		file.seekg(0, std::ios::beg);
		char* buffer = new char[length];
		file.read(buffer, length);
		file.close();
		contents.str(std::string(buffer));
	}

	std::string line;
	int lineIndex = 0;
	bSeparator space(" ");
	NewMeshGroup();
	bool inFirstGroup = true;

	while (std::getline(contents, line))
	{
		bTokenizer parser(line, space);
		bTokenizer::iterator ite = parser.begin();
		bool success = false;

		if (!ite.at_end())
		{
			if (*ite == "v")
				success = GetFloatGroup(++ite, m_OBJStorage._PositionSize, m_OBJStorage._Positions);
			else if (*ite == "vt")
				success = GetFloatGroup(++ite, m_OBJStorage._TexCoordSize, m_OBJStorage._TexCoords);
			else if (*ite == "vn")
				success = GetFloatGroup(++ite, m_OBJStorage._NormalSize, m_OBJStorage._Normals);
			else if (*ite == "f")
			{
				success = GetFace(++ite);
				if (success)
					ComputeFaceIndices(m_CurrentMeshData);
			}
			else if (*ite == "g")
			{
				if (!inFirstGroup)
				{
					FinishCurrentGroup(_path);
					NewMeshGroup();
				}
				else inFirstGroup = false;
				success = true;
			}
			else if (*ite == "mtllib")
			{
				// TODO: Ensure that materials are parsed the right way, especially for texture files paths
				m_MtlParser.ParseFile(ExtractFolderFromPath(_path) + *++ite);
				success = true;
			}
			else if (*ite == "usemtl")
				success = m_MtlParser.Get(*++ite, m_CurrentMeshData->m_Material);
		}

		if (lineIndex % 5000 == 0)
			printf("ObjParser : Line %d \n", lineIndex);

		lineIndex++;
	}
	FinishCurrentGroup(_path);
}


MultiMeshData&
ObjParser::GenerateMeshData(bool _computeNormalSpaces)
{
	if (_computeNormalSpaces)
	{
		for (auto& mesh : m_WorkingData.m_Meshes)
			mesh.ComputeNormalSpaces();
	}

	return m_WorkingData;
}


std::string
ObjParser::ExtractFolderFromPath(std::string const& _path)
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


void
ObjParser::NewMeshGroup()
{
	MeshData nextMeshData;
	m_WorkingData.m_Meshes.push_back(nextMeshData);
	m_CurrentMeshData = &m_WorkingData.m_Meshes[m_WorkingData.m_Meshes.size() - 1];
}


void
ObjParser::FinishCurrentGroup(std::string const& _path)
{
	auto positionSize = m_OBJStorage._PositionSize;
	auto texSize = m_OBJStorage._TexCoordSize;
	auto normalSize = m_OBJStorage._NormalSize;

	m_CurrentMeshData->m_VertexSize = 0;
	if (m_CurrentMeshData->m_Points.size() > 0)
		m_CurrentMeshData->m_VertexSize = m_CurrentMeshData->m_Points[0]._Size;
	
	m_CurrentMeshData->m_VerticesCount = static_cast<int32_t>(m_CurrentMeshData->m_Points.size());
	m_CurrentMeshData->m_PolyCount = static_cast<int32_t>(m_CurrentMeshData->m_Indices.size()) / 3;
	
	m_CurrentMeshData->m_AttribSizes.push_back(positionSize);
	m_CurrentMeshData->m_AttribSizes.push_back(texSize);
	m_CurrentMeshData->m_AttribSizes.push_back(normalSize);
	m_CurrentMeshData->ExtractFolder(_path);
}


void
ObjParser::ComputeFaceIndices(MeshData* _mesh)
{
	std::vector<int32_t>&	indices = m_OBJStorage._Indices;
	int32_t					positionSize = m_OBJStorage._PositionSize;
	int32_t					texCoordSize = m_OBJStorage._TexCoordSize;
	int32_t					normalSize = m_OBJStorage._NormalSize;
	int32_t					positionCount = (int32_t)m_OBJStorage._Positions.size() / positionSize;
	int32_t					texCoordCount = (int32_t)m_OBJStorage._TexCoords.size() / texCoordSize;
	int32_t					normalCount = (int32_t)m_OBJStorage._Normals.size() / normalSize;

	// NOTE: At this point it's assumed that every vertex is 9 float long.
	// Getting the faces's indices array offset from the current number of indices
	int faceSize = 9;
	int faceIndex = ((int)indices.size() / faceSize) - 1;
	int valueIndex = faceSize * faceIndex;

	for (int i = valueIndex; i < valueIndex + faceSize; i += 3)
	{
		Point		candidate(positionSize, texCoordSize, normalSize);
		int32_t		positionIndex = ComputeArrayOffset(indices[i], positionSize, positionCount);
		int32_t		texCoordIndex = ComputeArrayOffset(indices[i + 1], texCoordSize, texCoordCount);
		int32_t		normalIndex = ComputeArrayOffset(indices[i + 2], normalSize, normalCount);
		
		if (positionIndex != UNDEFINED_VALUE)
			candidate.Set(Point::POSITION, &m_OBJStorage._Positions[positionIndex]);
		if (texCoordIndex != UNDEFINED_VALUE)
			candidate.Set(Point::TEXTURE, &m_OBJStorage._TexCoords[texCoordIndex]);
		if (normalIndex != UNDEFINED_VALUE)
			candidate.Set(Point::NORMAL, &m_OBJStorage._Normals[normalIndex]);

		int		index;
		bool	found = false;
		for (index = 0; index < (int)_mesh->m_Points.size() && !found; ++index)
			found = (_mesh->m_Points[index] == candidate);

		if (found)
			_mesh->m_Indices.push_back(index - 1);
		else
		{
			_mesh->m_Points.push_back(candidate);
			_mesh->m_Indices.push_back(static_cast<uint32_t>(_mesh->m_Points.size()) - 1);
		}
	}
}


int32_t
ObjParser::ComputeArrayOffset(int32_t _readIndex, int32_t _componentSize, int32_t _componentCount)
{
	if (_readIndex != UNDEFINED_VALUE)
	{
		int32_t arrayOffset;
		if (_readIndex <= 0)
			arrayOffset = (_readIndex + _componentCount + 1) * _componentSize;
		else
			arrayOffset = (_readIndex - 1) * _componentSize;
		return arrayOffset;
	}
	else
		return _readIndex;
}


bool
ObjParser::GetFace(bIterator _ite)
{
	bSeparator	slash("/", "", boost::keep_empty_tokens);
	int			i = 0;
	
	std::vector<std::array<int32_t, 3>>	readFace;

	while (!_ite.at_end())
	{
		bTokenizer	faceParser(*_ite, slash);
		bIterator	readIndex = faceParser.begin();

		readFace.resize(readFace.size() + 1);
		for (int i = 0; i < 3; ++i)
		{
			int32_t	storedIndex = UNDEFINED_VALUE;

			if (!readIndex.at_end())
				if (*readIndex != "")
					storedIndex = (int32_t)std::stoi(*readIndex);

			readFace.back()[i] = storedIndex;
			++readIndex;
		}
		++_ite;
	}
	if (readFace.size() < 3) return false;

	// Triangularize any non-triangle face as a triangle fan
	std::array<int32_t, 3>		pivot = readFace[0];
	for (int i = 1; i <= readFace.size() - 2; ++i)
	{
		std::array<int32_t, 3>	first = readFace[i];
		std::array<int32_t, 3>	second = readFace[i + 1];

		for (int i = 0; i < 3; ++i)
			m_OBJStorage._Indices.push_back(pivot[i]);
		for (int i = 0; i < 3; ++i)
			m_OBJStorage._Indices.push_back(first[i]);
		for (int i = 0; i < 3; ++i)
			m_OBJStorage._Indices.push_back(second[i]);
	}
	return true;
}


bool
ObjParser::GetFloatGroup(bIterator _ite, int& _componentSize, std::vector<float>& _componentArray)
{
	// NOTE: The process relies on the fact that we now beforehand what the components sizes will be.
	//		 The other way to do it would be to base the component size on the first float group read.
	//	     It would mean that we would have to loop twice over the float group. 
	//       Once to check if it matches the stored size, and a second time to actually store values.

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

