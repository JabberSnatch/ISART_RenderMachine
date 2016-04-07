#ifndef __REF_OBJ_PARSER_HPP__
#define __REF_OBJ_PARSER_HPP__

#include <cstdint>
#include <vector>

#include "BoostTokHelper.hpp"

#include "_MeshData.hpp"
#include "MtlParser.hpp"

#define UNDEFINED_VALUE INT32_MAX


// TODO: After refactor remember to remove REF_ from header guard
namespace refactor {


struct OBJ
{
	// NOTE: Maybe we don't always know beforehand the size of each vertex component.
	int32_t		_PositionSize = 3;
	int32_t		_NormalSize = 3;
	int32_t		_TexCoordSize = 2;

	std::vector<float>		_Positions;
	std::vector<float>		_Normals;
	std::vector<float>		_TexCoords;
	std::vector<int32_t>	_Indices;

	void	Clear()
	{
		_PositionSize = 3;
		_NormalSize = 3;
		_TexCoordSize = 2;
		_Positions.clear();
		_Normals.clear();
		_TexCoords.clear();
		_Indices.clear();
	}
};


class ObjParser
{
public:
	ObjParser() = default;
	~ObjParser() = default;

	void			ParseFile(std::string const& _path);
	MultiMeshData&	GenerateMeshData(bool _computeNormalSpaces = false);

	static std::string	ExtractFolderFromPath(std::string const& _path);

private:
	void	NewMeshGroup();
	void	FinishCurrentGroup(std::string const& _path);
	void	ComputeFaceIndices(MeshData* _mesh);

	int32_t	ComputeArrayOffset(int32_t _readIndex, int32_t _componentSize, int32_t _componentCount);

	bool	GetFace(bIterator _ite);
	bool	GetFloatGroup(bIterator _ite, int& _componentSize, std::vector<float>& _componentArray);


	OBJ				m_OBJStorage;
	MeshData*		m_CurrentMeshData;
	MultiMeshData	m_WorkingData;

	MtlParser		m_MtlParser;
};


} 

#endif /*__REF_OBJ_PARSER_HPP__*/