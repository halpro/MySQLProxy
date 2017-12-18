#include <sys/time.h>

#include "logger/LoggerFactory.h"
#include "logger/NullLogger.h"
#include "logger/ConsoleLogger.h"
#include "logger/FileLogger.h"
#include "logger/DailyFileLogger.h"

std::mutex LoggerFactory::m_vLock;
string LoggerFactory::m_vstrMainLogFileName;
Logger::LogLevel LoggerFactory::m_MainLoggerLogLevel = Logger::TraceLevel;
string LoggerFactory::m_vstrErrorLogFileName;
Logger::LogLevel LoggerFactory::m_ErrorLoggerLogLevel = Logger::TraceLevel;

Logger* LoggerFactory::m_pNullLogger = NULL;
Logger* LoggerFactory::m_pMainLogger = NULL;
Logger* LoggerFactory::m_pErrorLogger = NULL;

void LoggerFactory::Init(const string& mainLogFileName, const string& errorLogFileName, const string& logLevel)
{
	m_MainLoggerLogLevel = Logger::Char2LogLevel( logLevel.at(0) );
	m_ErrorLoggerLogLevel = m_MainLoggerLogLevel;

	m_vstrMainLogFileName = mainLogFileName;
	m_vstrErrorLogFileName = errorLogFileName;

}

Logger* LoggerFactory::GetNullLogger()
{
    std::lock_guard<mutex> lock(m_vLock);
    try
    {
        if(NULL == m_pNullLogger)
            m_pNullLogger = new NullLogger();
    }
    catch(...)
    {
    }

	return m_pNullLogger;
}

Logger* LoggerFactory::GetMainLogger()
{
	std::lock_guard<mutex> lock(m_vLock);

	try
	{
		if(NULL == m_pMainLogger)
			m_pMainLogger = new DailyFileLogger( m_vstrMainLogFileName.c_str(), m_MainLoggerLogLevel );
	}
	catch(...)
	{
	}

	return m_pMainLogger;
}


Logger* LoggerFactory::GetErrorLogger()
{
	std::lock_guard<mutex> lock(m_vLock);

	try
	{
		if(NULL == m_pErrorLogger)
			m_pErrorLogger = new DailyFileLogger( m_vstrErrorLogFileName.c_str(), m_ErrorLoggerLogLevel );
	}
	catch(...)
	{
	}

	return m_pErrorLogger;
}

