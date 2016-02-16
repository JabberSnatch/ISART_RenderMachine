#ifndef __OBJ_PARSER_HPP__
#define __OBJ_PARSER_HPP__

#include <regex>
#include <vector>
#include <string>

#include <boost/tokenizer.hpp>

using bSeparator = boost::char_separator<char>;
using bTokenizer = boost::tokenizer<bSeparator>;
using bIterator = boost::token_iterator_generator<bSeparator>::type;


#include "MtlParser.hpp"

#define UNDEFINED_VALUE (int)0x0fffffff

struct OBJ
{
	std::string				m_Name;
	
	int						m_PositionSize = UNDEFINED_VALUE;
	int						m_NormalSize = UNDEFINED_VALUE;
	int						m_TexCoordSize = UNDEFINED_VALUE;

	std::vector<float>		m_Positions;
	std::vector<float>		m_Normals;
	std::vector<float>		m_TexCoords;
	std::vector<int>		m_Indices;
};

template <int A, int B, int C>
union PointT
{
	struct
	{
		float m_Position[A];
		float m_TexCoords[B];
		float m_Normal[C];
	} E;
	float m_Data[A + B + C];
};

struct Point
{
	Point(int _pos, int _tex, int _norm)
		:m_Size(_pos + _tex + _norm)
	{
		m_Position = new float[m_Size];
		m_Texture = m_Position + _pos;
		m_Normal = m_Texture + _tex;
	}
	Point(const Point& other)
	{
		m_Size = other.m_Size;
		m_Position = new float[m_Size];
		m_Texture = m_Position + (other.m_Texture - other.m_Position);
		m_Normal = m_Texture + (other.m_Normal - other.m_Texture);
		memcpy(m_Position, other.m_Position, m_Size * sizeof(float));
	}
	~Point()
	{
		delete[] m_Position;
	}

	float*	m_Position;
	float*	m_Texture;
	float*	m_Normal;
	int		m_Size;

	auto	GetPositionSize() const -> int { return m_Texture - m_Position; }
	auto	GetTextureSize() const -> int { return m_Normal - m_Texture; }
	auto	GetNormalSize() const -> int { return m_Size - (m_Normal - m_Position); }

	auto	operator == (const Point& other) const -> bool
	{
		return (!memcmp(this->m_Position, other.m_Position, m_Size * sizeof(float)));
	}
};

struct MeshData
{
	std::vector<Point>			m_Points;
	std::vector<unsigned int>	m_Indices;

	int							m_VerticesCount = 0;
	int							m_VertexSize = 0;
	int							m_PolyCount = 0;
	std::vector<int>			m_AttribSizes;

	MaterialData				m_Material;

	std::string					m_Folder;

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


class ObjParser
{
public:
	ObjParser() = default;
	~ObjParser() = default;

	auto	ParseFile(std::string const& _path) -> void;
	auto	GenerateMeshData() -> MultiMeshData&;

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