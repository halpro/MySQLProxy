#ifndef _FILE_LOGGER_H
#define _FILE_LOGGER_H

#include <fstream>
using namespace std;

#include "logger/Logger.h"

class FileLogger : public Logger
{
public:
	FileLogger(const char* pstrFileName);
	FileLogger(const char* pstrFileName,LogLevel logLevel);
	FileLogger(const char* pstrFileName,const char* pstrLogLevel);
	~FileLogger();
	void Close();
protected:
	string m_vstrFileName;
        ofstream  m_vLogFile;

	virtual void Open();
	virtual void CheckFile();
	virtual void WriteLog(LogLevel aiLogLevel, const char* apstrFormat, va_list& vArgs);
};

#endif // _FILE_LOGGER_H
