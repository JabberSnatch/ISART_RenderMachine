#ifndef __MTL_PARSER_HPP__
#define __MTL_PARSER_HPP__

#include <boost/tokenizer.hpp>

using bSeparator = boost::char_separator<char>;
using bTokenizer = boost::tokenizer<bSeparator>;
using bIterator = boost::token_iterator_generator<bSeparator>::type;


#include <string>
#include <unordered_map>

#include "MaterialData.hpp"


class MtlParser
{
public:
	MtlParser() = default;
	MtlParser(const MtlParser&) = delete;
	MtlParser(MtlParser&&) = delete;
	~MtlParser() = default;

	auto	ParseFile(const std::string& _path) -> void;
	auto	Get(const std::string& _material, MaterialData& o_Material) -> bool;

	auto	operator = (const MtlParser&) -> MtlParser& = delete;
	auto	operator = (MtlParser&&) -> MtlParser& = delete;

private:
	auto	m_NewMaterial(bIterator _ite) -> bool;
	auto	m_ParseColor(float* _colorArray, bIterator _ite) -> bool;
	auto	m_ParseFloat(float* _floatField, bIterator _ite) -> bool;

	MaterialData*									m_CurrentMaterial;
	std::unordered_map<std::string, MaterialData>	m_Materials;

};


#endif /*__MTL_PARSER_HPP__*/