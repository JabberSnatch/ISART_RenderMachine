#include "OGL_TextureLoader.hpp"

#include <fstream>

#include <stb-master\stb_image.h>

#include "ISkybox.hpp"

OGL_TextureLoader* OGL_TextureLoader::s_Instance = nullptr;


OGL_TextureLoader::~OGL_TextureLoader()
{
	FreeOGLResources();
}


auto 
OGL_TextureLoader::FreeOGLResources() -> void
{
	for (auto& pair : m_TextureMap)
		glDeleteTextures(1, &pair.second);

	m_TextureMap.clear();
}

auto 
OGL_TextureLoader::GetTexture(const std::string & _path, bool _forceAlpha) -> Texture
{
	if (!std::fstream(_path).good()) return 0;

	if (m_TextureMap.find(_path) == m_TextureMap.end())
	{
		m_TextureMap.emplace(_path, 0);
		int w, h, d;
		auto* data = stbi_load(_path.c_str(), &w, &h, &d, _forceAlpha ? STBI_rgb_alpha : STBI_rgb);
	
		if (data)
		{
			glGenTextures(1, &m_TextureMap[_path]);
			glBindTexture(GL_TEXTURE_2D, m_TextureMap[_path]);
			glTexImage2D(GL_TEXTURE_2D, 0, _forceAlpha ? GL_SRGB_ALPHA : GL_SRGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);

			stbi_image_free(data);
		}
	}

	return m_TextureMap[_path];
}


auto
OGL_TextureLoader::GetCubeMap(const std::string* _path, bool _forceAlpha) -> Texture
{
	m_TextureMap.emplace(*_path, 0);
	glGenTextures(1, &m_TextureMap[*_path]);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureMap[*_path]);

	for (int i = 0; i < ISkybox::E_FACE::COUNT; ++i)
	{
		int w, h, d;
		auto* data = stbi_load(_path[i].c_str(), &w, &h, &d, _forceAlpha ? STBI_rgb_alpha : STBI_rgb);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, _forceAlpha ? GL_SRGB_ALPHA : GL_SRGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return m_TextureMap[*_path];
}
