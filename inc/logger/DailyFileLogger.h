#ifndef _DAILY_FILE_LOGGER_H
#define _DAILY_FILE_LOGGER_H

#include "logger/FileLogger.h"

class DailyFileLogger : public FileLogger
{
public:
	DailyFileLogger(const char* pstrFileName);
	DailyFileLogger(const char* pstrFileName,LogLevel logLevel);
	DailyFileLogger(const char* pstrFileName,const char* pstrLogLevel);

protected:
	int m_iLogDay;

	void Open();
	virtual void CheckFile();
};

#endif // _DAILY_FILE_LOGGER_H
