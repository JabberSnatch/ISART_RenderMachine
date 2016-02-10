#ifndef __D3D_OBJECT_HPP__
#define __D3D_OBJECT_HPP__

#include <d3d11.h>
#include <string>

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = nullptr; } }

class D3D_Object
{
	struct AttribDesc;
	struct MatrixBuffer;

public:
	struct MatrixBuffer
	{
		float m_WorldMatrix[16];
		float m_ViewMatrix[16];
		float m_ProjectionMatrix[16];
	};
	enum ShaderType;

	D3D_Object() = default;
	D3D_Object(const D3D_Object&) = delete;
	D3D_Object(D3D_Object&&) = delete;
	~D3D_Object() = default;

	auto	Initialize(ID3D11Device* _device, ID3D11DeviceContext* _context) -> void;
	auto	Shutdown() -> void;
	auto	Render() -> void;

	auto	AppendVertexAttribDesc(std::string _name, DXGI_FORMAT _format) -> void;
	auto	LoadShaderAndCompile(std::string _path, ShaderType _type) -> HRESULT;

	auto	CreateVertexBuffer() -> void;
	auto	CreateIndexBuffer() -> void;
	auto	CreateMatrixBuffer() -> void;

	auto	SetData(float* _data) -> void;
	auto	SetIndices(unsigned long* _indices) -> void;

	auto	SetPolyCount(int _value) -> void { m_PolyCount = _value; }
	auto	SetVerticesCount(int _value) -> void { m_VerticesCount = _value; }
	auto	SetVerticesSize(int _value) -> void { m_VerticesSize = _value; }

	auto	SetMatrixBuffer(MatrixBuffer _value) -> void { m_Matrices = _value; }

	auto	operator = (const D3D_Object&) -> D3D_Object& = delete;
	auto	operator = (D3D_Object&&) -> D3D_Object& = delete;

private:
	auto	m_CompileVertexShader(const char* _contents, const char* _shaderTarget) -> HRESULT;
	auto	m_CompilePixelShader(const char* _contents, const char* _shaderTarget) -> HRESULT;

	auto	m_IndicesCount() -> int { return m_PolyCount * 3; }
	auto	m_DataCount() -> int { return m_VerticesCount * m_VerticesSize; }

	float*					m_Data = nullptr;
	unsigned long*			m_Indices = nullptr;

	int						m_PolyCount = 0;
	int						m_VerticesCount = 0;
	int						m_VerticesSize = 0;

	int						m_AttribCount = 0;
	AttribDesc*				m_VertexDescription = nullptr;

	ID3D11Device*			m_Device = nullptr;
	ID3D11DeviceContext*	m_Context = nullptr;

	ID3D11Buffer*			m_VertexBuffer = nullptr;
	ID3D11Buffer*			m_IndexBuffer = nullptr;
	ID3D11Buffer*			m_MatrixBuffer = nullptr;
	ID3D11InputLayout*		m_Layout = nullptr;

	ID3D11VertexShader*		m_VertexShader = nullptr;
	ID3D11PixelShader*		m_PixelShader = nullptr;

	MatrixBuffer			m_Matrices;
	
	struct AttribDesc
	{
		std::string		m_Name;
		DXGI_FORMAT		m_Format;
	};

public:
	enum ShaderType
	{
		VertexShader,
		PixelShader
	};
};


#endif /*__D3D_OBJECT_HPP__*/
