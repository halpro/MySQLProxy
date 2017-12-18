#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>

#include "logger/DailyFileLogger.h"

DailyFileLogger::DailyFileLogger(const char* pstrFileName)
	: FileLogger(pstrFileName),	m_iLogDay(-1)
{
}

DailyFileLogger::DailyFileLogger(const char* pstrFileName, LogLevel logLevel)
	: FileLogger(pstrFileName,logLevel), m_iLogDay(-1)
{
}


DailyFileLogger::DailyFileLogger(const char* pstrFileName,const char* pstrLogLevel)
	: FileLogger(pstrFileName,pstrLogLevel), m_iLogDay(-1)
{
}

void DailyFileLogger::Open()
{
	string vstrFileName = m_vstrFileName;
	vstrFileName.replace(vstrFileName.find("$(DATE)"), 7, Logger::GetDateTime());
    m_vLogFile.open(vstrFileName.c_str(), ios::out | ios::app);
}

void DailyFileLogger::CheckFile()
{
	time_t      iTimeStamp = time(NULL);
	struct tm   vTime;
	::localtime_r(&iTimeStamp, &vTime);

	if( !m_vLogFile.is_open() || vTime.tm_mday!= m_iLogDay ) {
		Close();
		Open();
		m_iLogDay = vTime.tm_mday;
	}
}

