#include "df3d_pch.h"
#include "Log.h"

#ifdef DF3D_ANDROID
#include <android/log.h>
#endif

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
    virtual ~LoggerImpl() = default;

    virtual void writeBuffer(const std::string &buffer, MessageType type) = 0;
};

class StdoutLogger : public LoggerImpl
{
public:
    StdoutLogger() = default;

    virtual void writeBuffer(const std::string &buffer, MessageType type) override
    {
        switch (type)
        {
        case MessageType::DEBUG:
            std::cout << "[debug]: ";
            break;
        case MessageType::MESSAGE:
            std::cout << "[message]: ";
            break;
        case MessageType::WARNING:
            std::cout << "[warning]: ";
            break;
        case MessageType::CRITICAL:
            std::cout << "[critical]: ";
            break;
        case MessageType::GAME:
            std::cout << "[game]: ";
            break;
        case MessageType::SCRIPT:
            std::cout << "[script]: ";
            break;
        case MessageType::NONE:
            break;
        default:
            break;
        }

        std::cout << buffer << std::endl;
    }
};

#ifdef DF3D_WINDOWS

class WindowsLoggerImpl : public StdoutLogger
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

        StdoutLogger::writeBuffer(buffer, type);

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

#ifdef DF3D_ANDROID

class AndroidLoggerImpl : public LoggerImpl
{
    std::unordered_map<int, android_LogPriority> m_priorities;

public:
    AndroidLoggerImpl()
    {
        m_priorities =
        {
            { (int)MessageType::DEBUG, ANDROID_LOG_DEBUG },
            { (int)MessageType::MESSAGE, ANDROID_LOG_INFO },
            { (int)MessageType::WARNING, ANDROID_LOG_WARN },
            { (int)MessageType::CRITICAL, ANDROID_LOG_FATAL },
            { (int)MessageType::GAME, ANDROID_LOG_INFO },
            { (int)MessageType::SCRIPT, ANDROID_LOG_INFO },
            { (int)MessageType::NONE, ANDROID_LOG_UNKNOWN }
        };

        assert(m_priorities.size() == (int)MessageType::COUNT);
    }

    virtual void writeBuffer(const std::string &buffer, MessageType type) override
    {
        __android_log_print(m_priorities[(int)type], "df3d_android", "%s", buffer.c_str());
    }
};

#endif

Log::Log()
{
#if defined(DF3D_WINDOWS)
    m_impl.reset(new WindowsLoggerImpl());
#elif defined(DF3D_WINDOWS_PHONE)
    m_impl.reset(new WindowsRTLoggerImpl());
#elif defined(DF3D_ANDROID)
    m_impl.reset(new AndroidLoggerImpl());
#else
    m_impl.reset(new StdoutLogger());
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
    m_impl->writeBuffer(str, MessageType::DEBUG);
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
