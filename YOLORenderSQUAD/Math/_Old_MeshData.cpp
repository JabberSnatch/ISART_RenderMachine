#include "_Old_MeshData.hpp"

#include <fstream>
#include <cstdint>

#include "BoostTokHelper.hpp"

#include "Vec3.hpp"

namespace old {

auto
MeshData::ComputeNormalSpaces() -> void
{
	// NOTE: Compute Tangent and Bitangent average
	// http://www.terathon.com/code/tangent.html

#if 0
	int vertexCount = m_Points.size();

	Vec3* tangents = new Vec3[vertexCount * 2];
	Vec3* bitangents = tangents + vertexCount;
	memset(tangents, 0, sizeof(Vec3) * vertexCount * 2);

	for (size_t face = 0; face < m_PolyCount; ++face)
	{
		int i0 = m_Indices[face * 3];
		int i1 = m_Indices[face * 3 + 1];
		int i2 = m_Indices[face * 3 + 2];

		Point target = m_Points[i0];
		Point next = m_Points[i1];
		Point previous = m_Points[i2];

		{
			Vec3 v0(target.m_Position[0], target.m_Position[1], target.m_Position[2]);
			Vec3 v1(next.m_Position[0], next.m_Position[1], next.m_Position[2]);
			Vec3 v2(previous.m_Position[0], previous.m_Position[1], previous.m_Position[2]);

			Vec3 uv0(target.m_Texture[0], target.m_Texture[1], target.m_Texture[2]);
			Vec3 uv1(next.m_Texture[0], next.m_Texture[1], next.m_Texture[2]);
			Vec3 uv2(previous.m_Texture[0], previous.m_Texture[1], previous.m_Texture[2]);

			Vec3 dPosition1 = v1 - v0;
			Vec3 dPosition2 = v2 - v0;
			Vec3 dUV1 = uv1 - uv0;
			Vec3 dUV2 = uv2 - uv0;

			float r = 1.0f / (dUV1.x * dUV2.y - dUV1.y * dUV2.x);

			Vec3 T = ((dPosition1 * dUV2.y - dPosition2 * dUV1.y) * r);
			Vec3 B = ((dPosition2 * dUV1.x - dPosition1 * dUV2.x) * r);

			tangents[i0] += T;
			tangents[i1] += T;
			tangents[i2] += T;

			bitangents[i0] += B;
			bitangents[i1] += B;
			bitangents[i2] += B;
		}
	}

	for (int vertex = 0; vertex < vertexCount; ++vertex)
	{
		Point target = m_Points[vertex];

		Point result(target.GetPositionSize(), target.GetTextureSize(), 9);
		memcpy(result.m_Position, target.m_Position, target.GetPositionSize() * sizeof(float));
		memcpy(result.m_Texture, target.m_Texture, target.GetTextureSize() * sizeof(float));
		memcpy(result.m_Normal, target.m_Normal, 3 * sizeof(float));

		Vec3 N(result.m_Normal[0], result.m_Normal[1], result.m_Normal[2]);
		Vec3& t = tangents[vertex];
		Vec3& b = bitangents[vertex];

		Vec3 T = (t - N * N.dot(t)).normalized();
		float signB = (N.cross(T)).dot(b) < 0.f ? -1.f : 1.f;
		Vec3 B = (N.cross(T)).normalized() * signB;

		result.m_Normal[3] = T.x;
		result.m_Normal[4] = T.y;
		result.m_Normal[5] = T.z;

		result.m_Normal[6] = B.x;
		result.m_Normal[7] = B.y;
		result.m_Normal[8] = B.z;

		m_Points[vertex] = result;
	}

	delete[] tangents;

#else
	for (size_t face = 0; face < m_Indices.size(); face += 3)
	{
		for (int point = 0; point < 3; ++point)
		{
			Point target = m_Points[m_Indices[face + point]];

			Point next = m_Points[m_Indices[face + ((point + 1) % 3)]];
			Point previous = m_Points[m_Indices[face + ((point + 2) % 3)]];

			Point result(target.GetPositionSize(), target.GetTextureSize(), 9);
			memcpy(result.m_Position, target.m_Position, target.GetPositionSize() * sizeof(float));
			memcpy(result.m_Texture, target.m_Texture, target.GetTextureSize() * sizeof(float));
			memcpy(result.m_Normal, target.m_Normal, 3 * sizeof(float));

			{
				Vec3 v0(target.m_Position[0], target.m_Position[1], target.m_Position[2]);
				Vec3 v1(next.m_Position[0], next.m_Position[1], next.m_Position[2]);
				Vec3 v2(previous.m_Position[0], previous.m_Position[1], previous.m_Position[2]);

				Vec3 uv0(target.m_Texture[0], target.m_Texture[1], 0.f);
				Vec3 uv1(next.m_Texture[0], next.m_Texture[1], 0.f);
				Vec3 uv2(previous.m_Texture[0], previous.m_Texture[1], 0.f);

				Vec3 dPosition1 = v1 - v0;
				Vec3 dPosition2 = v2 - v0;
				Vec3 dUV1 = uv1 - uv0;
				Vec3 dUV2 = uv2 - uv0;

				float r = 1.0f / (dUV1.x * dUV2.y - dUV1.y * dUV2.x);

				Vec3 T = ((dPosition1 * dUV2.y - dPosition2 * dUV1.y) * r).normalized();
				Vec3 B = ((dPosition2 * dUV1.x - dPosition1 * dUV2.x) * r).normalized();


				Vec3 N(result.m_Normal[0], result.m_Normal[1], result.m_Normal[2]);
				T = (T - N * N.dot(T)).normalized();
				float signB = (N.cross(T)).dot(B) < 0.f ? -1.f : 1.f;
				B = (N.cross(T)).normalized() * signB;

				result.m_Normal[3] = T.x;
				result.m_Normal[4] = T.y;
				result.m_Normal[5] = T.z;

				result.m_Normal[6] = B.x;
				result.m_Normal[7] = B.y;
				result.m_Normal[8] = B.z;
			}

			m_Points[m_Indices[face + point]] = result;
		}
	}
#endif

	m_VertexSize = m_Points[0].m_Size;
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
	{
		int32_t posSize, texSize, normSize;
		posSize = point.GetPositionSize();
		texSize = point.GetTextureSize();
		normSize = point.GetNormalSize();

		_stream.write((char*)&posSize, sizeof(int32_t));
		_stream.write((char*)&texSize, sizeof(int32_t));
		_stream.write((char*)&normSize, sizeof(int32_t));

		_stream.write((char*)point.m_Position, point.m_Size * sizeof(float));
	}

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
		int32_t posSize = 0, normSize = 0, texSize = 0;
		_stream.read((char*)&posSize, sizeof(int32_t));
		_stream.read((char*)&texSize, sizeof(int32_t));
		_stream.read((char*)&normSize, sizeof(int32_t));

		Point candidate(posSize, texSize, normSize);
		_stream.read((char*)candidate.m_Position, candidate.m_Size * sizeof(float));

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

} // old
