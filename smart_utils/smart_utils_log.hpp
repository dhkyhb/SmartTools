#ifndef SMARTTOOLS_SMART_UTILS_LOG_HPP
#define SMARTTOOLS_SMART_UTILS_LOG_HPP

#include "../kernel.hpp"
#include "../smart_utils/smart_utils_strings.hpp"

#include <mutex>
#include <android/log.h>

namespace smart::utils::log
{

using smart::utils::strings::Strings;

constexpr Jint LOG_CACHE_SIZE = 4096;
constexpr Jint LOG_TAG_BUF_SIZE = 32;

enum class LogType
{
    INFO, DEBUG, ERROR,
};

typedef struct
{
    Jchar cache[LOG_CACHE_SIZE / 2];

    std::mutex localMutex;
} LogHex;

typedef struct
{
    Jchar cache[LOG_CACHE_SIZE];
    Jchar target[LOG_TAG_BUF_SIZE];

    tm *nowTime;
    std::mutex localMutex;
} LogAttr;

constexpr Jchar LOG_DEFAULT_TARGET[] = "Tools";
constexpr Jchar LOG_INFO_TAMPLATE[] = "[%s][INFO][%04d-%02d-%02d %02d:%02d:%02d][%s]\r\n";
constexpr Jchar LOG_DEBG_TAMPLATE[] = "[%s][DEBG][%04d-%02d-%02d %02d:%02d:%02d][%s]\r\n";
constexpr Jchar LOG_EROR_TAMPLATE[] = "[%s][EROR][%04d-%02d-%02d %02d:%02d:%02d][%s]\r\n";

class Log
{
public:
    static Log &Instance()
    {
        static Log *obj = nullptr;
        if (obj == nullptr)
            obj = new Log();
        return (*obj);
    }

    void SetTag(const Jchar *v)
    {
        if (v == nullptr)
            return;

        memset(this->mLogAttr.target, 0, sizeof(this->mLogAttr.target));
        memcpy(this->mLogAttr.target, v, strlen(v));
    }

    void PrintHex(const Jbyte *v, Jint vLen)
    {
        if ((v == nullptr) || (vLen < 1))
            return;

        this->mLogHex.localMutex.lock();
        if (!Strings::Bytes2String(v, vLen, this->mLogHex.cache, sizeof(this->mLogHex.cache)))
            return;

        this->Print<LogType::DEBUG>(this->mLogHex.cache);
        this->mLogHex.localMutex.unlock();
    }

    template<LogType _type, typename...Args>
    void Print(const Jchar *v, Args...args)
    {
        std::lock_guard<std::mutex> lockGuard(this->mLogAttr.localMutex);
        time_t localTime = 0;

        if (v == nullptr)
            return;

        memset(this->mLogAttr.cache, 0, sizeof(this->mLogAttr.cache));
        if (snprintf(this->mLogAttr.cache, sizeof(this->mLogAttr.cache), v, args...) < 1)
            return;

        localTime = time(nullptr);
        if (this->mLogAttr.nowTime = localtime(&localTime);this->mLogAttr.nowTime == nullptr)
            return;

        this->mLogAttr.nowTime->tm_year += 1900;
        ++this->mLogAttr.nowTime->tm_mon;
        if (_type == LogType::INFO)
            this->PrintInfo();
        else if (_type == LogType::DEBUG)
            this->PrintDebug();
        else if (_type == LogType::ERROR)
            this->PrintError();
    }

private:
    LogHex mLogHex;
    LogAttr mLogAttr;

    Log() :
        mLogHex{},
        mLogAttr{}
    {
        memset(this->mLogAttr.target, 0, sizeof(this->mLogAttr.target));
        memcpy(this->mLogAttr.target, LOG_DEFAULT_TARGET, strlen(LOG_DEFAULT_TARGET));
    }

    void PrintInfo()
    {
        __android_log_print(
            ANDROID_LOG_INFO,
            this->mLogAttr.target,
            LOG_INFO_TAMPLATE,
            this->mLogAttr.target,
            this->mLogAttr.nowTime->tm_year,
            this->mLogAttr.nowTime->tm_mon,
            this->mLogAttr.nowTime->tm_mday,
            this->mLogAttr.nowTime->tm_hour,
            this->mLogAttr.nowTime->tm_min,
            this->mLogAttr.nowTime->tm_sec,
            this->mLogAttr.cache
        );
    }

    void PrintDebug()
    {
        __android_log_print(
            ANDROID_LOG_DEBUG,
            this->mLogAttr.target,
            LOG_DEBG_TAMPLATE,
            this->mLogAttr.target,
            this->mLogAttr.nowTime->tm_year,
            this->mLogAttr.nowTime->tm_mon,
            this->mLogAttr.nowTime->tm_mday,
            this->mLogAttr.nowTime->tm_hour,
            this->mLogAttr.nowTime->tm_min,
            this->mLogAttr.nowTime->tm_sec,
            this->mLogAttr.cache
        );
    }

    void PrintError()
    {
        __android_log_print(
            ANDROID_LOG_ERROR,
            this->mLogAttr.target,
            LOG_EROR_TAMPLATE,
            this->mLogAttr.target,
            this->mLogAttr.nowTime->tm_year,
            this->mLogAttr.nowTime->tm_mon,
            this->mLogAttr.nowTime->tm_mday,
            this->mLogAttr.nowTime->tm_hour,
            this->mLogAttr.nowTime->tm_min,
            this->mLogAttr.nowTime->tm_sec,
            this->mLogAttr.cache
        );
    }
};

}

#endif //SMARTTOOLS_SMART_UTILS_LOG_HPP
