#include <stdio.h>
#include <stdarg.h>
#include <fstream>
#include "logger/FileLogger.h"

using namespace std;

FileLogger::FileLogger(const char* pstrFileName)
	: m_vstrFileName(pstrFileName)
{
}

FileLogger::FileLogger(const char* pstrFileName, LogLevel logLevel)
	: m_vstrFileName(pstrFileName)
{
	SetLogLevel(logLevel);
}


FileLogger::FileLogger(const char* pstrFileName,const char* pstrLogLevel)
	: m_vstrFileName(pstrFileName)
{
	SetLogLevel( Char2LogLevel(pstrLogLevel[0]) );
}

FileLogger::~FileLogger()
{
	Close();
}

void FileLogger::Open()
{
    m_vLogFile.open(m_vstrFileName.c_str(), ios::out | ios::app);
}

void FileLogger::Close()
{
	if(m_vLogFile.is_open())
		m_vLogFile.close();
}

void FileLogger::CheckFile()
{
	if( !m_vLogFile.is_open() ) {
		Open();
	}
}

void FileLogger::WriteLog(LogLevel aiLogLevel, const char* apstrFormat, va_list& vArgs)
{
	if(m_iLogLevel > aiLogLevel) return;

	std::lock_guard<mutex> lock(m_vLock);

	try
	{
		CheckFile();
		string logString = Logger::FormatString(aiLogLevel,apstrFormat,vArgs);
		m_vLogFile << logString.c_str() << endl;

		if(aiLogLevel == TraceLevel)
			m_vLogFile.flush();
	}
	catch(...)
	{
	}
}

