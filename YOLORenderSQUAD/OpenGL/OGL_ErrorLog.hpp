#ifndef __OGL_ERROR_LOG_HPP__
#define __OGL_ERROR_LOG_HPP__

#include <string>

#include "glew/include/GL/glew.h"

class OGL_ErrorLog
{
public:

	inline static bool			PrintGLError(const std::string& _message);
	inline static std::string	GLToString(GLenum _errorCode);


	OGL_ErrorLog() = delete;
	OGL_ErrorLog(const OGL_ErrorLog&) = delete;
	OGL_ErrorLog(OGL_ErrorLog&&) = delete;
	~OGL_ErrorLog() = delete;

	auto	operator = (const OGL_ErrorLog&) -> OGL_ErrorLog& = delete;
	auto	operator = (OGL_ErrorLog&&) -> OGL_ErrorLog& = delete;
};


#define OGL_ERROR_LOG(message) OGL_ErrorLog::PrintGLError(message)


#endif /*__OGL_ERROR_LOG_HPP__*/