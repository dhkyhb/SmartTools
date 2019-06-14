#ifndef SMARTTOOLS_SMART_ANDROID_REMOTE_ACTIVE_1_HPP
#define SMARTTOOLS_SMART_ANDROID_REMOTE_ACTIVE_1_HPP

#include "../kernel.hpp"
#include "../smart_utils/smart_utils_strings.hpp"
#include "../smart_utils/smart_utils_simple_http.hpp"
#include "../smart_resouces/smart_resouces_errors.hpp"

#include "smart_android_config.hpp"

#include <APosSecurityManager.h>
#include <boost/property_tree/json_parser.hpp>

#include <fcntl.h>

namespace smart::android::remote::active1
{

using smart::utils::strings::Strings;
using smart::utils::simple::http::Http;
using smart::utils::simple::http::HttpMethod;
using smart::resouces::errors::Errors;
using smart::resouces::errors::ErrorsType;

constexpr Jint REMOTE_ACTIVE1_AREA_CODE_SIZE = 2048;
constexpr Jint REMOTE_ACTIVE1_CLIENT_BUF_SIZE = 4096;

enum class RemoteActive1Procedure
{
    REGISTER,
    CHECK,
    RECORD,
};

typedef struct
{
    const Jchar *sn;
    const Jchar *model;
    const Jchar *customer;
    const Jchar *subCustomer;
    const Jchar *hardwareVersion;
    const Jchar *softwareVersion;

    Jint timeouts;
    Jint port;
    const Jchar *address;

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

typedef struct RemoteActive1Client
{
    Jint socket;
    Jchar target[REMOTE_ACTIVE1_CLIENT_BUF_SIZE];
    Jchar cache[REMOTE_ACTIVE1_CLIENT_BUF_SIZE];

    sockaddr_in sockaddrIn;
    timeval readCountTimes;
    timeval writeCountTimes;

    std::stringstream jsonStream;
    boost::property_tree::ptree jsonRoot;

    Http http;

    RemoteActive1Client() :
        socket{},
        target{},
        cache{},
        sockaddrIn{},
        readCountTimes{},
        writeCountTimes{},
        jsonStream{},
        jsonRoot{},
        http{}
    {}
} RemoteActive1Client;

constexpr Jint REMOTEACTIVE1_APPLY_ACTIVE_CODE_BY_SP_OF_MODE = 0;
constexpr Jint REMOTEACTIVE1_REQUEST_TIMEOUTS = 3;
constexpr Jint REMOTEACTIVE1_RESPONSE_TIMEOUTS = 3;

constexpr Jchar REMOTEACTIVE1_TARGET_REQUEST_REGISTER[] =
    "/posService/requestunlock?"
    "dsn=%s&activateCode=%s&model=%s&ptype=%d&clientCode=%s&"
    "sonClientCode=%s&hardwareVersion=%s&softwareVersion=%s";
constexpr Jchar REMOTEACTIVE1_TARGET_REQUEST_SEARCH[] = "/posService/pollunlock?dsn=%s";
constexpr Jchar REMOTEACTIVE1_TARGET_REQUEST_UPDATE_STATE[] = "/posService/unlocksuccess?dsn=%s";

constexpr Jchar REMOTEACTIVE1_TARGET_RESPONSE_STATE[] = "success";
constexpr Jchar REMOTEACTIVE1_TARGET_RESPONSE_MSG[] = "msg";

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
        {
            Errors::Instance().SetErrorType<ErrorsType::CONFIG_INVALID>();
            return false;
        }

        do
        {
            if (!this->POSInit())
            {
                Errors::Instance().SetErrorType<ErrorsType::POS_NEED_SHORT_SMALL_BATTERY_OR_REBOOT>();
                break;
            }

            if (!this->GetActiveCodeBySP())
                break;

            if (!this->ExecuteUnlock())
            {
                Errors::Instance().SetErrorType<ErrorsType::SERVER_REQUEST_TIMEOUTS>();
                break;
            }

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

    RemoteActive1 &SetAddress(const Jchar *v)
    {
        this->mRemoteActive1Area.address = v;
        return (*this);
    }

    RemoteActive1 &SetPort(Jint v)
    {
        this->mRemoteActive1Area.port = v;
        return (*this);
    }

    RemoteActive1 &SetTimeouts(Jint v)
    {
        this->mRemoteActive1Area.timeouts = v;
        return (*this);
    }

private:
    POSSupport mPOSSupport;
    RemoteActive1Area mRemoteActive1Area;
    RemoteActive1Client mRemoteActive1Client;

    RemoteActive1() :
        mPOSSupport{},
        mRemoteActive1Area{},
        mRemoteActive1Client{}
    {}

    Jbool Check()
    {
        return ((this->mRemoteActive1Area.sn != nullptr)
                && (this->mRemoteActive1Area.model != nullptr)
                && (this->mRemoteActive1Area.customer != nullptr)
                && (this->mRemoteActive1Area.subCustomer != nullptr)
                && (this->mRemoteActive1Area.hardwareVersion != nullptr)
                && (this->mRemoteActive1Area.softwareVersion != nullptr)
                && (this->mRemoteActive1Area.address != nullptr));
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
        Jint times = 0;
        Jbool state = false;

        if (!this->Unlock<RemoteActive1Procedure::REGISTER>())
            return state;

        while (times < this->mRemoteActive1Area.timeouts)
        {
            state = this->Unlock<RemoteActive1Procedure::CHECK>();
            if (state)
                break;
            times += REMOTEACTIVE1_REQUEST_TIMEOUTS;
            sleep(REMOTEACTIVE1_REQUEST_TIMEOUTS);
        }

        if (state)
        {
            if (!this->SetActiveCodeBySP())
                return false;
            if (!this->Unlock<RemoteActive1Procedure::RECORD>())
                return false;
        }

        return state;
    }

    template<RemoteActive1Procedure _Procedure>
    Jbool Unlock()
    {
        Jint ret = 0;
        Jbool state = false;

        if (!this->Mashal<_Procedure>())
            return state;
        if (!this->SocketInit())
            return state;

        this->mRemoteActive1Client.http.Init()
            .SetMethod(HttpMethod::GET)
            .SetHost(this->mRemoteActive1Area.address)
            .SetTarget(this->mRemoteActive1Client.target);

        auto &&httpContent = this->mRemoteActive1Client.http.Marshal();
        if (httpContent == nullptr)
            return false;

        do
        {
            auto &&targetLen = strlen(httpContent);
            ret = send(this->mRemoteActive1Client.socket, httpContent, targetLen, 0);
            if (ret < 1)
                break;

            memset(this->mRemoteActive1Client.cache, 0, sizeof(this->mRemoteActive1Client.cache));
            ret = recv(
                this->mRemoteActive1Client.socket,
                this->mRemoteActive1Client.cache,
                sizeof(this->mRemoteActive1Client.cache),
                0
            );
            if (ret < 1)
                break;
            if (!this->mRemoteActive1Client.http.Parse(this->mRemoteActive1Client.cache, ret))
                break;

            this->mRemoteActive1Client.jsonStream.str(this->mRemoteActive1Client.http.GetBody());
            Log::Instance().Print<LogType::DEBUG>("Unlock: %s", this->mRemoteActive1Client.http.GetBody());
            state = true;
        } while (false);

        this->SocketClose();
        if (state)
            state = this->Parse<_Procedure>();

        return state;
    }

    Jbool SocketInit()
    {
        Jint i = 0;

        auto &&hostT = gethostbyname(this->mRemoteActive1Area.address);
        if (hostT == nullptr)
            return false;

        while ((*hostT).h_addr_list[i] != nullptr)
        {
            Log::Instance().Print<LogType::DEBUG>(
                "remote activation address parse: %s",
                inet_ntoa(*reinterpret_cast<in_addr *>((*hostT).h_addr_list[i]))
            );
            ++i;
        }

        if (i < 1)
            return false;

        this->mRemoteActive1Client.socket = socket(AF_INET, SOCK_STREAM, 0);
        this->mRemoteActive1Client.sockaddrIn.sin_family = AF_INET;
        this->mRemoteActive1Client.sockaddrIn.sin_port = htons(this->mRemoteActive1Area.port);
        this->mRemoteActive1Client.sockaddrIn.sin_addr = *reinterpret_cast<in_addr *>((*hostT).h_addr_list[0]);

        if (connect(
            this->mRemoteActive1Client.socket,
            reinterpret_cast<sockaddr *>(&this->mRemoteActive1Client.sockaddrIn),
            sizeof(this->mRemoteActive1Client.sockaddrIn)
        ) == -1)
            return false;

        this->mRemoteActive1Client.readCountTimes.tv_sec = REMOTEACTIVE1_RESPONSE_TIMEOUTS;
        this->mRemoteActive1Client.writeCountTimes.tv_sec = REMOTEACTIVE1_REQUEST_TIMEOUTS;

        if (setsockopt(
            this->mRemoteActive1Client.socket,
            SOL_SOCKET,
            SO_SNDTIMEO,
            &this->mRemoteActive1Client.writeCountTimes,
            sizeof(this->mRemoteActive1Client.writeCountTimes)
        ) != 0)
            return false;

        if (setsockopt(
            this->mRemoteActive1Client.socket,
            SOL_SOCKET,
            SO_RCVTIMEO,
            &this->mRemoteActive1Client.readCountTimes,
            sizeof(this->mRemoteActive1Client.readCountTimes)
        ) != 0)
            return false;

        auto &&flag = fcntl(this->mRemoteActive1Client.socket, F_GETFL, 0);
        fcntl(this->mRemoteActive1Client.socket, F_SETFL, (flag & ~O_NONBLOCK));
        Log::Instance().Print<LogType::INFO>("remote activation socket initialization successful");
        return true;
    }

    void SocketClose()
    {
        shutdown(this->mRemoteActive1Client.socket, SHUT_RDWR);
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
        Jint ret = 0;

        ret = APosSecurityManager_SysRemoteUnlockReq(
            this->mPOSSupport.context,
            REMOTEACTIVE1_APPLY_ACTIVE_CODE_BY_SP_OF_MODE,
            &this->mPOSSupport.activeCode,
            &this->mPOSSupport.activeCodeLen
        );

        if (ret == -1)
        {
            Errors::Instance().SetErrorType<ErrorsType::POS_INVALID>();
            return false;
        } else if (ret != 0)
        {
            Errors::Instance().SetErrorType<ErrorsType::POS_NEED_SHORT_SMALL_BATTERY_OR_REBOOT>();
            return false;
        }

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

    template<RemoteActive1Procedure _Procedure>
    Jbool Mashal()
    {
        Jint ret = 0;

        memset(this->mRemoteActive1Client.target, 0, sizeof(this->mRemoteActive1Client.target));
        if (_Procedure == RemoteActive1Procedure::REGISTER)
            ret = snprintf(
                this->mRemoteActive1Client.target,
                (sizeof(this->mRemoteActive1Client.target) - 1),
                REMOTEACTIVE1_TARGET_REQUEST_REGISTER,
                this->mRemoteActive1Area.sn,
                this->mRemoteActive1Area.code,
                this->mRemoteActive1Area.model,
                1,
                this->mRemoteActive1Area.customer,
                this->mRemoteActive1Area.subCustomer,
                this->mRemoteActive1Area.hardwareVersion,
                this->mRemoteActive1Area.softwareVersion
            );
        else if (_Procedure == RemoteActive1Procedure::CHECK)
            ret = snprintf(
                this->mRemoteActive1Client.target,
                (sizeof(this->mRemoteActive1Client.target) - 1),
                REMOTEACTIVE1_TARGET_REQUEST_SEARCH,
                this->mRemoteActive1Area.sn
            );
        else if (_Procedure == RemoteActive1Procedure::RECORD)
            ret = snprintf(
                this->mRemoteActive1Client.target,
                (sizeof(this->mRemoteActive1Client.target) - 1),
                REMOTEACTIVE1_TARGET_REQUEST_UPDATE_STATE,
                this->mRemoteActive1Area.sn
            );
        return ((ret > 0) && (ret < static_cast<Jint>(sizeof(this->mRemoteActive1Client.target) - 1)));
    }

    template<RemoteActive1Procedure _Procedure>
    Jbool Parse()
    {
        boost::property_tree::read_json(
            this->mRemoteActive1Client.jsonStream,
            this->mRemoteActive1Client.jsonRoot
        );

        if (_Procedure == RemoteActive1Procedure::REGISTER)
        {
            if (this->mRemoteActive1Client.jsonRoot.get_optional<bool>(
                REMOTEACTIVE1_TARGET_RESPONSE_STATE
            ).get_ptr() == nullptr)
                return false;

            return this->mRemoteActive1Client.jsonRoot.get<bool>(REMOTEACTIVE1_TARGET_RESPONSE_STATE);
        } else if (_Procedure == RemoteActive1Procedure::CHECK)
        {
            if (this->mRemoteActive1Client.jsonRoot.get_optional<bool>(
                REMOTEACTIVE1_TARGET_RESPONSE_STATE
            ).get_ptr() == nullptr)
                return false;

            if (this->mRemoteActive1Client.jsonRoot.get_optional<std::string>(
                REMOTEACTIVE1_TARGET_RESPONSE_MSG
            ).get_ptr() == nullptr)
                return false;

            auto &&code = this->mRemoteActive1Client.jsonRoot.get<std::string>(REMOTEACTIVE1_TARGET_RESPONSE_MSG);
            memset(this->mRemoteActive1Area.code, 0, sizeof(this->mRemoteActive1Area.code));
            memcpy(this->mRemoteActive1Area.code, code.c_str(), code.size());
            return this->mRemoteActive1Client.jsonRoot.get<bool>(REMOTEACTIVE1_TARGET_RESPONSE_STATE);
        } else if (_Procedure == RemoteActive1Procedure::RECORD)
        {
            if (this->mRemoteActive1Client.jsonRoot.get_optional<bool>(
                REMOTEACTIVE1_TARGET_RESPONSE_STATE
            ).get_ptr() == nullptr)
                return false;

            return this->mRemoteActive1Client.jsonRoot.get<bool>(REMOTEACTIVE1_TARGET_RESPONSE_STATE);
        }

        return false;
    }
};

}

#endif //SMARTTOOLS_SMART_ANDROID_REMOTE_ACTIVE_1_HPP
