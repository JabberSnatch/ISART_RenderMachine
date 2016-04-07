#ifndef __REF_MESH_DATA_HPP__
#define __REF_MESH_DATA_HPP__

#include <cstdint>
#include <vector>

#include "_Point.hpp"
#include "MaterialData.hpp"

// TODO: After refactor remember to remove REF_ from header guard
namespace refactor {


struct MeshData
{
	std::vector<Point>			m_Points;
	std::vector<uint32_t>		m_Indices;

	int32_t						m_VerticesCount = 0;
	int32_t						m_VertexSize = 0;
	int32_t						m_PolyCount = 0;
	std::vector<int32_t>		m_AttribSizes;

	MaterialData				m_Material;

	std::string					m_Folder;

	auto	ComputeNormalSpaces() -> void;

	auto	ExtractFolder(const std::string& _path) -> void;

	auto	Serialize(const std::string& _path) -> void;
	auto	Serialize(std::fstream& _stream) -> void;

	auto	Deserialize(const std::string& _path) -> void;
	auto	Deserialize(std::fstream& _stream) -> void;
};

struct MultiMeshData
{
	std::string					m_Name;
	std::vector<MeshData>		m_Meshes;

	auto	Serialize(const std::string& _path) -> void;
	auto	Deserialize(const std::string& _path) -> void;
};


} // refactor

#endif /*__REF_MESH_DATA_HPP__*/