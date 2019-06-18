#pragma once

#ifndef SMARTTOOLS_SMART_ANDROID_BACKUP_TAMPERED_INFORMATION_HPP
#define SMARTTOOLS_SMART_ANDROID_BACKUP_TAMPERED_INFORMATION_HPP

#include "../kernel.hpp"
#include "../smart_utils/smart_utils_strings.hpp"
#include "../smart_utils/smart_utils_simple_http.hpp"
#include "../smart_utils/smart_utils_socket_based_on_linux.hpp"
#include "../smart_utils/smart_utils_socket_ssl_based_on_mbedtls.hpp"

namespace smart::android::backup::tampered::information
{

using smart::utils::strings::Strings;
using smart::utils::simple::http::Http;
using smart::utils::simple::http::HttpMethod;
using smart::utils::soket::linux::Socket;
using smart::utils::soket::ssl::based::on::mbedlts::SocketSSL;

constexpr Jint BACKUP_TAMPERED_INFORMATION_BUF_SIZE = 2048;
constexpr Jint BACKUP_TAMPERED_INFORMATION_TIME_SIZE = 32;
constexpr Jint BACKUP_TAMPERED_INFORMATION_SENSOR_STR_SIZE = 64;
constexpr Jint BACKUP_TAMPERED_INFORMATION_HOSTNAME_SIZE = 24;
constexpr Jint BACKUP_TAMPERED_INFORMATION_NEW_SP_VERSION_BUF = 64;

typedef struct
{
    const Jchar *sn;
    const Jchar *model;
    const Jchar *customer;
    const Jchar *subCustomer;
    const Jchar *hwVersion;
    const Jchar *swVersion;
    const Jchar *androidVersion;
    const Jchar *androidSDKVersion;
    const Jchar *androidID;
    const Jchar *spVersion;
    const Jchar *androidDevice;
    const Jchar *androidBootloader;

    Jint port;
    const Jchar *address;
    const Jchar *cert;

    Jchar newSPVersion[BACKUP_TAMPERED_INFORMATION_NEW_SP_VERSION_BUF];
    Jchar sensor[BACKUP_TAMPERED_INFORMATION_SENSOR_STR_SIZE];
    Jchar nowTime[BACKUP_TAMPERED_INFORMATION_TIME_SIZE];
    tm *nowTimeSource;
} BackupTamperedInformationAttr;

typedef struct
{
    Jint socket;
    Jchar hostName[BACKUP_TAMPERED_INFORMATION_HOSTNAME_SIZE];
    Jchar target[BACKUP_TAMPERED_INFORMATION_BUF_SIZE];
    Jchar cache[BACKUP_TAMPERED_INFORMATION_BUF_SIZE];

    Http http;
    Socket socketObj;
    SocketSSL socketSSL;
} BackupTamperedInformationClient;

constexpr Jint BACKUP_TAMPERED_INFORMATION_REQUEST_TIMEOUTS = 10;

constexpr Jchar BACKUP_TAMPERED_INFORMATION_URL_TARGET[] =
    "/prevent_flash?operation=upload_device_ex_info"
    "&device_time=%s&customer=%s&customer_son=%s&dsn=%s&model=%s&hardware=%s&software=%s&android_version=%s"
    "&android_sdk=%s&android_id=%s&base_version=%s&kernel_version=%s&cpu_info=%s&sp_state_code=%s";

constexpr Jchar BACKUP_TAMPERED_INFORMATION_TIMES_FORMAT[] = "%04d-%02d-%02d+%02d:%02d:%02d";
constexpr Jchar BACKUP_TAMPERED_INFORMATION_HOSTNAME_FORMAT[] = "%s:%d";

class BackupTamperedInformation
{
public:
    static BackupTamperedInformation &Instance()
    {
        static BackupTamperedInformation *obj = nullptr;
        if (obj == nullptr)
            obj = new BackupTamperedInformation();
        return (*obj);
    }

    void UploadTamperInformation()
    {
        if (!this->Check())
            return;
        this->HttpRequest();
    }

    BackupTamperedInformation &SetSN(const Jchar *v)
    {
        this->mBackupTamperedInformationAttr.sn = v;
        return (*this);
    }

    BackupTamperedInformation &SetModel(const Jchar *v)
    {
        this->mBackupTamperedInformationAttr.model = v;
        return (*this);
    }

    BackupTamperedInformation &SetCustomer(const Jchar *v)
    {
        this->mBackupTamperedInformationAttr.customer = v;
        return (*this);
    }

    BackupTamperedInformation &SetSubCustomer(const Jchar *v)
    {
        this->mBackupTamperedInformationAttr.subCustomer = v;
        return (*this);
    }

    BackupTamperedInformation &SetHardwareVersion(const Jchar *v)
    {
        this->mBackupTamperedInformationAttr.hwVersion = v;
        return (*this);
    }

    BackupTamperedInformation &SetSoftwareVersion(const Jchar *v)
    {
        this->mBackupTamperedInformationAttr.swVersion = v;
        return (*this);
    }

    BackupTamperedInformation &SetAndroidVersion(const Jchar *v)
    {
        this->mBackupTamperedInformationAttr.androidVersion = v;
        return (*this);
    }

    BackupTamperedInformation &SetAndroidSDKVersion(const Jchar *v)
    {
        this->mBackupTamperedInformationAttr.androidSDKVersion = v;
        return (*this);
    }

    BackupTamperedInformation &SetAndroidID(const Jchar *v)
    {
        this->mBackupTamperedInformationAttr.androidID = v;
        return (*this);
    }

    BackupTamperedInformation &SetSPVersion(const Jchar *v)
    {
        this->mBackupTamperedInformationAttr.spVersion = v;
        return (*this);
    }

    BackupTamperedInformation &SetAndroidDevice(const Jchar *v)
    {
        this->mBackupTamperedInformationAttr.androidDevice = v;
        return (*this);
    }

    BackupTamperedInformation &SetAndroidBootloader(const Jchar *v)
    {
        this->mBackupTamperedInformationAttr.androidBootloader = v;
        return (*this);
    }

    BackupTamperedInformation &SetAddress(const Jchar *v)
    {
        this->mBackupTamperedInformationAttr.address = v;
        return (*this);
    }

    BackupTamperedInformation &SetPort(Jint v)
    {
        this->mBackupTamperedInformationAttr.port = v;
        return (*this);
    }

    BackupTamperedInformation &SetCert(const Jchar *v)
    {
        this->mBackupTamperedInformationAttr.cert = v;
        return (*this);
    }

private:
    BackupTamperedInformationAttr mBackupTamperedInformationAttr;
    BackupTamperedInformationClient mBackupTamperedInformationClient;

    BackupTamperedInformation() :
        mBackupTamperedInformationAttr{},
        mBackupTamperedInformationClient{}
    {}

    Jbool Check()
    {
        return ((this->mBackupTamperedInformationAttr.sn != nullptr)
                && (this->mBackupTamperedInformationAttr.model != nullptr)
                && (this->mBackupTamperedInformationAttr.customer != nullptr)
                && (this->mBackupTamperedInformationAttr.subCustomer != nullptr)
                && (this->mBackupTamperedInformationAttr.hwVersion != nullptr)
                && (this->mBackupTamperedInformationAttr.swVersion != nullptr)
                && (this->mBackupTamperedInformationAttr.androidVersion != nullptr)
                && (this->mBackupTamperedInformationAttr.androidSDKVersion != nullptr)
                && (this->mBackupTamperedInformationAttr.androidID != nullptr)
                && (this->mBackupTamperedInformationAttr.spVersion != nullptr)
                && (this->mBackupTamperedInformationAttr.androidDevice != nullptr)
                && (this->mBackupTamperedInformationAttr.androidBootloader != nullptr)
                && (this->mBackupTamperedInformationAttr.address != nullptr)
                && (this->mBackupTamperedInformationAttr.cert != nullptr));
    }

    void HttpRequest()
    {
        Jint ret = 0;

        if (!this->Marshal())
            return;

        memset(
            this->mBackupTamperedInformationClient.hostName,
            0,
            sizeof(this->mBackupTamperedInformationClient.hostName)
        );
        if (snprintf(
            this->mBackupTamperedInformationClient.hostName,
            (sizeof(this->mBackupTamperedInformationClient.hostName) - 1),
            BACKUP_TAMPERED_INFORMATION_HOSTNAME_FORMAT,
            this->mBackupTamperedInformationAttr.address,
            this->mBackupTamperedInformationAttr.port
        ) < 1)
            return;

        auto &&url = this->mBackupTamperedInformationClient.http.Init()
            .SetMethod(HttpMethod::GET)
            .SetHost(this->mBackupTamperedInformationClient.hostName)
            .SetTarget(this->mBackupTamperedInformationClient.target)
            .Marshal();
        if (url == nullptr)
            return;

        do
        {
            if (!this->SSLInit())
                break;

            if (ret = this->mBackupTamperedInformationClient.socketSSL.Write(
                    reinterpret_cast<Jbyte *>(const_cast<Jchar *>(url)),
                    strlen(url)
                );ret < 1)
                break;

            memset(
                this->mBackupTamperedInformationClient.cache,
                0,
                sizeof(this->mBackupTamperedInformationClient.cache)
            );

            if (ret = this->mBackupTamperedInformationClient.socketSSL.Read(
                    reinterpret_cast<Jbyte *>(this->mBackupTamperedInformationClient.cache),
                    sizeof(this->mBackupTamperedInformationClient.cache)
                );ret < 1)
                break;

            if (!this->mBackupTamperedInformationClient.http.Parse(this->mBackupTamperedInformationClient.cache, ret))
                return;

            Log::Instance().Print<LogType::DEBUG>("recv: %s", this->mBackupTamperedInformationClient.http.GetBody());
        } while (false);

        this->SSLRelease();
    }

    Jbool SSLInit()
    {
        if (!this->SocketInit())
            return false;

        this->mBackupTamperedInformationClient.socket = this->mBackupTamperedInformationClient.socketObj.GetSocket();
        return this->mBackupTamperedInformationClient.socketSSL.SetCert(this->mBackupTamperedInformationAttr.cert)
            .SetHostName(this->mBackupTamperedInformationAttr.address)
            .SetSocketID(this->mBackupTamperedInformationClient.socket)
            .Connect();
    }

    void SSLRelease()
    {
        this->mBackupTamperedInformationClient.socketSSL.DisConnect();
        this->SocketClose();
    }

    Jbool SocketInit()
    {
        return this->mBackupTamperedInformationClient.socketObj.SetAddress(this->mBackupTamperedInformationAttr.address)
            .SetPort(this->mBackupTamperedInformationAttr.port)
            .SetWriteTimeoutsSecounds(BACKUP_TAMPERED_INFORMATION_REQUEST_TIMEOUTS)
            .SetReadTimeoutsSecounds(BACKUP_TAMPERED_INFORMATION_REQUEST_TIMEOUTS)
            .Connect();
    }

    void SocketClose()
    {
        this->mBackupTamperedInformationClient.socketObj.DisConnect();
    }

    Jbool Marshal()
    {
        if (!this->FormatNowTime())
            return false;
        if (!this->Sensor2Str())
            return false;

        this->SPVersionURLCoding();
        memset(this->mBackupTamperedInformationClient.target, 0, sizeof(this->mBackupTamperedInformationClient.target));
        return (snprintf(
            this->mBackupTamperedInformationClient.target,
            (sizeof(this->mBackupTamperedInformationClient.target) - 1),
            BACKUP_TAMPERED_INFORMATION_URL_TARGET,
            this->mBackupTamperedInformationAttr.nowTime,
            this->mBackupTamperedInformationAttr.customer,
            this->mBackupTamperedInformationAttr.subCustomer,
            this->mBackupTamperedInformationAttr.sn,
            this->mBackupTamperedInformationAttr.model,
            this->mBackupTamperedInformationAttr.hwVersion,
            this->mBackupTamperedInformationAttr.swVersion,
            this->mBackupTamperedInformationAttr.androidVersion,
            this->mBackupTamperedInformationAttr.androidSDKVersion,
            this->mBackupTamperedInformationAttr.androidID,
            this->mBackupTamperedInformationAttr.newSPVersion,
            this->mBackupTamperedInformationAttr.androidDevice,
            this->mBackupTamperedInformationAttr.androidBootloader,
            this->mBackupTamperedInformationAttr.sensor
        ) > 0);
    }

    void SPVersionURLCoding()
    {
        constexpr Jchar SP_VERSION_URL_CODING_SPACE = ' ';
        constexpr Jchar SP_VERSION_URL_CODING_EQUAL_SIGN = '=';
        constexpr Jchar SP_VERSION_URL_CODING_SPACE_2_ADD = '+';
        constexpr Jchar SP_VERSION_URL_CODING_EQUAL_SIGN_2_3D[] = "%3D";

        Jint i = 0;
        Jint step = 0;

        for (i = 0; i < static_cast<Jint>(strlen(this->mBackupTamperedInformationAttr.spVersion)); ++i)
        {
            if (this->mBackupTamperedInformationAttr.spVersion[i] == SP_VERSION_URL_CODING_SPACE)
            {
                this->mBackupTamperedInformationAttr.newSPVersion[step] = SP_VERSION_URL_CODING_SPACE_2_ADD;
                ++step;
            } else if (this->mBackupTamperedInformationAttr.spVersion[i] == SP_VERSION_URL_CODING_EQUAL_SIGN)
            {
                auto &&len = strlen(SP_VERSION_URL_CODING_EQUAL_SIGN_2_3D);

                memcpy(
                    &this->mBackupTamperedInformationAttr.newSPVersion[step],
                    SP_VERSION_URL_CODING_EQUAL_SIGN_2_3D,
                    len
                );

                step += len;
            } else
            {
                this->mBackupTamperedInformationAttr.newSPVersion[step] =
                    this->mBackupTamperedInformationAttr.spVersion[i];
                ++step;
            }
        }
    }

    Jbool Sensor2Str()
    {
        memset(this->mBackupTamperedInformationAttr.sensor, 0, sizeof(this->mBackupTamperedInformationAttr.sensor));
        return Strings::Bytes2String(
            Sensor::Instance().GetSourceSensorValue(),
            Sensor::Instance().GetSourceSensorValueLength(),
            this->mBackupTamperedInformationAttr.sensor,
            sizeof(this->mBackupTamperedInformationAttr.sensor)
        );
    }

    Jbool FormatNowTime()
    {
        time_t nowTime = 0;

        nowTime = time(nullptr);
        this->mBackupTamperedInformationAttr.nowTimeSource = localtime(&nowTime);
        this->mBackupTamperedInformationAttr.nowTimeSource->tm_year += 1900;
        ++this->mBackupTamperedInformationAttr.nowTimeSource->tm_mon;

        memset(this->mBackupTamperedInformationAttr.nowTime, 0, sizeof(this->mBackupTamperedInformationAttr.nowTime));
        return (snprintf(
            this->mBackupTamperedInformationAttr.nowTime,
            (sizeof(this->mBackupTamperedInformationAttr.nowTime) - 1),
            BACKUP_TAMPERED_INFORMATION_TIMES_FORMAT,
            this->mBackupTamperedInformationAttr.nowTimeSource->tm_year,
            this->mBackupTamperedInformationAttr.nowTimeSource->tm_mon,
            this->mBackupTamperedInformationAttr.nowTimeSource->tm_mday,
            this->mBackupTamperedInformationAttr.nowTimeSource->tm_hour,
            this->mBackupTamperedInformationAttr.nowTimeSource->tm_min,
            this->mBackupTamperedInformationAttr.nowTimeSource->tm_sec
        ) > 0);
    }

};

}

#endif //SMARTTOOLS_SMART_ANDROID_BACKUP_TAMPERED_INFORMATION_HPP
