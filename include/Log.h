#ifndef LOG_H
#define LOG_H

#include <mutex>
#include <string>
#include <fstream>

class Log {
public:
    enum class Severity : int {
        Debug = 0,
        Info,
        Warning,
        Error,
        None,
        Default = Debug
    };

private:
    Severity severity = Severity::Default;
    std::ofstream logFile;
    std::mutex lock;

    void Send(const std::string &name, const std::string &msg);

protected:
    virtual void Write(const std::string &msg);

public:
    Log();
    virtual ~Log();

    void SetSeverity(Severity sev)
    {
        severity = sev;
    }

    void Error(const std::string &msg)
    {
        if (severity <= Severity::Error)
            Send("Error", msg);
    }

    template<typename... Args>
    void Error(const std::string &fmt, Args... args)
    {
        if (severity <= Severity::Error)
        {
            if (int size = snprintf(nullptr, 0, fmt.c_str(), args...); size > 0)
            {
                ++size;
                auto *buf = new char[size];
                snprintf(buf, size, fmt.c_str(), args...);
                std::string msgFull = std::string(buf, buf + size - 1);
                delete[] buf;

                Send("Error", msgFull);
            }
        }
    }

    void Warning(const std::string &msg)
    {
        if (severity <= Severity::Warning)
            Send("Warning", msg);
    }

    template<typename... Args>
    void Warning(const std::string &fmt, Args... args)
    {
        if (severity <= Severity::Warning)
        {
            if (int size = snprintf(nullptr, 0, fmt.c_str(), args...); size > 0)
            {
                ++size;
                auto *buf = new char[size];
                snprintf(buf, size, fmt.c_str(), args...);
                std::string msgFull = std::string(buf, buf + size - 1);
                delete[] buf;

                Send("Warning", msgFull);
            }
        }
    }

    void Debug(const std::string &msg)
    {
        if (severity <= Severity::Debug)
            Send("Debug", msg);
    }

    template<typename... Args>
    void Debug(const std::string &fmt, Args... args)
    {
        if (severity <= Severity::Debug)
        {
            if (int size = snprintf(nullptr, 0, fmt.c_str(), args...); size > 0)
            {
                ++size;
                auto *buf = new char[size];
                snprintf(buf, size, fmt.c_str(), args...);
                std::string msgFull = std::string(buf, buf + size - 1);
                delete[] buf;

                Send("Debug", msgFull);
            }
        }
    }

    void Info(const std::string &msg)
    {
        if (severity <= Severity::Info)
            Send("Info", msg);
    }

    template<typename... Args>
    void Info(const std::string &fmt, Args... args)
    {
        if (severity <= Severity::Info)
        {
            if (int size = snprintf(nullptr, 0, fmt.c_str(), args...); size > 0)
            {
                ++size;
                auto *buf = new char[size];
                snprintf(buf, size, fmt.c_str(), args...);
                std::string msgFull = std::string(buf, buf + size - 1);
                delete[] buf;

                Send("Info", msgFull);
            }
        }
    }
};

extern Log *logger;

#endif