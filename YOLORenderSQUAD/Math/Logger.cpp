#include "Logger.hpp"

#include <iostream>


void		
Logger::Log(LogLevel _level, const std::string& _message)
{
	if (_level < CurrentLevel)
		std::cout << "[" << LogLevelToString(_level) << "] " << _message << std::endl;
}


std::string	
Logger::LogLevelToString(LogLevel _level)
{
	switch (_level)
	{
	case LOG_CRITICAL:
		return "CRITICAL";
	case LOG_ERROR:
		return "ERROR";
	case LOG_WARNING:
		return "WARNING";
	case LOG_DEBUG:
		return "DEBUG";
	case LOG_INFO:
		return "INFO";
	case LOG_VERBOSE:
		return "VERBOSE";
	default:
		return "";
	}
}


Logger::LogLevel Logger::CurrentLevel = LOG_DEBUG;
