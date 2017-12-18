#ifndef _CONSOLE_LOGGER_H
#define _CONSOLE_LOGGER_H

#include "logger/Logger.h"

class ConsoleLogger : public Logger
{
public:
	ConsoleLogger() : Logger()
	{
	}

	ConsoleLogger(LogLevel logLevel) : Logger()
	{
		SetLogLevel(logLevel);
	}

protected:
	void WriteLog(LogLevel aiLogLevel, const char* apstrFormat, va_list& vArgs);
};

#endif
