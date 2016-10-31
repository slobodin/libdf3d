#pragma once

#include <iostream>
#include <sstream>

namespace df3d {

#ifndef DF3D_DISABLE_LOGGING

class LoggerImpl;

class Log
{
public:
    enum LogChannel
    {
        CHANNEL_DEBUG,
        CHANNEL_MESS,
        CHANNEL_WARN,
        CHANNEL_CRITICAL,
        CHANNEL_GAME,

        CHANNEL_COUNT
    };

private:
    std::stringstream m_buffer;
    std::recursive_mutex m_lock;

    std::string m_logData;

    unique_ptr<LoggerImpl> m_impl;

    Log();
    ~Log();

public:
    static Log& instance();

    void print(LogChannel channel, const char *fmt, ...);
    void vprint(LogChannel channel, const char *fmt, va_list argList);
};

extern Log &glog;

#define DFLOG_DEBUG(...) df3d::glog.print(df3d::Log::CHANNEL_DEBUG, __VA_ARGS__)
#define DFLOG_MESS(...) df3d::glog.print(df3d::Log::CHANNEL_MESS, __VA_ARGS__)
#define DFLOG_WARN(...) df3d::glog.print(df3d::Log::CHANNEL_WARN, __VA_ARGS__)
#define DFLOG_CRITICAL(...) df3d::glog.print(df3d::Log::CHANNEL_CRITICAL, __VA_ARGS__)
#define DFLOG_GAME(...) df3d::glog.print(df3d::Log::CHANNEL_GAME, __VA_ARGS__)

#else

#define DFLOG_DEBUG(...) ((void)0)
#define DFLOG_MESS(...) ((void)0)
#define DFLOG_WARN(...) ((void)0)
#define DFLOG_CRITICAL(...) ((void)0)
#define DFLOG_GAME(...) ((void)0)

#endif

}
