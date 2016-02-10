#include "D3D_Object.hpp"

#include <d3dcompiler.h>
#include <fstream>

auto
D3D_Object::Initialize(ID3D11Device* _device, ID3D11DeviceContext* _context) -> void
{
	m_Device = _device;
	m_Context = _context;
}


auto
D3D_Object::Shutdown() -> void
{

}


auto
D3D_Object::Render() -> void
{
	m_Context->UpdateSubresource(m_MatrixBuffer, 0, nullptr, &m_Matrices, sizeof(m_Matrices), 0);
	m_Context->VSSetConstantBuffers(0, 1, &m_MatrixBuffer);

	m_Context->IASetInputLayout(m_Layout);
	m_Context->VSSetShader(m_VertexShader, nullptr, 0);
	m_Context->PSSetShader(m_PixelShader, nullptr, 0);

	unsigned int stride = m_VerticesSize * sizeof(float);
	unsigned int offset = 0;
	m_Context->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	m_Context->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_Context->DrawIndexed(m_IndicesCount(), 0, 0);

	m_Context->VSSetShader(nullptr, nullptr, 0);
	m_Context->PSSetShader(nullptr, nullptr, 0);
}


auto
D3D_Object::AppendVertexAttribDesc(std::string _name, DXGI_FORMAT _format) -> void
{
	AttribDesc* vertexDescription = new AttribDesc[m_AttribCount + 1];
	
	if (m_VertexDescription)
	{
		//memcpy(vertexDescription, m_VertexDescription, m_AttribCount * sizeof(AttribDesc));
		for (int i = 0; i < m_AttribCount; ++i)
		{
			vertexDescription[i] = m_VertexDescription[i];
		}
		delete[] m_VertexDescription;
	}

	AttribDesc newVertexAttrib = { _name, _format };
	vertexDescription[m_AttribCount] = newVertexAttrib;

	m_VertexDescription = vertexDescription;
	m_AttribCount++;
}


auto
D3D_Object::LoadShaderAndCompile(std::string _path, ShaderType _type) -> HRESULT
{
	HRESULT result = S_OK;

	
	std::fstream sourceFile(_path, std::fstream::in);
	sourceFile.seekg(0, sourceFile.end);
	std::streamoff length = sourceFile.tellg();
	sourceFile.seekg(0, sourceFile.beg);

	char* buffer = new char[length];
	sourceFile.get(buffer, length, '\0');


	switch (_type)
	{
	case ShaderType::VertexShader:
		result = m_CompileVertexShader(buffer, "vs_5_0");
		break;
	case ShaderType::PixelShader:
		result = m_CompilePixelShader(buffer, "ps_5_0");
		break;
	}

	return result;
}


auto
D3D_Object::CreateVertexBuffer() -> void
{
	D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = m_VerticesCount * m_VerticesSize * sizeof(float);

	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = m_Data;

	m_Device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer);
}


auto
D3D_Object::CreateIndexBuffer() -> void
{
	D3D11_BUFFER_DESC indexBufferDesc = { 0 };
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = m_IndicesCount() * sizeof(unsigned long);

	D3D11_SUBRESOURCE_DATA indexData = { 0 };
	indexData.pSysMem = m_Indices;

	m_Device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer);
}


auto
D3D_Object::CreateMatrixBuffer() -> void
{
	D3D11_BUFFER_DESC matrixBufferDesc = { 0 };
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.CPUAccessFlags = 0;
	matrixBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		
	m_Device->CreateBuffer(&matrixBufferDesc, nullptr, &m_MatrixBuffer);
}


auto
D3D_Object::SetData(float* _data) -> void
{
	if (m_DataCount() != 0)
	{
		m_Data = new float[m_DataCount()];
		memcpy(m_Data, _data, m_DataCount() * sizeof(float));
	}
}


auto
D3D_Object::SetIndices(unsigned long* _indices) -> void
{
	if (m_IndicesCount() != 0)
	{
		m_Indices = new unsigned long[m_IndicesCount()];
		memcpy(m_Indices, _indices, m_IndicesCount() * sizeof(unsigned long));
	}
}


auto
D3D_Object::m_CompileVertexShader(const char* _contents, const char* _shaderTarget) -> HRESULT
{
	HRESULT hr = S_OK;
	ID3D10Blob* errorBlob = nullptr;
	ID3D10Blob* shaderBlob = nullptr;

	D3DCompile(_contents, strlen(_contents), nullptr, nullptr, nullptr
			   , "mainVS", "vs_5_0", 0, 0, &shaderBlob, &errorBlob);
	if (!shaderBlob)
	{
		OutputDebugString((LPCSTR)errorBlob->GetBufferPointer());
		hr = E_FAIL;
		errorBlob->Release();
	}
	m_Device->CreateVertexShader(shaderBlob->GetBufferPointer()
									, shaderBlob->GetBufferSize()
									, nullptr, &m_VertexShader);

	D3D11_INPUT_ELEMENT_DESC* layout = new D3D11_INPUT_ELEMENT_DESC[m_AttribCount];
	for (int i = 0; i < m_AttribCount; ++i)
	{
		D3D11_INPUT_ELEMENT_DESC element = { m_VertexDescription[i].m_Name.c_str(),
											0,
											m_VertexDescription[i].m_Format,
											0,
											(i == 0) ? 0 : D3D11_APPEND_ALIGNED_ELEMENT,
											D3D11_INPUT_PER_VERTEX_DATA,
											0 };
		layout[i] = element;
	}

	m_Device->CreateInputLayout(layout, 2, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &m_Layout);

	SAFE_RELEASE(shaderBlob);

	return hr;
}


auto
D3D_Object::m_CompilePixelShader(const char* _contents, const char* _shaderTarget) -> HRESULT
{
	HRESULT hr = S_OK;
	ID3D10Blob* errorBlob = nullptr;
	ID3D10Blob* shaderBlob = nullptr;

	D3DCompile(_contents, strlen(_contents), nullptr, nullptr, nullptr
			   , "mainPS", "ps_5_0", 0, 0, &shaderBlob, &errorBlob);
	
	if (!shaderBlob)
	{
		OutputDebugString((LPCSTR)errorBlob->GetBufferPointer());
		hr = E_FAIL;
		errorBlob->Release();
	}
	
	m_Device->CreatePixelShader(shaderBlob->GetBufferPointer()
								   , shaderBlob->GetBufferSize()
								   , nullptr, &m_PixelShader);

	SAFE_RELEASE(shaderBlob);

	return hr;
}
