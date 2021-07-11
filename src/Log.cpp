#include "Log.h"

#include <ctime>
#include <sstream>

Log *logger = nullptr;

Log::Log()
{
    outf.open("LatestLog.txt", std::ios::trunc);
}

Log::~Log()
{
    if (outf.is_open())
        outf.close();
}

void Log::Send(const std::string &name, const std::string &msg)
{
    time_t now = time(nullptr);
    tm localTime{};
#ifdef __linux__
    localtime_r(&now, &localTime);
#elif _WIN32
    localtime_s(&localTime, &now);
#endif
    std::stringstream ss;
    ss << "[" << name << "]" << "[" << localTime.tm_hour << ":" << localTime.tm_min << ":" << localTime.tm_sec << "]: " << msg << std::endl;

    Write(ss.str());
}

void Log::Write(const std::string &msg)
{
    lock.lock();
    outf << msg;
    lock.unlock();
}
