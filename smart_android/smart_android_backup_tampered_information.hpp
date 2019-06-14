#ifndef SMARTTOOLS_SMART_ANDROID_BACKUP_TAMPERED_INFORMATION_HPP
#define SMARTTOOLS_SMART_ANDROID_BACKUP_TAMPERED_INFORMATION_HPP

#include "../kernel.hpp"
#include "../smart_utils/smart_utils_strings.hpp"
#include "../smart_utils/smart_utils_simple_http.hpp"

namespace smart::android::backup::tampered::information
{

using smart::utils::strings::Strings;
using smart::utils::simple::http::Http;
using smart::utils::simple::http::HttpMethod;

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

            if (ret = mbedtls_ssl_write(
                    &this->mBackupTamperedInformationClient.mbedtlsSslContext,
                    reinterpret_cast<Jbyte *>(const_cast<Jchar *>(url)),
                    strlen(url)
                );ret < 1)
                break;

            memset(
                this->mBackupTamperedInformationClient.cache,
                0,
                sizeof(this->mBackupTamperedInformationClient.cache)
            );

            if (ret = mbedtls_ssl_read(
                    &this->mBackupTamperedInformationClient.mbedtlsSslContext,
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
        Jint ret = 0;
        Jbool state = true;

        mbedtls_x509_crt_init(&this->mBackupTamperedInformationClient.mbedtlsX509Crt);
        mbedtls_ssl_config_init(&this->mBackupTamperedInformationClient.mbedtlsSslConfig);
        mbedtls_ssl_init(&this->mBackupTamperedInformationClient.mbedtlsSslContext);

        if (!this->SocketInit())
            return false;

        if (ret = mbedtls_x509_crt_parse(
                &this->mBackupTamperedInformationClient.mbedtlsX509Crt,
                reinterpret_cast<Jbyte *>(const_cast<Jchar *>(this->mBackupTamperedInformationAttr.cert)),
                (strlen(this->mBackupTamperedInformationAttr.cert) + 1)
            );ret < 0)
        {
            Log::Instance().Print<LogType::DEBUG>("ssl cert parse fail, ret: %d", ret);
            return false;
        }

        if (ret = mbedtls_ssl_config_defaults(
                &this->mBackupTamperedInformationClient.mbedtlsSslConfig,
                MBEDTLS_SSL_IS_CLIENT,
                MBEDTLS_SSL_TRANSPORT_STREAM,
                MBEDTLS_SSL_PRESET_DEFAULT
            );ret != 0)
        {
            Log::Instance().Print<LogType::DEBUG>("ssl config fail, ret: %d", ret);
            return false;
        }

        mbedtls_ssl_conf_authmode(
            &this->mBackupTamperedInformationClient.mbedtlsSslConfig,
            MBEDTLS_SSL_VERIFY_OPTIONAL
        );
        mbedtls_ssl_conf_ca_chain(
            &this->mBackupTamperedInformationClient.mbedtlsSslConfig,
            &this->mBackupTamperedInformationClient.mbedtlsX509Crt,
            nullptr
        );

        mbedtls_ssl_conf_rng(
            &this->mBackupTamperedInformationClient.mbedtlsSslConfig,
            [](void *pRng, Jbyte *output, Jsize outputLen) -> Jint {
                Jint rnglen = outputLen;
                Jbyte rngoffset = 0;

                while (rnglen > 0)
                {
                    *(&output[rngoffset]) = static_cast<Jbyte>(
                        (static_cast<Juint>(rand()) << static_cast<Juint>(16)) + rand()
                    );
                    rngoffset++;
                    rnglen--;
                }

                return 0;
            },
            nullptr
        );

        if (ret = mbedtls_ssl_setup(
                &this->mBackupTamperedInformationClient.mbedtlsSslContext,
                &this->mBackupTamperedInformationClient.mbedtlsSslConfig
            );ret != 0)
        {
            Log::Instance().Print<LogType::ERROR>("step config to ssl context is fail, ret: %d", ret);
            return false;
        }

        if (ret = mbedtls_ssl_set_hostname(
                &this->mBackupTamperedInformationClient.mbedtlsSslContext,
                this->mBackupTamperedInformationAttr.address
            );ret != 0)
        {
            Log::Instance().Print<LogType::ERROR>("ssl set hostname fail, ret: %d", ret);
            return false;
        }

        mbedtls_ssl_set_bio(
            &this->mBackupTamperedInformationClient.mbedtlsSslContext,
            &this->mBackupTamperedInformationClient,
            [](void *ctx, const Jbyte *buf, Jsize len) -> Jint {
                auto &&client = *reinterpret_cast<BackupTamperedInformationClient *>(ctx);
                return send(client.socket, buf, len, 0);
            },
            [](void *ctx, Jbyte *buf, Jsize len) -> Jint {
                auto &&client = *reinterpret_cast<BackupTamperedInformationClient *>(ctx);
                return recv(client.socket, buf, len, 0);
            },
            [](void *ctx, Jbyte *buf, Jsize len, Juint timeouts) -> Jint {
                auto &&client = *reinterpret_cast<BackupTamperedInformationClient *>(ctx);
                return recv(client.socket, buf, len, 0);
            }
        );

        while ((ret = mbedtls_ssl_handshake(&this->mBackupTamperedInformationClient.mbedtlsSslContext)) != 0)
        {
            if ((ret == MBEDTLS_ERR_SSL_WANT_READ) || (ret == MBEDTLS_ERR_SSL_WANT_WRITE))
                continue;
            if (ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED)
            {
                state = false;
                break;
            }
        }

        return state;
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
            *reinterpret_cast<in_addr *>((*hostT).h_addr_list[0]);

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
