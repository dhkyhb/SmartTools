#ifndef SMARTTOOLS_SMART_ANDROID_REMOTE_ACTIVE_1_HPP
#define SMARTTOOLS_SMART_ANDROID_REMOTE_ACTIVE_1_HPP

#include "../kernel.hpp"
#include "../smart_utils/smart_utils_strings.hpp"

#include "smart_android_config.hpp"

#include <fcntl.h>

#include <APosSecurityManager.h>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace smart::android::remote::active1
{

using smart::utils::strings::Strings;
using smart::android::config::Config;

constexpr Jint REMOTE_ACTIVE1_AREA_CODE_SIZE = 2048;
constexpr Jint REMOTE_ACTIVE1_REQUEST_CACHE_SIZE = 4096;

typedef struct
{
    const Jchar *sn;
    const Jchar *model;
    const Jchar *customer;
    const Jchar *subCustomer;
    const Jchar *hardwareVersion;
    const Jchar *softwareVersion;

    Jchar code[REMOTE_ACTIVE1_AREA_CODE_SIZE];
    Jbyte codeArray[REMOTE_ACTIVE1_AREA_CODE_SIZE];
} RemoteActive1Area;

typedef struct
{
    Jint activeCodeLen;
    Jchar *activeCode;
    APosSecurityManager *context;

    std::mutex lock;
} POSSupport;

typedef struct BoostSupport
{
    boost::asio::io_context context;
    boost::asio::ip::tcp::resolver resolver;

    boost::beast::tcp_stream httpStream;
    boost::beast::http::request<boost::beast::http::string_body> request;
    boost::beast::http::response<boost::beast::http::dynamic_body> response;
    boost::beast::flat_buffer buffer;
    boost::beast::error_code errorCode;

    timeval readTimeouts;
    timeval writeTimeouts;

    std::stringstream jsonHandle;
    Jchar cache[REMOTE_ACTIVE1_REQUEST_CACHE_SIZE];

    BoostSupport() :
        context{},
        resolver{this->context},
        httpStream{this->context},
        request{},
        response{},
        buffer{},
        errorCode{},
        readTimeouts{},
        writeTimeouts{},
        jsonHandle{},
        cache{}
    {}
} BoostSupport;

constexpr Jint REMOTEACTIVE1_APPLY_ACTIVE_CODE_BY_SP_OF_MODE = 0;
constexpr Jint REMOTEACTIVE1_REQUEST_TIMEOUTS = 3;
constexpr Jint REMOTEACTIVE1_RESPONSE_TIMEOUTS = 3;

constexpr Jchar REMOTEACTIVE1_TARGET_REQUEST_REGISTER[] = "/posService/requestunlock";
constexpr Jchar REMOTEACTIVE1_TARGET_REQUEST_SEARCH[] = "/posService/pollunlock";
constexpr Jchar REMOTEACTIVE1_TARGET_REQUEST_UPDATE_STATE[] = "/posService/unlocksuccess";
constexpr Jchar REMOTEACTIVE1_REQUEST_FORMATS[] =
    "dsn=%s&activateCode=%s&model=%s&ptype=%d&clientCode=%s&"
    "sonClientCode=%s&hardwareVersion=%s&softwareVersion=%s";

// New version, it's based on HTTP protocol to unlock request.
class RemoteActive1
{
public:
    static RemoteActive1 &Instance()
    {
        static RemoteActive1 *obj = nullptr;
        if (obj == nullptr)
            obj = new RemoteActive1();
        return (*obj);
    }

    Jbool ApplyUnlock()
    {
        Jbool state = false;

        if (!this->Check())
            return false;

        do
        {
            if (!this->POSInit())
                break;
            if (!this->GetActiveCodeBySP())
                break;
            if (!this->ExecuteUnlock())
                break;
            if (!this->SetActiveCodeBySP())
                break;

            state = true;
        } while (false);

        this->POSRelease();
        return state;
    }

    RemoteActive1 &SetSN(const Jchar *sn)
    {
        this->mRemoteActive1Area.sn = sn;
        return (*this);
    }

    RemoteActive1 &SetModel(const Jchar *model)
    {
        this->mRemoteActive1Area.model = model;
        return (*this);
    }

    RemoteActive1 &SetCustomer(const Jchar *customer)
    {
        this->mRemoteActive1Area.customer = customer;
        return (*this);
    }

    RemoteActive1 &SetSubCustomer(const Jchar *subCustomer)
    {
        this->mRemoteActive1Area.subCustomer = subCustomer;
        return (*this);
    }

    RemoteActive1 &SetHardwareVersion(const Jchar *hardwareVersion)
    {
        this->mRemoteActive1Area.hardwareVersion = hardwareVersion;
        return (*this);
    }

    RemoteActive1 &SetSoftwareVersion(const Jchar *softwareVersion)
    {
        this->mRemoteActive1Area.softwareVersion = softwareVersion;
        return (*this);
    }

private:
    POSSupport mPOSSupport;
    BoostSupport mBoostSupport;
    RemoteActive1Area mRemoteActive1Area;

    RemoteActive1() :
        mPOSSupport{},
        mBoostSupport{},
        mRemoteActive1Area{}
    {}

    Jbool Check()
    {
        return ((this->mRemoteActive1Area.sn != nullptr)
                && (this->mRemoteActive1Area.model != nullptr)
                && (this->mRemoteActive1Area.customer != nullptr)
                && (this->mRemoteActive1Area.subCustomer != nullptr)
                && (this->mRemoteActive1Area.hardwareVersion != nullptr)
                && (this->mRemoteActive1Area.softwareVersion != nullptr));
    }

    Jbool POSInit()
    {
        this->mPOSSupport.lock.lock();
        memset(&this->mPOSSupport, 0, sizeof(this->mPOSSupport));
        this->mPOSSupport.context = APosSecurityManager_new();
        return (this->mPOSSupport.context != nullptr);
    }

    void POSRelease()
    {
        if (this->mPOSSupport.activeCode != nullptr)
            free(this->mPOSSupport.activeCode);
        if (this->mPOSSupport.context != nullptr)
            APosSecurityManager_delete(this->mPOSSupport.context);

        this->mPOSSupport.lock.unlock();
    }

    Jbool ExecuteUnlock()
    {
        return this->Register();
    }

    Jbool Register()
    {
        Jbool state = false;

        if (!this->NetOpen())
            return state;

        do
        {
            if (!this->NetSetParameter())
                break;

            this->mBoostSupport.request.method(boost::beast::http::verb::post);
            this->mBoostSupport.request.target(REMOTEACTIVE1_TARGET_REQUEST_REGISTER);
            this->mBoostSupport.request.version(11);
            if (!this->Mashal())
                break;

            this->mBoostSupport.request.set(boost::beast::http::field::body, this->mBoostSupport.cache);
            this->mBoostSupport.request.content_length(strlen(this->mBoostSupport.cache));

//            Log::Instance().Print<LogType::DEBUG>("Request: \r\n%s\r\n", this->mBoostSupport.request);
            boost::beast::http::write(
                this->mBoostSupport.httpStream,
                this->mBoostSupport.request,
                this->mBoostSupport.errorCode
            );
            if (this->mBoostSupport.errorCode != boost::beast::errc::success)
                break;

            boost::beast::http::read(
                this->mBoostSupport.httpStream,
                this->mBoostSupport.buffer,
                this->mBoostSupport.response,
                this->mBoostSupport.errorCode
            );
            if (this->mBoostSupport.errorCode != boost::beast::errc::success)
                break;

            if (!this->Parse())
                break;

            state = true;
        } while (false);

        if (!this->NetClose())
            state = false;

        return state;
    }


    Jbool NetOpen()
    {
        constexpr Jint EXECUTE_UNLOCK_MESSAGE_CONV_PORT_SIZE = 64;
        static Jchar convPort[EXECUTE_UNLOCK_MESSAGE_CONV_PORT_SIZE];

        memset(convPort, 0, sizeof(convPort));
        if (snprintf(convPort, sizeof(convPort), "%d", Config::Instance().GetRemoteActivationPort()) < 1)
            return false;

        auto &&results = this->mBoostSupport.resolver.resolve(
            Config::Instance().GetRemoteActivationAddress(),
            convPort
        );

        this->mBoostSupport.httpStream.connect(results, this->mBoostSupport.errorCode);
        return (this->mBoostSupport.errorCode == boost::beast::errc::success);
    }

    Jbool NetSetParameter()
    {
        this->mBoostSupport.readTimeouts.tv_sec = REMOTEACTIVE1_REQUEST_TIMEOUTS;
        this->mBoostSupport.writeTimeouts.tv_sec = REMOTEACTIVE1_RESPONSE_TIMEOUTS;

        if (setsockopt(
            this->mBoostSupport.httpStream.socket().native_handle(),
            SOL_SOCKET,
            SO_SNDTIMEO,
            reinterpret_cast<Jchar *>(&this->mBoostSupport.writeTimeouts),
            sizeof(this->mBoostSupport.writeTimeouts)
        ) != 0)
            return false;

        if (setsockopt(
            this->mBoostSupport.httpStream.socket().native_handle(),
            SOL_SOCKET,
            SO_RCVTIMEO,
            reinterpret_cast<Jchar *>(&this->mBoostSupport.readTimeouts),
            sizeof(this->mBoostSupport.readTimeouts)
        ) != 0)
            return false;

        auto &&flag = fcntl(this->mBoostSupport.httpStream.socket().native_handle(), F_GETFL, 0);
        fcntl(this->mBoostSupport.httpStream.socket().native_handle(), F_SETFL, (flag & ~O_NONBLOCK));
        Log::Instance().Print<LogType::INFO>("remote activation socket initialization successful");
        return true;
    }

    Jbool NetClose()
    {
        this->mBoostSupport.request.clear();
        this->mBoostSupport.response.clear();
        this->mBoostSupport.buffer.clear();
        this->mBoostSupport.jsonHandle.str("");

        this->mBoostSupport.httpStream.socket().shutdown(
            boost::asio::ip::tcp::socket::shutdown_both,
            this->mBoostSupport.errorCode
        );
        return (this->mBoostSupport.errorCode == boost::beast::errc::success);
    }

    Jbool SetActiveCodeBySP()
    {
        Jint ret = 0;

        if (strlen(this->mRemoteActive1Area.code) > (sizeof(this->mRemoteActive1Area.codeArray) * 2 + 2))
            return false;
        if (ret = Strings::String2Bytes(
                this->mRemoteActive1Area.code,
                this->mRemoteActive1Area.codeArray,
                sizeof(this->mRemoteActive1Area.codeArray)
            );ret == 0)
            return false;

        return (APosSecurityManager_SysRemoteUnlockRsp(
            this->mPOSSupport.context,
            REMOTEACTIVE1_APPLY_ACTIVE_CODE_BY_SP_OF_MODE,
            reinterpret_cast<Jchar *>(this->mRemoteActive1Area.codeArray),
            ret
        ) == 0);
    }

    Jbool GetActiveCodeBySP()
    {
        if (APosSecurityManager_SysRemoteUnlockReq(
            this->mPOSSupport.context,
            REMOTEACTIVE1_APPLY_ACTIVE_CODE_BY_SP_OF_MODE,
            &this->mPOSSupport.activeCode,
            &this->mPOSSupport.activeCodeLen
        ) != 0)
            return false;

        Log::Instance().Print<LogType::DEBUG>("apply activation code by the SP is successful");
        if (this->mPOSSupport.activeCode == nullptr)
            return false;
        if (this->mPOSSupport.activeCodeLen > static_cast<Jint>(
            sizeof(this->mRemoteActive1Area.code) / 2 - 2
        ))
            return false;

        if (!Strings::Bytes2String(
            reinterpret_cast<Jbyte *>(&this->mPOSSupport.activeCode[3]),
            (this->mPOSSupport.activeCodeLen -= 3),
            this->mRemoteActive1Area.code,
            sizeof(this->mRemoteActive1Area.code)
        ))
            return false;

        Log::Instance().Print<LogType::DEBUG>("this is got active code by the SP");
        Log::Instance().Print<LogType::DEBUG>(this->mRemoteActive1Area.code);
        return true;
    }

    Jbool Mashal()
    {
        memset(this->mBoostSupport.cache, 0, sizeof(this->mBoostSupport.cache));
        return (snprintf(
            this->mBoostSupport.cache,
            (sizeof(this->mBoostSupport.cache) - 1),
            REMOTEACTIVE1_REQUEST_FORMATS,
            this->mRemoteActive1Area.sn,
            this->mRemoteActive1Area.code,
            this->mRemoteActive1Area.model,
            1,
            this->mRemoteActive1Area.customer,
            this->mRemoteActive1Area.subCustomer,
            this->mRemoteActive1Area.hardwareVersion,
            this->mRemoteActive1Area.softwareVersion
        ) > 0);
    }

    Jbool Parse()
    {
        return false;
    }
};

}

#endif //SMARTTOOLS_SMART_ANDROID_REMOTE_ACTIVE_1_HPP
