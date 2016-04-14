#include "OGL_ErrorLog.hpp"

#include "Logger.hpp"


bool			
OGL_ErrorLog::PrintGLError(const std::string& _message)
{
	GLenum error = glGetError();
	bool success = error == GL_NO_ERROR;
	if (!success)
		LOG_ERROR(_message + " : " + GLToString(error));
	
	return success;
}


std::string	
OGL_ErrorLog::GLToString(GLenum _errorCode)
{
	switch (_errorCode)
	{
	case (GL_INVALID_ENUM) :
		return "INVALID ENUM";
	case (GL_INVALID_VALUE) :
		return "INVALID VALUE";
	case (GL_INVALID_OPERATION) :
		return "INVALID OPERATION";
	case (GL_INVALID_FRAMEBUFFER_OPERATION) :
		return "INVALID FRAMEBUFFER OPERATION";
	case (GL_OUT_OF_MEMORY) :
		return "OUT OF MEMORY";
	case (GL_STACK_UNDERFLOW) :
		return "STACK OVERFLOW";
	default :
		return std::to_string(_errorCode);
	}
}

