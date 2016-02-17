#include "OGL_TextureLoader.hpp"

#include <fstream>
#include <stb-master\stb_image.h>

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
