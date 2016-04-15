#ifndef __OGL_ERROR_LOG_HPP__
#define __OGL_ERROR_LOG_HPP__

#include <string>

#include "glew/include/GL/glew.h"

class OGL_ErrorLog
{
public:

	static bool			PrintGLError(const std::string& _message);
	static int			ClearErrorStack();
	static std::string	GLToString(GLenum _errorCode);


	OGL_ErrorLog() = delete;
	OGL_ErrorLog(const OGL_ErrorLog&) = delete;
	OGL_ErrorLog(OGL_ErrorLog&&) = delete;
	~OGL_ErrorLog() = delete;

	auto	operator = (const OGL_ErrorLog&) -> OGL_ErrorLog& = delete;
	auto	operator = (OGL_ErrorLog&&) -> OGL_ErrorLog& = delete;
};


#ifdef _DEBUG
#define OGL_ERROR_LOG(message) OGL_ErrorLog::PrintGLError(message)
#else
#define OGL_ERROR_LOG(message) (glGetError() == GL_NO_ERROR)
#endif


#endif /*__OGL_ERROR_LOG_HPP__*/