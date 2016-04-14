#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <string>


class Logger final
{
public:
	enum LogLevel
	{
		LOG_NONE = 0,
		LOG_CRITICAL,
		LOG_ERROR,
		LOG_WARNING,
		LOG_DEBUG,
		LOG_INFO,
		LOG_VERBOSE,
		LOG_LEVEL_COUNT
	};


	static void			Log(LogLevel _level, const std::string& _message);
	static std::string	LogLevelToString(LogLevel _level);
	
	static LogLevel	CurrentLevel;


	Logger() = delete;
	Logger(const Logger&) = delete;
	Logger(Logger&&) = delete;
	~Logger() = delete;

	auto	operator = (const Logger&) -> Logger& = delete;
	auto	operator = (Logger&&) -> Logger& = delete;
};


#ifdef _DEBUG
#define LOG_CRITICAL(message)	Logger::Log(Logger::LOG_CRITICAL, message)
#define LOG_ERROR(message)		Logger::Log(Logger::LOG_ERROR, message)
#define LOG_WARNING(message)	Logger::Log(Logger::LOG_WARNING, message)
#define LOG_DEBUG(message)		Logger::Log(Logger::LOG_DEBUG, message)
#define LOG_INFO(message)		Logger::Log(Logger::LOG_INFO, message)
#define LOG_VERBOSE(message)	Logger::Log(Logger::LOG_VERBOSE, message)
#else
#define LOG_CRITICAL(message)
#define LOG_ERROR(message)
#define LOG_WARNING(message)
#define LOG_DEBUG(message)
#define LOG_INFO(message)	
#define LOG_VERBOSE(message)
#endif


#endif /*__LOGGER_HPP__*/