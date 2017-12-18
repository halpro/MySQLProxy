#ifndef _NULL_LOGGER_H
#define _NULL_LOGGER_H

#include "Logger.h"

class NullLogger : public Logger {
  public:
    NullLogger() : Logger() {
    }

  protected:
    virtual void WriteLog(LogLevel aiLogLevel, const char* apstrFormat, va_list& vArgs) {};
};

#endif
