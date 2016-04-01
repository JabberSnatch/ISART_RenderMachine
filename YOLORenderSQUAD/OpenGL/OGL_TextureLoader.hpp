#ifndef __OGL_TEXTURE_LOADER__
#define __OGL_TEXTURE_LOADER__

#include <unordered_map>
#include <string>

#include <glew\include\GL\glew.h>

using Texture = GLuint;

class OGL_TextureLoader
{
private:
	static OGL_TextureLoader* s_Instance;
public:
	static auto Get() -> OGL_TextureLoader*
	{
		if (s_Instance == nullptr)
			s_Instance = new OGL_TextureLoader();
		return s_Instance;
	}

	static auto Kill() -> void
	{
		if (s_Instance)
			delete s_Instance;
	}

public:
	OGL_TextureLoader() = default;
	OGL_TextureLoader(const OGL_TextureLoader&) = delete;
	OGL_TextureLoader(OGL_TextureLoader&&) = delete;
	~OGL_TextureLoader();

	auto	FreeOGLResources() -> void;
	auto	GetTexture(const std::string& _path, bool _forceAlpha = false) -> Texture;
	auto	GetCubeMap(const std::string* _path, bool _forceAlpha = false) -> Texture;

	auto	operator = (const OGL_TextureLoader&) -> OGL_TextureLoader& = delete;
	auto	operator = (OGL_TextureLoader&&) -> OGL_TextureLoader& = delete;

private:
	std::unordered_map<std::string, Texture> m_TextureMap;

};


#endif // __OGL_TEXTURE_LOADER__
