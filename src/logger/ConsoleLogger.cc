#include <stdio.h>
#include <stdarg.h>

#include <iostream>

#include "logger/ConsoleLogger.h"

using namespace std;

void ConsoleLogger::WriteLog(LogLevel aiLogLevel, const char* apstrFormat, va_list& vArgs)
{
	if(m_iLogLevel > aiLogLevel) return;

	m_vLock.lock();

	try
	{
		string logString = Logger::FormatString(aiLogLevel,apstrFormat,vArgs);
		std::cout << logString.c_str() << endl;
	}
	catch(...)
	{
	}

	m_vLock.unlock();
}

