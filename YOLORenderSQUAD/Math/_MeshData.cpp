#include "_MeshData.hpp"

#include <fstream>
#include <cstdint>

#include "BoostTokHelper.hpp"

#include "Vec3.hpp"

namespace refactor {


auto
MeshData::ComputeNormalSpaces() -> void
{
	
}


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
	stream.close();
}


auto
MeshData::Serialize(std::fstream& _stream) -> void
{
	int64_t pointsCount = static_cast<int64_t>(m_Points.size());
	_stream.write((char*)&pointsCount, sizeof(int64_t));
	for (auto&& point : m_Points)
		point.Serialize(_stream);

	int64_t indicesCount;
	indicesCount = static_cast<int64_t>(m_Indices.size());
	_stream.write((char*)&indicesCount, sizeof(int64_t));
	_stream.write((char*)m_Indices.data(), indicesCount * sizeof(uint32_t));

	int32_t attribCount;
	attribCount = static_cast<int32_t>(m_AttribSizes.size());
	_stream.write((char*)&attribCount, sizeof(int32_t));
	_stream.write((char*)m_AttribSizes.data(), attribCount * sizeof(int32_t));

	_stream.write((char*)&(m_VerticesCount), sizeof(int32_t));
	_stream.write((char*)&(m_VertexSize), sizeof(int32_t));
	_stream.write((char*)&(m_PolyCount), sizeof(int32_t));

	m_Material.Serialize(_stream);
}


auto
MeshData::Deserialize(const std::string& _path) -> void
{
	std::fstream stream(_path, std::fstream::in | std::fstream::binary);
	Deserialize(stream);
	ExtractFolder(_path);
	stream.close();
}


auto
MeshData::Deserialize(std::fstream& _stream) -> void
{
	int64_t pointsCount = 0;
	_stream.read((char*)&pointsCount, sizeof(int64_t));
	for (int64_t i = 0; i < pointsCount; ++i)
	{
		Point candidate;
		candidate.Deserialize(_stream);
		m_Points.push_back(candidate);
	}

	int64_t indicesCount = 0;
	_stream.read((char*)&indicesCount, sizeof(int64_t));
	m_Indices.resize((size_t)indicesCount);
	_stream.read((char*)m_Indices.data(), indicesCount * sizeof(uint32_t));

	int32_t attribCount = 0;
	_stream.read((char*)&attribCount, sizeof(int32_t));
	m_AttribSizes.resize((size_t)attribCount);
	_stream.read((char*)m_AttribSizes.data(), attribCount * sizeof(int32_t));

	_stream.read((char*)&m_VerticesCount, sizeof(int32_t));
	_stream.read((char*)&m_VertexSize, sizeof(int32_t));
	_stream.read((char*)&m_PolyCount, sizeof(int32_t));

	m_Material.Deserialize(_stream);
}


auto
MultiMeshData::Serialize(const std::string& _path) -> void
{
	std::fstream stream(_path, std::fstream::out | std::fstream::trunc | std::fstream::binary);

	int32_t meshesCount = static_cast<int32_t>(m_Meshes.size());
	stream.write((char*)&meshesCount, sizeof(int32_t));
	for (int32_t i = 0; i < meshesCount; ++i)
		m_Meshes[i].Serialize(stream);

	stream.close();
}


auto
MultiMeshData::Deserialize(const std::string& _path) -> void
{
	m_Meshes.clear();
	std::fstream stream(_path, std::fstream::in | std::fstream::binary);

	int32_t meshesCount = 0;
	stream.read((char*)&meshesCount, sizeof(int32_t));
	m_Meshes.resize(meshesCount);
	for (int32_t i = 0; i < meshesCount; ++i)
	{
		m_Meshes[i].Deserialize(stream);
		m_Meshes[i].ExtractFolder(_path);
	}

	stream.close();
}



} // refactor

