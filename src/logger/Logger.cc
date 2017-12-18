#include <stdio.h>
#include <stdarg.h>
#include <strings.h>
#include <sys/time.h>
#include "util/ThreadHelper.h"
#include <sstream>
#include "logger/Logger.h"

#define MESSAGE_BUFFER_SIZE       4096
#define TID_STRING_SIZE           5
#define MILLISEC_STRING_SIZE      5
#define TIME_STRING_SIZE          25
#define TIME_STRING_FORMAT        "%Y-%m-%d %H:%M:%S"
#define TIME_STRING_FORMAT_FILE   "%Y-%m-%d"

using namespace std;

char Logger::LogLevel2Char(Logger::LogLevel logLevel) {
    static char log_level_chars[] = { 'T', 'D', 'I', 'E', 'O' };
    return  (logLevel >= TraceLevel && logLevel<=OffLevel) ? log_level_chars[logLevel] : 'X';
};

Logger::LogLevel Logger::Char2LogLevel(char charLogLevel) {
    LogLevel level = TraceLevel; // by default
    switch(charLogLevel) {
    case 'D':
        level = DebugLevel;
        break;
    case 'I':
        level = InfoLevel;
        break;
    case 'E':
        level = ErrorLevel;
        break;
    }

    return level;
}

string Logger::GetDateTime() {
    time_t      iTimeStamp = time(NULL);
    struct tm   vTime;
    char        pstrTime[TIME_STRING_SIZE];

    localtime_r(&iTimeStamp, &vTime);
    bzero(pstrTime, TIME_STRING_SIZE);
    strftime(pstrTime, TIME_STRING_SIZE, TIME_STRING_FORMAT_FILE, &vTime);

    return pstrTime;
}

string Logger::GetDateTime(bool showMilliseconds) {
    char pstrTime[TIME_STRING_SIZE];

    struct timeval tv;
    ::gettimeofday(&tv, NULL);
    struct tm* ptm = ::localtime(&tv.tv_sec);
    ::strftime(pstrTime, TIME_STRING_SIZE, TIME_STRING_FORMAT, ptm);

    if(showMilliseconds) {
        long milliseconds = tv.tv_usec/1000;
        char pstrMillisec[MILLISEC_STRING_SIZE];
        snprintf(pstrMillisec,MILLISEC_STRING_SIZE,".%03ld",milliseconds);
        strncat(pstrTime,pstrMillisec,TIME_STRING_SIZE);
    }
    return pstrTime;
}

string Logger::GetThreadId() {
    std::ostringstream os;
    os << ThreadHelper::GetThreadId();
    return os.str();
}

string Logger::FormatString(LogLevel logLevel, const char* apstrFormat, va_list& vArgs) {
    ostringstream oss;

    try {
        char *pstrText = new char[MESSAGE_BUFFER_SIZE];
        if(pstrText != NULL) {
            bzero(pstrText, MESSAGE_BUFFER_SIZE);

            ::vsnprintf(pstrText, MESSAGE_BUFFER_SIZE, apstrFormat, vArgs);

            oss << "[" << GetDateTime(true) << "]";
            oss << "[T:" << GetThreadId() << "]";
            oss << "[" << LogLevel2Char(logLevel) <<"] ";
            oss << pstrText;

            delete []pstrText;
        }
    } catch(...) {
    }

    return oss.str();
}

void Logger::WriteLog(LogLevel aiLogLevel, const char* apstrText, ...) {
    va_list vArgs;
    va_start(vArgs, apstrText);
    WriteLog(aiLogLevel, apstrText, vArgs);
    va_end(vArgs);
}

void Logger::Error(const char* apstrText, ...) {
    va_list vArgs;
    va_start(vArgs, apstrText);
    WriteLog(ErrorLevel,apstrText, vArgs);
    va_end(vArgs);
}

void Logger::Info(const char* apstrText, ...) {
    va_list vArgs;
    va_start(vArgs, apstrText);
    WriteLog(InfoLevel,apstrText, vArgs);
    va_end(vArgs);
}

void Logger::Debug(const char* apstrText, ...) {
    va_list vArgs;
    va_start(vArgs, apstrText);
    WriteLog(DebugLevel,apstrText, vArgs);
    va_end(vArgs);
}

void Logger::Trace(const char* apstrText, ...) {
    va_list vArgs;
    va_start(vArgs, apstrText);
    WriteLog(TraceLevel,apstrText, vArgs);
    va_end(vArgs);
}
