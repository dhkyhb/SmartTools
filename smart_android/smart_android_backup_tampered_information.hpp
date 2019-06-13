#ifndef SMARTTOOLS_SMART_ANDROID_BACKUP_TAMPERED_INFORMATION_HPP
#define SMARTTOOLS_SMART_ANDROID_BACKUP_TAMPERED_INFORMATION_HPP

#include "../kernel.hpp"
#include "../smart_utils/smart_utils_simple_http.hpp"

namespace smart::android::backup::tampered::information
{

using smart::utils::simple::http::Http;
using smart::utils::simple::http::HttpMethod;

constexpr Jint BACKUP_TAMPERED_INFORMATION_BUF_SIZE = 2048;
constexpr Jint BACKUP_TAMPERED_INFORMATION_TIME_SIZE = 32;

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

    const Jchar nowTime[BACKUP_TAMPERED_INFORMATION_TIME_SIZE];
} BackupTamperedInformationAttr;

typedef struct
{
    Jint socket;
    Jchar target[BACKUP_TAMPERED_INFORMATION_BUF_SIZE];
    Jchar cache[BACKUP_TAMPERED_INFORMATION_BUF_SIZE];

    sockaddr_in sockaddrIn;
    timeval readTimeouts;
    timeval writeTimeouts;

    mbedtls_x509_crt mbedtlsX509Crt;
    mbedtls_ssl_config mbedtlsSslConfig;
    mbedtls_ssl_context mbedtlsSslContext;

    Http http;
} BackupTamperedInformationClient;

constexpr Jint BACKUP_TAMPERED_INFORMATION_REQUEST_TIMEOUTS = 10;

constexpr Jchar BACKUP_TAMPERED_INFORMATION_URL_TARGET[] =
    "/prevent_flash?operation=upload_device_ex_info"
    "&device_time=%s&customer=%s&customer_son=%s&dsn=%s&model=%s&hardware=%s&software=%s&android_version=%s"
    "&android_sdk=%s&android_id=%s&base_version=%s&kernel_version=%s&cpu_info=%s&sp_state_code=%s";

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
        if (this->Check())
            return;
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
                && (this->mBackupTamperedInformationAttr.address != nullptr));
    }

    Jbool HttpRequest()
    {
        return false;
    }

    Jbool SSLInit()
    {
        mbedtls_x509_crt_init(&this->mBackupTamperedInformationClient.mbedtlsX509Crt);
        mbedtls_ssl_config_init(&this->mBackupTamperedInformationClient.mbedtlsSslConfig);
        mbedtls_ssl_init(&this->mBackupTamperedInformationClient.mbedtlsSslContext);

        if (!this->SocketInit())
            return false;

        return false;
    }

    void SSLRelease()
    {
        mbedtls_x509_crt_free(&this->mBackupTamperedInformationClient.mbedtlsX509Crt);
        mbedtls_ssl_config_free(&this->mBackupTamperedInformationClient.mbedtlsSslConfig);
        mbedtls_ssl_free(&this->mBackupTamperedInformationClient.mbedtlsSslContext);
        this->SocketClose();
    }

    Jbool SocketInit()
    {
        Jint i = 0;

        auto &&hostT = gethostbyname(this->mBackupTamperedInformationAttr.address);
        if (hostT == nullptr)
            return false;

        while ((*hostT).h_addr_list[i] != nullptr)
        {
            Log::Instance().Print<LogType::DEBUG>(
                "backup tampered information address parse: %s",
                inet_ntoa(*reinterpret_cast<in_addr *>((*hostT).h_addr_list[i]))
            );
            ++i;
        }

        if (i < 1)
            return false;

        this->mBackupTamperedInformationClient.socket = socket(AF_INET, SOCK_STREAM, 0);
        this->mBackupTamperedInformationClient.sockaddrIn.sin_family = AF_INET;
        this->mBackupTamperedInformationClient.sockaddrIn.sin_port =
            htons(this->mBackupTamperedInformationAttr.port);
        this->mBackupTamperedInformationClient.sockaddrIn.sin_addr =
            *reinterpret_cast<in_addr *>((*hostT).h_addr_list[i]);

        if (connect(
            this->mBackupTamperedInformationClient.socket,
            reinterpret_cast<sockaddr *>(&this->mBackupTamperedInformationClient.sockaddrIn),
            sizeof(this->mBackupTamperedInformationClient.sockaddrIn)
        ) == -1)
            return false;

        this->mBackupTamperedInformationClient.readTimeouts.tv_sec = BACKUP_TAMPERED_INFORMATION_REQUEST_TIMEOUTS;
        this->mBackupTamperedInformationClient.writeTimeouts.tv_sec = BACKUP_TAMPERED_INFORMATION_REQUEST_TIMEOUTS;

        if (setsockopt(
            this->mBackupTamperedInformationClient.socket,
            SOL_SOCKET,
            SO_SNDTIMEO,
            &this->mBackupTamperedInformationClient.writeTimeouts,
            sizeof(this->mBackupTamperedInformationClient.writeTimeouts)
        ) != 0)
            return false;

        if (setsockopt(
            this->mBackupTamperedInformationClient.socket,
            SOL_SOCKET,
            SO_RCVTIMEO,
            &this->mBackupTamperedInformationClient.readTimeouts,
            sizeof(this->mBackupTamperedInformationClient.readTimeouts)
        ) != 0)
            return false;

        auto &&flag = fcntl(this->mBackupTamperedInformationClient.socket, F_GETFL, 0);
        fcntl(this->mBackupTamperedInformationClient.socket, F_SETFL, (flag & ~O_NONBLOCK));
        Log::Instance().Print<LogType::INFO>("backup tampered information socket initialization successful");
        return true;
    }

    void SocketClose()
    {
        shutdown(this->mBackupTamperedInformationClient.socket, SHUT_RDWR);
    }
};

}

#endif //SMARTTOOLS_SMART_ANDROID_BACKUP_TAMPERED_INFORMATION_HPP
