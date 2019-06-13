#ifndef SMARTTOOLS_SMART_ANDROID_REMOTE_ACTIVE_0_HPP
#define SMARTTOOLS_SMART_ANDROID_REMOTE_ACTIVE_0_HPP

#include "../kernel.hpp"
#include "../smart_utils/smart_utils_log.hpp"
#include "../smart_utils/smart_utils_math.hpp"
#include "../smart_resouces/smart_resouces_errors.hpp"
#include "../smart_android/smart_android_config.hpp"

#include <APosSecurityManager.h>

#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <linux/in.h>
#include <sys/socket.h>

namespace smart::android::remote::active0
{

using smart::utils::log::Log;
using smart::utils::log::LogType;
using smart::utils::math::Math;
using smart::resouces::errors::Errors;
using smart::resouces::errors::ErrorsType;

constexpr Jint REMOTEACTIVE0_DAM_DATA_AREA_SN_SIZE = 32;
constexpr Jint REMOTEACTIVE0_DAM_DATA_AREA_CODE_SIZE = 1024;
constexpr Jint REMOTEACTIVE0_DAM_PACKAGES_SIZE = 4096;

enum class RemoteActive0Type
{
    ARQ = 1,
    ARP = 129,
    ARR = 2,
    ARK = 130,
};

typedef struct
{
    Jint codeLen;
    Jbyte code[REMOTEACTIVE0_DAM_DATA_AREA_CODE_SIZE];
    Jint snLen;
    Jbyte sn[REMOTEACTIVE0_DAM_DATA_AREA_SN_SIZE];

    Jbyte result;
} RemoteActive0DAMDataArea;

typedef struct
{
    Jbyte packages[REMOTEACTIVE0_DAM_PACKAGES_SIZE];

    Jbyte stx;
    Jbyte lenH;
    Jbyte lenL;
    Jbyte crcH;
    Jbyte crcL;

    RemoteActive0DAMDataArea damDataArea;
} RemoteActive0DAM;

typedef struct
{
    Jint socket;

    sockaddr_in sockaddrIn;
    timeval readCountTimes;
    timeval writeCountTimes;
} RemoteActive0Client;

typedef struct
{

    Jint timeouts;
    Jint port;
    const Jchar *address;
} RemoteActive0Attr;

typedef struct
{
    Jint activeCodeLen;
    Jchar *activeCode;
    APosSecurityManager *context;

    std::mutex posMutex;
} POSSDKSupport;

constexpr Jint REMOTEACTIVE0_APPLY_ACTIVE_CODE_BY_SP_OF_MODE = 0;
constexpr Jbyte REMOTEACTIVE0_PKG_TYPE = 0xA5;

// Old version, it's based on Socket to unlock request.
class RemoteActive0
{
public:
    static RemoteActive0 &Instance()
    {
        static RemoteActive0 *obj = nullptr;
        if (obj == nullptr)
            obj = new RemoteActive0();
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
            {
                Errors::Instance().SetErrorType<ErrorsType::POS_ACTIVATED>();
                break;
            }

            if (!this->SocketInit())
            {
                Errors::Instance().SetErrorType<ErrorsType::SERVER_REQUEST_FAIL>();
                break;
            }

            if (!this->ExecuteUnlockMessage())
            {
                Errors::Instance().SetErrorType<ErrorsType::SERVER_REQUEST_TIMEOUTS>();
                break;
            }

            state = true;
            Log::Instance().Print<LogType::INFO>("remote activation is activation sussesful");
        } while (false);

        this->SocketRelease();
        this->PosRelease();
        return state;
    }

    RemoteActive0 &SetSN(const Jchar *sn)
    {
        Jint snLen = 0;

        memset(this->mRemoteActive0DAM.damDataArea.sn, 0, sizeof(this->mRemoteActive0DAM.damDataArea.sn));

        do
        {
            if (sn == nullptr)
                break;
            if (snLen = strlen(sn);snLen > static_cast<Jint>(sizeof(this->mRemoteActive0DAM.damDataArea.sn) - 1))
                break;

            this->mRemoteActive0DAM.damDataArea.snLen = snLen;
            memcpy(this->mRemoteActive0DAM.damDataArea.sn, sn, this->mRemoteActive0DAM.damDataArea.snLen);
        } while (false);

        return (*this);
    }

    RemoteActive0 &SetAddress(const Jchar *v)
    {
        this->mRemoteActive0Attr.address = v;
        return (*this);
    }

    RemoteActive0 &SetPort(Jint v)
    {
        this->mRemoteActive0Attr.port = v;
        return (*this);
    }

    RemoteActive0 &SetTimeouts(Jint v)
    {
        this->mRemoteActive0Attr.timeouts = v;
        return (*this);
    }

private:
    POSSDKSupport mPOSSDKSupport;
    RemoteActive0DAM mRemoteActive0DAM;
    RemoteActive0Client mRemoteActive0Client;
    RemoteActive0Attr mRemoteActive0Attr;

    RemoteActive0() :
        mPOSSDKSupport{},
        mRemoteActive0DAM{},
        mRemoteActive0Client{},
        mRemoteActive0Attr{}
    {}

    Jbool Check()
    {
        return (this->mRemoteActive0Attr.address != nullptr);
    }

    Jbool POSInit()
    {
        this->mPOSSDKSupport.posMutex.lock();
        memset(&this->mPOSSDKSupport, 0, sizeof(this->mPOSSDKSupport));
        this->mPOSSDKSupport.context = APosSecurityManager_new();
        return (this->mPOSSDKSupport.context != nullptr);
    }

    void PosRelease()
    {
        if (this->mPOSSDKSupport.activeCode != nullptr)
            free(this->mPOSSDKSupport.activeCode);
        if (this->mPOSSDKSupport.context != nullptr)
            APosSecurityManager_delete(this->mPOSSDKSupport.context);

        this->mPOSSDKSupport.posMutex.unlock();
    }

    Jbool SocketInit()
    {
        Jint i = 0;

        auto &&hostT = gethostbyname(this->mRemoteActive0Attr.address);
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

        this->mRemoteActive0Client.socket = socket(AF_INET, SOCK_STREAM, 0);
        this->mRemoteActive0Client.sockaddrIn.sin_family = AF_INET;
        this->mRemoteActive0Client.sockaddrIn.sin_port = htons(this->mRemoteActive0Attr.port);
        this->mRemoteActive0Client.sockaddrIn.sin_addr = *reinterpret_cast<in_addr *>((*hostT).h_addr_list[0]);

        if (connect(
            this->mRemoteActive0Client.socket,
            reinterpret_cast<sockaddr *>(&this->mRemoteActive0Client.sockaddrIn),
            sizeof(this->mRemoteActive0Client.sockaddrIn)
        ) == -1)
            return false;

        this->mRemoteActive0Client.readCountTimes.tv_sec = this->mRemoteActive0Attr.timeouts;
        this->mRemoteActive0Client.writeCountTimes.tv_sec = this->mRemoteActive0Attr.timeouts;

        if (setsockopt(
            this->mRemoteActive0Client.socket,
            SOL_SOCKET,
            SO_SNDTIMEO,
            &this->mRemoteActive0Client.writeCountTimes,
            sizeof(this->mRemoteActive0Client.writeCountTimes)
        ) != 0)
            return false;

        if (setsockopt(
            this->mRemoteActive0Client.socket,
            SOL_SOCKET,
            SO_RCVTIMEO,
            &this->mRemoteActive0Client.readCountTimes,
            sizeof(this->mRemoteActive0Client.readCountTimes)
        ) != 0)
            return false;

        auto &&flag = fcntl(this->mRemoteActive0Client.socket, F_GETFL, 0);
        fcntl(this->mRemoteActive0Client.socket, F_SETFL, (flag & ~O_NONBLOCK));
        Log::Instance().Print<LogType::INFO>("remote activation socket initialization successful");
        return true;
    }

    void SocketRelease()
    {
        shutdown(this->mRemoteActive0Client.socket, SHUT_RDWR);
    }

    Jbool ExecuteUnlockMessage()
    {
        Jint ret = 0;

        if (ret = this->Mashal<RemoteActive0Type::ARQ, false>();ret == 0)
            return false;
        if (ret = send(this->mRemoteActive0Client.socket, this->mRemoteActive0DAM.packages, ret, 0);ret < 1)
            return false;

        Log::Instance().Print<LogType::DEBUG>("send to %d", ret);
        if (ret = recv(
                this->mRemoteActive0Client.socket,
                this->mRemoteActive0DAM.packages,
                sizeof(this->mRemoteActive0DAM.packages),
                0
            );ret < 1)
            return false;

        Log::Instance().Print<LogType::DEBUG>("recv to %d", ret);
        if (!this->Parse<RemoteActive0Type::ARP>())
            return false;

        if (this->SetActiveCodeBySP())
        {
            if (ret = this->Mashal<RemoteActive0Type::ARR, true>();ret == 0)
                return false;
        } else
        {
            if (ret = this->Mashal<RemoteActive0Type::ARR, false>();ret == 0)
                return false;
        }

        ret = send(this->mRemoteActive0Client.socket, this->mRemoteActive0DAM.packages, ret, 0);
        return (ret > 0);
    }

    Jbool SetActiveCodeBySP()
    {
        return (APosSecurityManager_SysRemoteUnlockRsp(
            this->mPOSSDKSupport.context,
            REMOTEACTIVE0_APPLY_ACTIVE_CODE_BY_SP_OF_MODE,
            reinterpret_cast<Jchar *>(this->mRemoteActive0DAM.damDataArea.code),
            this->mRemoteActive0DAM.damDataArea.codeLen
        ) == 0);
    }

    Jbool GetActiveCodeBySP()
    {
        Jint ret = 0;

        if (ret = APosSecurityManager_SysRemoteUnlockReq(
                this->mPOSSDKSupport.context,
                REMOTEACTIVE0_APPLY_ACTIVE_CODE_BY_SP_OF_MODE,
                &this->mPOSSDKSupport.activeCode,
                &this->mPOSSDKSupport.activeCodeLen
            );ret != 0)
            return false;

        Log::Instance().Print<LogType::DEBUG>("apply activation code ret: %d", ret);
        if (this->mPOSSDKSupport.activeCode == nullptr)
            return false;
        if (this->mPOSSDKSupport.activeCodeLen > static_cast<Jint>(
            sizeof(this->mRemoteActive0DAM.damDataArea.code) - 1
        ))
            return false;

        this->mRemoteActive0DAM.damDataArea.codeLen = (this->mPOSSDKSupport.activeCodeLen -= 3);
        memcpy(
            this->mRemoteActive0DAM.damDataArea.code,
            &this->mPOSSDKSupport.activeCode[3],
            this->mRemoteActive0DAM.damDataArea.codeLen
        );

        Log::Instance().Print<LogType::DEBUG>("this is got active code by the SP");
        Log::Instance().PrintHex(
            this->mRemoteActive0DAM.damDataArea.code,
            this->mRemoteActive0DAM.damDataArea.codeLen
        );
        return true;
    }

    template<RemoteActive0Type _type>
    Jbool Parse()
    {
        Jint step = 0;

        if (this->mRemoteActive0DAM.stx = this->mRemoteActive0DAM.packages[step];
            this->mRemoteActive0DAM.stx != REMOTEACTIVE0_PKG_TYPE)
            return false;

        this->mRemoteActive0DAM.lenH = this->mRemoteActive0DAM.packages[++step];
        this->mRemoteActive0DAM.lenL = this->mRemoteActive0DAM.packages[++step];

        if ((_type == RemoteActive0Type::ARQ) || (_type == RemoteActive0Type::ARP))
        {
            if (static_cast<RemoteActive0Type>(this->mRemoteActive0DAM.packages[++step]) != RemoteActive0Type::ARP)
                return false;
        } else if ((_type == RemoteActive0Type::ARR) || (_type == RemoteActive0Type::ARK))
        {
            if (static_cast<RemoteActive0Type>(this->mRemoteActive0DAM.packages[++step]) != RemoteActive0Type::ARK)
                return false;
        } else
        {
            return false;
        }

        this->mRemoteActive0DAM.damDataArea.snLen = this->mRemoteActive0DAM.packages[++step];
        if (this->mRemoteActive0DAM.damDataArea.snLen > static_cast<Jint>(
            sizeof(this->mRemoteActive0DAM.damDataArea.sn) - 1
        ))
            return false;

        memcpy(
            this->mRemoteActive0DAM.damDataArea.sn,
            &this->mRemoteActive0DAM.packages[++step],
            this->mRemoteActive0DAM.damDataArea.snLen
        );

        if ((_type == RemoteActive0Type::ARQ) || (_type == RemoteActive0Type::ARP))
        {
            auto &&aCodeSpecialLen =
                this->mRemoteActive0DAM.packages[step += this->mRemoteActive0DAM.damDataArea.snLen];
            this->mRemoteActive0DAM.damDataArea.codeLen = aCodeSpecialLen == 0 ? 256 : aCodeSpecialLen;
            if (this->mRemoteActive0DAM.damDataArea.codeLen > static_cast<Jint>(
                sizeof(this->mRemoteActive0DAM.damDataArea.code)
            ))
                return false;

            memcpy(
                this->mRemoteActive0DAM.damDataArea.code,
                &this->mRemoteActive0DAM.packages[++step],
                this->mRemoteActive0DAM.damDataArea.codeLen
            );

            step += this->mRemoteActive0DAM.damDataArea.codeLen;
        } else if ((_type == RemoteActive0Type::ARR) || (_type == RemoteActive0Type::ARK))
        {
            this->mRemoteActive0DAM.damDataArea.result =
                this->mRemoteActive0DAM.packages[step += this->mRemoteActive0DAM.damDataArea.snLen];
            ++step;
        }

        this->mRemoteActive0DAM.crcH = this->mRemoteActive0DAM.packages[step];
        this->mRemoteActive0DAM.crcL = this->mRemoteActive0DAM.packages[++step];

        Log::Instance().Print<LogType::DEBUG>("parse sn: %s", this->mRemoteActive0DAM.damDataArea.sn);
        if ((_type == RemoteActive0Type::ARQ) || (_type == RemoteActive0Type::ARP))
        {
            Log::Instance().Print<LogType::DEBUG>("parse active code");
            Log::Instance().PrintHex(
                this->mRemoteActive0DAM.damDataArea.code, this->mRemoteActive0DAM.damDataArea.codeLen
            );
        } else if ((_type == RemoteActive0Type::ARR) || (_type == RemoteActive0Type::ARK))
        {
            Log::Instance().Print<LogType::DEBUG>(
                "parse active state: %02x",
                this->mRemoteActive0DAM.damDataArea.result
            );
        }
        return true;
    }

    template<RemoteActive0Type _type, Jbool isSuccessfulActivation>
    Jint Mashal()
    {
        Jint step = 0;

        this->mRemoteActive0DAM.stx = REMOTEACTIVE0_PKG_TYPE;
        auto &&dataAreaLen = this->mRemoteActive0DAM.damDataArea.snLen
                             + this->mRemoteActive0DAM.damDataArea.codeLen
                             + 3;

        this->mRemoteActive0DAM.lenH = static_cast<Jbyte>(dataAreaLen >> 8);
        this->mRemoteActive0DAM.lenL = static_cast<Jbyte>(dataAreaLen);

        this->mRemoteActive0DAM.packages[step] = this->mRemoteActive0DAM.lenH;
        this->mRemoteActive0DAM.packages[++step] = this->mRemoteActive0DAM.lenL;
        this->mRemoteActive0DAM.packages[++step] = static_cast<Jbyte>(_type);

        this->mRemoteActive0DAM.packages[++step] = this->mRemoteActive0DAM.damDataArea.snLen;

        memcpy(
            &this->mRemoteActive0DAM.packages[++step],
            this->mRemoteActive0DAM.damDataArea.sn,
            this->mRemoteActive0DAM.damDataArea.snLen
        );

        if (_type == RemoteActive0Type::ARQ)
        {
            this->mRemoteActive0DAM.packages[step += this->mRemoteActive0DAM.damDataArea.snLen] =
                this->mRemoteActive0DAM.damDataArea.codeLen;

            memcpy(
                &this->mRemoteActive0DAM.packages[++step],
                this->mRemoteActive0DAM.damDataArea.code,
                this->mRemoteActive0DAM.damDataArea.codeLen
            );

            step += this->mRemoteActive0DAM.damDataArea.codeLen;
        } else if (_type == RemoteActive0Type::ARR)
        {
            this->mRemoteActive0DAM.packages[step += this->mRemoteActive0DAM.damDataArea.snLen] =
                isSuccessfulActivation;

            ++step;
        }

        auto &&crcV = Math::CRC16(this->mRemoteActive0DAM.packages, step, 0);
        this->mRemoteActive0DAM.crcH = static_cast<Jbyte>(crcV >> 8);
        this->mRemoteActive0DAM.crcL = static_cast<Jbyte>(crcV);

        step = 0;
        this->mRemoteActive0DAM.packages[step] = this->mRemoteActive0DAM.stx;
        this->mRemoteActive0DAM.packages[++step] = this->mRemoteActive0DAM.lenH;
        this->mRemoteActive0DAM.packages[++step] = this->mRemoteActive0DAM.lenL;
        this->mRemoteActive0DAM.packages[++step] = static_cast<Jbyte>(_type);

        this->mRemoteActive0DAM.packages[++step] = this->mRemoteActive0DAM.damDataArea.snLen;
        memcpy(
            &this->mRemoteActive0DAM.packages[++step],
            this->mRemoteActive0DAM.damDataArea.sn,
            this->mRemoteActive0DAM.damDataArea.snLen
        );

        if (_type == RemoteActive0Type::ARQ)
        {
            this->mRemoteActive0DAM.packages[step += this->mRemoteActive0DAM.damDataArea.snLen] =
                this->mRemoteActive0DAM.damDataArea.codeLen;

            memcpy(
                &this->mRemoteActive0DAM.packages[++step],
                this->mRemoteActive0DAM.damDataArea.code,
                this->mRemoteActive0DAM.damDataArea.codeLen
            );

            step += this->mRemoteActive0DAM.damDataArea.codeLen;
        } else if (_type == RemoteActive0Type::ARR)
        {
            this->mRemoteActive0DAM.packages[step += this->mRemoteActive0DAM.damDataArea.snLen] =
                isSuccessfulActivation;

            ++step;
        }

        this->mRemoteActive0DAM.packages[step] = this->mRemoteActive0DAM.crcH;
        this->mRemoteActive0DAM.packages[++step] = this->mRemoteActive0DAM.crcL;
        ++step;
        Log::Instance().Print<LogType::DEBUG>("active marshal packages");
        Log::Instance().PrintHex(this->mRemoteActive0DAM.packages, step);
        return step;
    }
};

}

#endif //SMARTTOOLS_SMART_ANDROID_REMOTE_ACTIVE_0_HPP
