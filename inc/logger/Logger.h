#ifndef _LOGGER_H
#define _LOGGER_H

#include <string>
#include <string.h>
#include <mutex>

using std::string;

class Logger {
  public:

    enum LogLevel {
        TraceLevel = 0,
        DebugLevel = 1,
        InfoLevel  = 2,
        ErrorLevel = 3,
        OffLevel   = 4
    };

    Logger() : m_iLogLevel(TraceLevel)  {};

    virtual ~Logger() {};

    void WriteLog(LogLevel aiLogLevel, const char* apstrText, ...);
    void Error(const char* apstrText, ...);
    void Info(const char* apstrText, ...);
    void Debug(const char* apstrText, ...);
    void Trace(const char* apstrText, ...);

    LogLevel SetLogLevel(LogLevel aiLogLevel) {
        LogLevel cur = m_iLogLevel;
        m_iLogLevel = aiLogLevel;
        return cur;
    };
    LogLevel GetLogLevel() const {
        return m_iLogLevel;
    }

    bool IsTraceLevel() {
        return m_iLogLevel==TraceLevel;
    };
    bool IsDebugLevel() {
        return m_iLogLevel<=DebugLevel;
    };

    static char LogLevel2Char(LogLevel aiLogLevel);
    static LogLevel Char2LogLevel(char charLogLevel);

    static string FormatString(LogLevel logLevel, const char* apstrFormat, va_list& vArgs);

    static string GetDateTime();
    static string GetDateTime(bool showMilliseconds);
    static string GetThreadId();

  protected:
    LogLevel m_iLogLevel;
    std::mutex m_vLock;

    virtual void WriteLog(LogLevel aiLogLevel, const char* apstrFormat, va_list& vArgs) = 0;
};

#endif
