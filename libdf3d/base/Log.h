#pragma once

#include <iostream>
#include <sstream>

namespace df3d { namespace base {

enum class MessageType;
class LoggerImpl;

class LoggerManipulator
{
    friend class Log;

    MessageType m_type;

public:
    LoggerManipulator(const MessageType type)
        : m_type(type)
    { }
};

//! Simple log support.
class DF3D_DLL Log
{
    std::stringstream m_buffer;
    std::recursive_mutex m_lock;

    std::string m_logData;

    std::unique_ptr<LoggerImpl> m_impl;

    Log();
    ~Log();

public:
    static Log &instance();

    template<typename T>
    Log& operator<< (const T &t)
    {
        std::lock_guard<std::recursive_mutex> lock(m_lock);

        m_buffer << t << " ";

        return *this;
    }

    Log& operator<< (const glm::vec2 &v)
    {
        std::lock_guard<std::recursive_mutex> lock(m_lock);

        m_buffer << "[x: " << v.x << " y: " << v.y << "] ";

        return *this;
    }

    Log& operator<< (const glm::vec3 &v)
    {
        std::lock_guard<std::recursive_mutex> lock(m_lock);

        m_buffer << "[x: " << v.x << " y: " << v.y << " z: " << v.z << "] ";

        return *this;
    }

    Log& operator<< (const glm::vec4 &v)
    {
        std::lock_guard<std::recursive_mutex> lock(m_lock);

        m_buffer << "[x: " << v.x << " y: " << v.y << " z: " << v.z << " w: " << v.w << "] ";

        return *this;
    }

    Log &operator<< (const LoggerManipulator &man);

    void printWithoutFormat(const char *str);

    const std::string &logData() const;
};

extern DF3D_DLL Log &glog;
extern const DF3D_DLL LoggerManipulator logdebug;
extern const DF3D_DLL LoggerManipulator logmess;
extern const DF3D_DLL LoggerManipulator logwarn;
extern const DF3D_DLL LoggerManipulator logcritical;
extern const DF3D_DLL LoggerManipulator loggame;
extern const DF3D_DLL LoggerManipulator logscript;

} }
