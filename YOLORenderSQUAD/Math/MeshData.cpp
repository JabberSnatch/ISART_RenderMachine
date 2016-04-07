#include "MeshData.hpp"

#include <fstream>
#include <cstdint>

#include "BoostTokHelper.hpp"

#include "Vec3.hpp"


auto
MeshData::ComputeNormalSpaces() -> void
{
	for (size_t face = 0; face < m_Indices.size(); face += 3)
	{
		for (int point = 0; point < 3; ++point)
		{
			Point& target = m_Points[m_Indices[face + point]];
			target.Reserve(Point::TANGENT, 3);
			target.Reserve(Point::BITANGENT, 3);

			Point& next = m_Points[m_Indices[face + ((point + 1) % 3)]];
			Point& previous = m_Points[m_Indices[face + ((point + 2) % 3)]];
			//Point& next = m_Points[m_Indices[face + ((point + 1) % 3)]];
			//Point& previous = m_Points[m_Indices[face + ((point + 2) % 3)]];

			{
				Vec3 v0(target[Point::POSITION][0], target[Point::POSITION][1], target[Point::POSITION][2]);
				Vec3 v1(next[Point::POSITION][0], next[Point::POSITION][1], next[Point::POSITION][2]);
				Vec3 v2(previous[Point::POSITION][0], previous[Point::POSITION][1], previous[Point::POSITION][2]);

				Vec3 uv0(target[Point::TEXTURE][0], target[Point::TEXTURE][1], 0.f);
				Vec3 uv1(next[Point::TEXTURE][0], next[Point::TEXTURE][1], 0.f);
				Vec3 uv2(previous[Point::TEXTURE][0], previous[Point::TEXTURE][1], 0.f);

				Vec3 dPosition1 = v1 - v0;
				Vec3 dPosition2 = v2 - v0;
				Vec3 dUV1 = uv1 - uv0;
				Vec3 dUV2 = uv2 - uv0;

				float r = (dUV1.x * dUV2.y - dUV1.y * dUV2.x);

				Vec3 T = (dPosition1 * dUV2.y - dPosition2 * dUV1.y) / r;
				Vec3 B = (dPosition2 * dUV1.x - dPosition1 * dUV2.x) / r;

				if (T.x == 0.f && T.y == 0.f && T.z == 0.f)
					T = Vec3(1.0f, 0.f, 0.f);

				Vec3 t(target[Point::TANGENT][0], target[Point::TANGENT][1], target[Point::TANGENT][2]);
				Vec3 b(target[Point::BITANGENT][0], target[Point::BITANGENT][1], target[Point::BITANGENT][2]);

				T += t;
				B += b;

				target.Set(Point::TANGENT, 3, T.ToArray().get());
				target.Set(Point::BITANGENT, 3, B.ToArray().get());

				//T = Vec3(1.f, 0.f, 0.f);
				//B = T;
			}
		}
	}

	for (int i = 0; i < m_Points.size(); ++i)
	{
		Point& target = m_Points[i];

		Vec3 N(target[Point::NORMAL][0], target[Point::NORMAL][1], target[Point::NORMAL][2]);
		Vec3 B(target[Point::BITANGENT][0], target[Point::BITANGENT][1], target[Point::BITANGENT][2]);
			
		Vec3 T = (T - N * N.dot(T)).normalized();
		float signB = (N.cross(T)).dot(B) < 0.f ? -1.f : 1.f;
		B = (N.cross(T)).normalized() * signB;

		target.Set(Point::TANGENT, 3, T.ToArray().get());
		target.Set(Point::BITANGENT, 3, B.ToArray().get());
	}

	m_VertexSize = m_Points[0]._Size;
	m_AttribSizes.push_back(3);
	m_AttribSizes.push_back(3);
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


