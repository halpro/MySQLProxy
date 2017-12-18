#define BOOST_SPIRIT_THREADSAFE

#include "util/ThreadHelper.h"
#include <sstream>
#include <thread>
unsigned long ThreadHelper::GetThreadId() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    uint64_t id = std::stoull(ss.str());
    return id;
}

