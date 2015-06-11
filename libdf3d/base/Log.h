#pragma once

#include <iostream>
#include <sstream>

namespace df3d { namespace base {

enum class MessageType;

class LoggerManipulator
{
    friend class Log;

    MessageType m_type;

public:
    LoggerManipulator(const MessageType type)
        : m_type(type)
    { }
};

/**
 * @brief Simple log support.
 */
class DF3D_DLL Log
{
    std::stringstream m_buffer;
    std::recursive_mutex m_lock;

    std::string m_logData;

    Log();

public:
    static Log &instance();

    Log &operator<< (const char *text);
    Log &operator<< (const std::string &text);
    Log &operator<< (int num);
    Log &operator<< (long num);
    Log &operator<< (long long num);
    Log &operator<< (unsigned num);
    Log &operator<< (double num);
    Log &operator<< (const glm::vec2 &v);
    Log &operator<< (const glm::vec3 &v);
    Log &operator<< (const glm::vec4 &v);

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
extern const DF3D_DLL LoggerManipulator loglua;

} }