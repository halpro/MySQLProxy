#ifndef _LOGGER_FACTORY_H
#define _LOGGER_FACTORY_H

#include <string>
#include <mutex>
#include "logger/Logger.h"

using namespace std;

class LoggerFactory
{
public:
	static void Init(const string& mainLogFileName, const string& errorLogFileName, const string& logLevel);
	static Logger* GetNullLogger();
	static Logger* GetMainLogger();
	static Logger* GetErrorLogger();

private:
	LoggerFactory() {};

	static mutex  m_vLock;
	static string m_vstrMainLogFileName;
	static string m_vstrErrorLogFileName;

	static Logger* m_pNullLogger;
	static Logger* m_pMainLogger;
	static Logger::LogLevel m_MainLoggerLogLevel;
	static Logger* m_pErrorLogger;
	static Logger::LogLevel m_ErrorLoggerLogLevel;
};
#endif
