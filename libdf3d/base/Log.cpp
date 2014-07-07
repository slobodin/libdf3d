#include "df3d_pch.h"
#include "Log.h"

#ifdef __ANDROID__
#include <android/log.h>
#endif // __ANDROID__

namespace df3d { namespace base {

const size_t MAX_LOG_SIZE = 2 << 18;

Log::Log()
{
}

Log &Log::instance()
{
    static Log m_instance;

    return m_instance;
}

Log &Log::operator<< (const char *text)
{
    std::unique_lock<std::recursive_mutex> lock(m_lock);

    m_buffer << text << " ";

    return *this;
}

Log &Log::operator<< (const std::string &text)
{
    std::unique_lock<std::recursive_mutex> lock(m_lock);

    m_buffer << text << " ";

    return *this;
}

Log &Log::operator<< (int num)
{
    std::unique_lock<std::recursive_mutex> lock(m_lock);

    m_buffer << num << " ";

    return *this;
}

Log &Log::operator<< (long num)
{
    std::unique_lock<std::recursive_mutex> lock(m_lock);

    m_buffer << num << " ";

    return *this;
}

Log &Log::operator<< (long long num)
{
    std::unique_lock<std::recursive_mutex> lock(m_lock);

    m_buffer << num << " ";

    return *this;
}

Log &Log::operator<< (unsigned num)
{
    std::unique_lock<std::recursive_mutex> lock(m_lock);

    m_buffer << num << " ";

    return *this;
}

Log &Log::operator<< (double num)
{
    std::unique_lock<std::recursive_mutex> lock(m_lock);

    m_buffer << num << " ";

    return *this;
}

Log &Log::operator<< (const glm::vec2 &v)
{
    std::unique_lock<std::recursive_mutex> lock(m_lock);

    m_buffer << "[x: " << v.x << " y: " << v.y << "] ";

    return *this;
}

Log &Log::operator<< (const glm::vec3 &v)
{
    std::unique_lock<std::recursive_mutex> lock(m_lock);

    m_buffer << "[x: " << v.x << " y: " << v.y << " z: " << v.z << "] ";

    return *this;
}

Log &Log::operator<< (const glm::vec4 &v)
{
    std::unique_lock<std::recursive_mutex> lock(m_lock);

    m_buffer << "[x: " << v.x << " y: " << v.y << " z: " << v.z << " w: " << v.w << "] ";

    return *this;
}

Log &Log::operator<< (const LoggerManipulator &man)
{
    std::unique_lock<std::recursive_mutex> lock(m_lock);

#if 0
    // Print time.
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cerr << std::ctime(&now);
#endif

#if defined(__ANDROID__)
    android_LogPriority androidPriority = ANDROID_LOG_UNKNOWN;
#endif

    switch (man.m_type)
    {
    case MT_DEBUG:
        std::cerr << "[debug]: ";
#if defined(__ANDROID__)
        androidPriority = ANDROID_LOG_DEBUG;
#endif
        break;

    case MT_MESSAGE:
        std::cerr << "[message]: ";
#if defined(__ANDROID__)
        androidPriority = ANDROID_LOG_INFO;
#endif
        break;

    case MT_WARNING:
        std::cerr << "[warning]: ";
#if defined(__ANDROID__)
        androidPriority = ANDROID_LOG_WARN;
#endif
        break;

    case MT_CRITICAL:
        std::cerr << "[critical]: ";
#if defined(__ANDROID__)
        androidPriority = ANDROID_LOG_FATAL;
#endif
        break;

    case MT_GAME:
        std::cerr << "[game]: ";
#if defined(__ANDROID__)
        androidPriority = ANDROID_LOG_INFO;
#endif
        break;
    }

#ifdef __ANDROID__
    __android_log_print(androidPriority, "libdf3d", "%s", m_buffer.str().c_str());
#else
    std::cerr << m_buffer.str() << std::endl;
#endif

    m_logData += m_buffer.str();
    m_logData += "\n";

    if (m_logData.size() > MAX_LOG_SIZE)
    {
        m_logData.clear();
        m_logData.shrink_to_fit();
    }

    m_buffer.str("");

    return *this;
}

const std::string &Log::logData() const
{
    return m_logData;
}

Log &glog = Log::instance();
const LoggerManipulator logdebug(MT_DEBUG);
const LoggerManipulator logmess(MT_MESSAGE);
const LoggerManipulator logwarn(MT_WARNING);
const LoggerManipulator logcritical(MT_CRITICAL);
const LoggerManipulator loggame(MT_GAME);

} }
