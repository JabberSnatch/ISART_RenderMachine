#include "ObjParser.hpp"

#include <fstream>

auto
MeshData::ExtractFolder(const std::string& _path) -> void
{
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
		m_Folder = "";
		bIterator ite = parser.begin();
		for (int i = 0; i < tokenCount - 1; ++i)
			m_Folder += *ite++;
	}
}


auto
MeshData::Serialize(const std::string& _path) -> void
{
	std::fstream stream(_path, std::fstream::out | std::fstream::trunc | std::fstream::binary);
	Serialize(stream);
}


auto
MeshData::Serialize(std::fstream& _stream) -> void
{
	size_t pointsSize = m_Points.size();
	_stream.write((char*)&pointsSize, sizeof(size_t));
	for (auto&& point : m_Points)
	{
		ptrdiff_t posSize, texSize, normSize;
		posSize = point.m_Texture - point.m_Position;
		texSize = point.m_Normal - point.m_Texture;
		normSize = point.m_Size - (point.m_Normal - point.m_Position);

		_stream.write((char*)&posSize, sizeof(ptrdiff_t));
		_stream.write((char*)&texSize, sizeof(ptrdiff_t));
		_stream.write((char*)&normSize, sizeof(ptrdiff_t));

		_stream.write((char*)point.m_Position, point.m_Size * sizeof(float));
	}

	size_t indicesSize;
	indicesSize = m_Indices.size();
	_stream.write((char*)&indicesSize, sizeof(size_t));
	_stream.write((char*)m_Indices.data(), indicesSize * sizeof(unsigned int));

	size_t attribSize;
	attribSize = m_AttribSizes.size();
	_stream.write((char*)&attribSize, sizeof(size_t));
	_stream.write((char*)m_AttribSizes.data(), attribSize * sizeof(int));

	_stream.write((char*)&(m_VerticesCount), sizeof(m_VerticesCount));
	_stream.write((char*)&(m_VertexSize), sizeof(m_VertexSize));
	_stream.write((char*)&(m_PolyCount), sizeof(m_PolyCount));

	m_Material.Serialize(_stream);
}


auto
MeshData::Deserialize(const std::string& _path) -> void
{
	std::fstream stream(_path, std::fstream::in | std::fstream::binary);
	Deserialize(stream);
	ExtractFolder(_path);
}


auto
MeshData::Deserialize(std::fstream& _stream) -> void
{
	int pointsCount = 0;
	_stream.read((char*)&pointsCount, sizeof(size_t));
	for (int i = 0; i < pointsCount; ++i)
	{
		ptrdiff_t posSize = 0, normSize = 0, texSize = 0;
		_stream.read((char*)&posSize, sizeof(ptrdiff_t));
		_stream.read((char*)&texSize, sizeof(ptrdiff_t));
		_stream.read((char*)&normSize, sizeof(ptrdiff_t));

		Point candidate(posSize, texSize, normSize);
		_stream.read((char*)candidate.m_Position, candidate.m_Size * sizeof(candidate.m_Position));

		m_Points.push_back(candidate);
	}

	size_t indicesCount = 0, attribCount = 0;
	_stream.read((char*)&indicesCount, sizeof(size_t));
	m_Indices.resize(indicesCount);
	_stream.read((char*)m_Indices.data(), indicesCount * sizeof(unsigned int));

	_stream.read((char*)&attribCount, sizeof(size_t));
	m_AttribSizes.resize(attribCount);
	_stream.read((char*)m_AttribSizes.data(), attribCount * sizeof(int));

	_stream.read((char*)&m_VerticesCount, sizeof(m_VerticesCount));
	_stream.read((char*)&m_VertexSize, sizeof(m_VertexSize));
	_stream.read((char*)&m_PolyCount, sizeof(m_PolyCount));

	m_Material.Deserialize(_stream);
}


auto
MultiMeshData::Serialize(const std::string& _path) -> void
{
	std::fstream stream(_path, std::fstream::out | std::fstream::trunc | std::fstream::binary);

	int meshesCount = m_Meshes.size();
	stream.write((char*)&meshesCount, sizeof(int));
	for (int i = 0; i < meshesCount; ++i)
		m_Meshes[i].Serialize(stream);
}


auto
MultiMeshData::Deserialize(const std::string& _path) -> void
{
	std::fstream stream(_path, std::fstream::in | std::fstream::binary);

	int meshesCount = 0;
	stream.read((char*)&meshesCount, sizeof(int));
	m_Meshes.resize(meshesCount);
	for (int i = 0; i < meshesCount; ++i)
	{
		m_Meshes[i].Deserialize(stream);
		m_Meshes[i].ExtractFolder(_path);
	}
}

