#ifndef __OBJ_PARSER_HPP__
#define __OBJ_PARSER_HPP__

#include <regex>
#include <vector>
#include <string>
#include <cstdint>

#include "BoostTokHelper.hpp"

#include "MtlParser.hpp"
#include "MeshData.hpp"

#define UNDEFINED_VALUE (int)0x0fffffff

struct OBJ
{
	std::string				m_Name;
	
	int32_t					m_PositionSize = 3;
	int32_t					m_NormalSize = 3;
	int32_t					m_TexCoordSize = 2;

	std::vector<float>		m_Positions;
	std::vector<float>		m_Normals;
	std::vector<float>		m_TexCoords;
	std::vector<int>		m_Indices;

	auto	Clear() -> void
	{
		m_Name = "";
		m_PositionSize = 3;
		m_NormalSize = 3;
		m_TexCoordSize = 2;
		m_Positions.clear();
		m_Normals.clear();
		m_TexCoords.clear();
		m_Indices.clear();
	}
};


class ObjParser
{
public:
	ObjParser() = default;
	~ObjParser() = default;

	auto	ParseFile(std::string const& _path) -> void;
	auto	GenerateMeshData(bool _computeNormalSpaces = false) -> MultiMeshData&;

	static auto		ExtractFolderFromPath(std::string const& _path) -> std::string;

private:
	auto	m_NewMeshGroup() -> void;
	auto	m_FinishCurrentGroup(const std::string& _path = "") -> void;
	auto	m_SetPointData(float* _pointArray, int _index, int _componentSize, std::vector<float>& _componentArray) -> void;
	auto	m_ComputeFaceIndices(OBJ& _currentOBJ, MeshData& _mesh) -> void;

	auto	m_ParseName(OBJ& _currentOBJ, bIterator _ite) -> bool;
	auto	m_ParseFace(OBJ& _currentOBJ, bIterator _ite) -> bool;
	auto	m_ParseFloatGroup(OBJ& _currentOBJ, int& _componentSize, std::vector<float>& _componentArray, bIterator _ite) -> bool;
	
	OBJ					m_CurrentOBJ;

	MeshData*			m_CurrentMeshData;
	MultiMeshData		m_WorkingData;

	MtlParser			m_MtlParser;

	auto	m_Regex_ParseName(std::smatch const& _match) -> bool;
	auto	m_Regex_ParseFace(std::smatch const& _match) -> bool;
	auto	m_Regex_ParseVertex(std::string const& _line) -> bool;
	auto	m_Regex_ParseTexture(std::string const& _line) -> bool;
	auto	m_Regex_ParseNormal(std::string const& _line) -> bool;

	static const std::regex Object;
	static const std::regex Face;
	static const std::regex VertexLine;
	static const std::regex TextureLine;
	static const std::regex NormalLine;
	static const std::regex FloatComponent;
};


#endif /*__OBJ_PARSER_HPP__*/