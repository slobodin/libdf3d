#include "df3d_pch.h"
#include "Log.h"

#ifdef __ANDROID__
#include <android/log.h>
#endif // __ANDROID__

#if defined(DF3D_WINDOWS_PHONE)
#include <wrl.h>
#include <wrl/client.h>
#endif

#if defined(DF3D_WINDOWS)
#include <Windows.h>
#endif

namespace df3d { namespace base {

const size_t MAX_LOG_SIZE = 2 << 18;

enum class MessageType
{
    DEBUG,
    MESSAGE,
    WARNING,
    CRITICAL,
    GAME,
    SCRIPT,
    NONE,

    COUNT
};

class LoggerImpl
{
public:
    virtual ~LoggerImpl() { }

    virtual void writeBuffer(const std::string &buffer, MessageType type) = 0;
};

#ifdef DF3D_WINDOWS

class WindowsLoggerImpl : public LoggerImpl
{
    HANDLE m_consoleHandle = nullptr;
    std::unordered_map<MessageType, WORD> m_consoleColors;

public:
    WindowsLoggerImpl()
    {
        m_consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

        m_consoleColors =
        {
            { MessageType::DEBUG, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED },
            { MessageType::MESSAGE, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY },
            { MessageType::WARNING, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY },
            { MessageType::CRITICAL, FOREGROUND_RED | FOREGROUND_INTENSITY },
            { MessageType::GAME, FOREGROUND_GREEN | FOREGROUND_INTENSITY },
            { MessageType::SCRIPT, FOREGROUND_GREEN | FOREGROUND_INTENSITY },
            { MessageType::NONE, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED }
        };

        assert(m_consoleColors.size() == (int)MessageType::COUNT);
    }

    virtual void writeBuffer(const std::string &buffer, MessageType type) override
    {
        if (m_consoleHandle)
            SetConsoleTextAttribute(m_consoleHandle, m_consoleColors[type]);

        switch (type)
        {
        case MessageType::DEBUG:
            std::cerr << "[debug]: ";
            break;
        case MessageType::MESSAGE:
            std::cerr << "[message]: ";
            break;
        case MessageType::WARNING:
            std::cerr << "[warning]: ";
            break;
        case MessageType::CRITICAL:
            std::cerr << "[critical]: ";
            break;
        case MessageType::GAME:
            std::cerr << "[game]: ";
            break;
        case MessageType::SCRIPT:
            std::cerr << "[script]: ";
            break;
        case MessageType::NONE:
            break;
        default:
            break;
        }

        std::cerr << buffer << std::endl;

        if (m_consoleHandle)
            SetConsoleTextAttribute(m_consoleHandle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
    }
};

#endif

#ifdef DF3D_WINDOWS_PHONE

class WindowsRTLoggerImpl : public LoggerImpl
{
public:
    WindowsRTLoggerImpl()
    {
    }

    virtual void writeBuffer(const std::string &buffer, MessageType type) override
    {
        OutputDebugStringA(buffer.c_str());
        OutputDebugStringA("\n");
    }
};

#endif

Log::Log()
{
#ifdef DF3D_WINDOWS
    m_impl.reset(new WindowsLoggerImpl());
#else
    m_impl.reset(new WindowsRTLoggerImpl());
#endif
}

Log::~Log()
{

}

Log &Log::instance()
{
    static Log m_instance;

    return m_instance;
}

Log &Log::operator<< (const char *text)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    m_buffer << text << " ";

    return *this;
}

Log &Log::operator<< (const wchar_t *text)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    m_buffer << text << " ";

    return *this;
}

Log &Log::operator<< (const std::string &text)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    m_buffer << text << " ";

    return *this;
}

Log &Log::operator<< (int num)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    m_buffer << num << " ";

    return *this;
}

Log &Log::operator<< (long num)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    m_buffer << num << " ";

    return *this;
}

Log &Log::operator<< (long long num)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    m_buffer << num << " ";

    return *this;
}

Log &Log::operator<< (unsigned num)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    m_buffer << num << " ";

    return *this;
}

Log &Log::operator<< (double num)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    m_buffer << num << " ";

    return *this;
}

Log &Log::operator<< (const glm::vec2 &v)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    m_buffer << "[x: " << v.x << " y: " << v.y << "] ";

    return *this;
}

Log &Log::operator<< (const glm::vec3 &v)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    m_buffer << "[x: " << v.x << " y: " << v.y << " z: " << v.z << "] ";

    return *this;
}

Log &Log::operator<< (const glm::vec4 &v)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    m_buffer << "[x: " << v.x << " y: " << v.y << " z: " << v.z << " w: " << v.w << "] ";

    return *this;
}

Log &Log::operator<< (const LoggerManipulator &man)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

#if 0
    // Print time.
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cerr << std::ctime(&now);
#endif

    m_impl->writeBuffer(m_buffer.str(), man.m_type);

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

void Log::printWithoutFormat(const char *str)
{
    m_impl->writeBuffer(str, MessageType::NONE);
}

const std::string &Log::logData() const
{
    return m_logData;
}

Log &glog = Log::instance();
const LoggerManipulator logdebug(MessageType::DEBUG);
const LoggerManipulator logmess(MessageType::MESSAGE);
const LoggerManipulator logwarn(MessageType::WARNING);
const LoggerManipulator logcritical(MessageType::CRITICAL);
const LoggerManipulator loggame(MessageType::GAME);
const LoggerManipulator logscript(MessageType::SCRIPT);

} }
