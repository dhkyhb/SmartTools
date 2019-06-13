#ifndef SMARTTOOLS_SMART_ANDROID_CUSTOMER_CHECK_UNLOCK_HPP
#define SMARTTOOLS_SMART_ANDROID_CUSTOMER_CHECK_UNLOCK_HPP

#include "../kernel.hpp"
#include "../smart_utils/smart_utils_simple_http.hpp"
#include "../smart_resouces/smart_resouces_errors.hpp"

#include <mbedtls/ssl.h>
#include <boost/property_tree/json_parser.hpp>

namespace smart::android::customer::check::unlock
{

using smart::utils::simple::http::Http;
using smart::utils::simple::http::HttpMethod;
using smart::resouces::errors::Errors;
using smart::resouces::errors::ErrorsType;

constexpr Jint CUSTOMER_CHECK_UNLOCK_HOSTNAME_SIZE = 256;
constexpr Jint CUSTOMER_CHECK_UNLOCK_BUF_SIZE = 2048;

enum class CustomerCheckUnlockProcedure
{
    APPLY_UNLOCK, UNLOCK,
};

typedef struct
{
    const Jchar *sn;
    const Jchar *model;
    const Jchar *customer;
    const Jchar *subCustomer;
    const Jchar *oldCustomer;
    const Jchar *oldSubCustomer;
    const Jchar *hardwareVersion;
    const Jchar *softwareVersion;

    Jint timeouts;
    Jint port;
    const Jchar *address;
    const Jchar *clientCert;

    Jchar hostName[CUSTOMER_CHECK_UNLOCK_HOSTNAME_SIZE];
} CustomerCheckUnlockAttr;

typedef struct CustomerCheckUnlockClient
{
    Jint socket;
    Jchar target[CUSTOMER_CHECK_UNLOCK_BUF_SIZE];
    Jchar cache[CUSTOMER_CHECK_UNLOCK_BUF_SIZE];

    sockaddr_in sockaddrIn;
    timeval readTimeouts;
    timeval writeTimeouts;

    mbedtls_x509_crt mbedtlsX509Crt;
    mbedtls_ssl_config mbedtlsSslConfig;
    mbedtls_ssl_context mbedtlsSslContext;

    std::stringstream jsonStream;
    boost::property_tree::ptree jsonRoot;

    Http http;

    CustomerCheckUnlockClient() :
        socket{},
        target{},
        cache{},
        sockaddrIn{},
        readTimeouts{},
        writeTimeouts{},
        mbedtlsX509Crt{},
        mbedtlsSslConfig{},
        mbedtlsSslContext{},
        jsonStream{},
        jsonRoot{},
        http{}
    {}
} CustomerCheckUnlockClient;

constexpr Jchar CUSTOMER_CHECK_HOSTNAME_FORMAT[] = "%s:%d";
constexpr Jchar CUSTOMER_CHECK_UNLOCK_URL_APPLY_UNLOCK[] =
    "/prevent_flash?operation=apply_unlock&dsn=%s&model=%s&customer=%s&customer_son=%s&old_customer=%s"
    "&old_customer_son=%s&hardware=%s&software=%s";
constexpr Jchar CUSTOMER_CHECK_UNLOCK_URL_UNLOCK_CHECK[] =
    "/prevent_flash?operation=unlock&dsn=%s&model=%s&customer=%s&customer_son=%s&old_customer=%s"
    "&old_customer_son=%s&hardware=%s&software=%s";

constexpr Jint CUSTOMER_CHECK_UNLOCK_REQUEST_TIMES = 3;
constexpr Jchar CUSTOMER_CHECK_UNLOCK_PROTOCOL_CODE[] = "Code";

class CustomerCheckUnlock
{
public:
    static CustomerCheckUnlock &Instance()
    {
        static CustomerCheckUnlock *obj = nullptr;
        if (obj == nullptr)
            obj = new CustomerCheckUnlock();
        return (*obj);
    }

    Jbool ApplyUnlock()
    {
        if (!this->Check())
        {
            Errors::Instance().SetErrorType<ErrorsType::CONFIG_INVALID>();
            return false;
        }
        if (!this->ExecuteUnlock())
        {
            Errors::Instance().SetErrorType<ErrorsType::SERVER_REQUEST_TIMEOUTS>();
            return false;
        }
        return true;
    }

    CustomerCheckUnlock &SetSN(const Jchar *v)
    {
        this->mCustomerCheckUnlockAttr.sn = v;
        return (*this);
    }

    CustomerCheckUnlock &SetModel(const Jchar *v)
    {
        this->mCustomerCheckUnlockAttr.model = v;
        return (*this);
    }

    CustomerCheckUnlock &SetCustomer(const Jchar *v)
    {
        this->mCustomerCheckUnlockAttr.customer = v;
        return (*this);
    }

    CustomerCheckUnlock &SetSubCustomer(const Jchar *v)
    {
        this->mCustomerCheckUnlockAttr.subCustomer = v;
        return (*this);
    }

    CustomerCheckUnlock &SetOldCustomer(const Jchar *v)
    {
        this->mCustomerCheckUnlockAttr.oldCustomer = v;
        return (*this);
    }

    CustomerCheckUnlock &SetOldSubCustomer(const Jchar *v)
    {
        this->mCustomerCheckUnlockAttr.oldSubCustomer = v;
        return (*this);
    }

    CustomerCheckUnlock &SetHardwareVersion(const Jchar *v)
    {
        this->mCustomerCheckUnlockAttr.hardwareVersion = v;
        return (*this);
    }

    CustomerCheckUnlock &SetSoftwareVersion(const Jchar *v)
    {
        this->mCustomerCheckUnlockAttr.softwareVersion = v;
        return (*this);
    }

    CustomerCheckUnlock &SetClientCert(const Jchar *v)
    {
        this->mCustomerCheckUnlockAttr.clientCert = v;
        return (*this);
    }

    CustomerCheckUnlock &SetAddress(const Jchar *v)
    {
        this->mCustomerCheckUnlockAttr.address = v;
        return (*this);
    }

    CustomerCheckUnlock &SetPort(Jint v)
    {
        this->mCustomerCheckUnlockAttr.port = v;
        return (*this);
    }

    CustomerCheckUnlock &SetTimeouts(Jint v)
    {
        this->mCustomerCheckUnlockAttr.timeouts = v;
        return (*this);
    }

private:
    CustomerCheckUnlockAttr mCustomerCheckUnlockAttr;
    CustomerCheckUnlockClient mCustomerCheckUnlockClient;

    CustomerCheckUnlock() :
        mCustomerCheckUnlockAttr{},
        mCustomerCheckUnlockClient{}
    {}

    Jbool Check()
    {
        return ((this->mCustomerCheckUnlockAttr.sn != nullptr)
                && (this->mCustomerCheckUnlockAttr.model != nullptr)
                && (this->mCustomerCheckUnlockAttr.customer != nullptr)
                && (this->mCustomerCheckUnlockAttr.subCustomer != nullptr)
                && (this->mCustomerCheckUnlockAttr.oldCustomer != nullptr)
                && (this->mCustomerCheckUnlockAttr.oldSubCustomer != nullptr)
                && (this->mCustomerCheckUnlockAttr.hardwareVersion != nullptr)
                && (this->mCustomerCheckUnlockAttr.softwareVersion != nullptr)
                && (this->mCustomerCheckUnlockAttr.clientCert != nullptr)
                && (this->mCustomerCheckUnlockAttr.address != nullptr));
    }

    Jbool ExecuteUnlock()
    {
        Jint count = 0;
        Jbool state = false;

        if (!this->HttpRequest<CustomerCheckUnlockProcedure::APPLY_UNLOCK>())
            return false;

        while (count < this->mCustomerCheckUnlockAttr.timeouts)
        {
            state = this->HttpRequest<CustomerCheckUnlockProcedure::UNLOCK>();
            if (state)
                break;
            count += CUSTOMER_CHECK_UNLOCK_REQUEST_TIMES;
            sleep(CUSTOMER_CHECK_UNLOCK_REQUEST_TIMES);
        }
        return state;
    }

    template<CustomerCheckUnlockProcedure _Procedure>
    Jbool HttpRequest()
    {
        Jint ret = 0;
        Jbool state = false;

        if (!this->Marshal<_Procedure>())
            return false;

        memset(this->mCustomerCheckUnlockAttr.hostName, 0, sizeof(this->mCustomerCheckUnlockAttr.hostName));
        if (snprintf(
            this->mCustomerCheckUnlockAttr.hostName,
            (sizeof(this->mCustomerCheckUnlockAttr.hostName) - 1),
            CUSTOMER_CHECK_HOSTNAME_FORMAT,
            this->mCustomerCheckUnlockAttr.address,
            this->mCustomerCheckUnlockAttr.port
        ) < 1)
            return false;

        auto &&httpContent = this->mCustomerCheckUnlockClient.http.Init()
            .SetMethod(HttpMethod::GET)
            .SetHost(this->mCustomerCheckUnlockAttr.hostName)
            .SetTarget(this->mCustomerCheckUnlockClient.target)
            .Marshal();
        if (httpContent == nullptr)
            return false;

        do
        {
            if (!this->SSLInit())
                break;

            if (ret = mbedtls_ssl_write(
                    &this->mCustomerCheckUnlockClient.mbedtlsSslContext,
                    reinterpret_cast<Jbyte *>(const_cast<Jchar *>(httpContent)),
                    strlen(httpContent)
                );ret < 1)
                break;

            memset(this->mCustomerCheckUnlockClient.cache, 0, sizeof(this->mCustomerCheckUnlockClient.cache));
            if (ret = mbedtls_ssl_read(
                    &this->mCustomerCheckUnlockClient.mbedtlsSslContext,
                    reinterpret_cast<Jbyte *>(this->mCustomerCheckUnlockClient.cache),
                    sizeof(this->mCustomerCheckUnlockClient.cache)
                );ret < 1)
                break;

            if (!this->mCustomerCheckUnlockClient.http.Parse(this->mCustomerCheckUnlockClient.cache, ret))
                break;

            Log::Instance().Print<LogType::DEBUG>("body: %s", this->mCustomerCheckUnlockClient.http.GetBody());
            this->mCustomerCheckUnlockClient.jsonStream.str(this->mCustomerCheckUnlockClient.http.GetBody());
            if (!this->Parse<_Procedure>())
                break;

            state = true;
        } while (false);

        this->SSLRelease();
        return state;
    }

    Jbool SSLInit()
    {
        Jint ret = 0;
        Jbool state = true;

        mbedtls_x509_crt_init(&this->mCustomerCheckUnlockClient.mbedtlsX509Crt);
        mbedtls_ssl_config_init(&this->mCustomerCheckUnlockClient.mbedtlsSslConfig);
        mbedtls_ssl_init(&this->mCustomerCheckUnlockClient.mbedtlsSslContext);

        if (!this->SocketInit())
            return false;

        if (ret = mbedtls_x509_crt_parse(
                &this->mCustomerCheckUnlockClient.mbedtlsX509Crt,
                reinterpret_cast<Jbyte *>(const_cast<Jchar *>(this->mCustomerCheckUnlockAttr.clientCert)),
                (strlen(this->mCustomerCheckUnlockAttr.clientCert) + 1)
            );ret < 0)
        {
            Log::Instance().Print<LogType::ERROR>("ssl cert parse fail, ret: %d", ret);
            return false;
        }

        if (ret = mbedtls_ssl_config_defaults(
                &this->mCustomerCheckUnlockClient.mbedtlsSslConfig,
                MBEDTLS_SSL_IS_CLIENT,
                MBEDTLS_SSL_TRANSPORT_STREAM,
                MBEDTLS_SSL_PRESET_DEFAULT
            );ret != 0)
        {
            Log::Instance().Print<LogType::ERROR>("ssl config fail, ret: %d", ret);
            return false;
        }

        mbedtls_ssl_conf_authmode(&this->mCustomerCheckUnlockClient.mbedtlsSslConfig, MBEDTLS_SSL_VERIFY_OPTIONAL);
        mbedtls_ssl_conf_ca_chain(
            &this->mCustomerCheckUnlockClient.mbedtlsSslConfig,
            &this->mCustomerCheckUnlockClient.mbedtlsX509Crt,
            nullptr
        );

        mbedtls_ssl_conf_rng(
            &this->mCustomerCheckUnlockClient.mbedtlsSslConfig,
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
                &this->mCustomerCheckUnlockClient.mbedtlsSslContext,
                &this->mCustomerCheckUnlockClient.mbedtlsSslConfig
            );ret != 0)
        {
            Log::Instance().Print<LogType::ERROR>("step config to ssl context is fail, ret: %d", ret);
            return false;
        }

        if (ret = mbedtls_ssl_set_hostname(
                &this->mCustomerCheckUnlockClient.mbedtlsSslContext,
                this->mCustomerCheckUnlockAttr.address
            );ret != 0)
        {
            Log::Instance().Print<LogType::ERROR>("ssl set hostname fail, ret: %d", ret);
            return false;
        }

        mbedtls_ssl_set_bio(
            &this->mCustomerCheckUnlockClient.mbedtlsSslContext,
            &this->mCustomerCheckUnlockClient,
            [](void *ctx, const Jbyte *buf, Jsize len) -> Jint {
                auto &&client = *reinterpret_cast<CustomerCheckUnlockClient *>(ctx);
                return send(client.socket, buf, len, 0);
            },
            [](void *ctx, Jbyte *buf, Jsize len) -> Jint {
                auto &&client = *reinterpret_cast<CustomerCheckUnlockClient *>(ctx);
                return recv(client.socket, buf, len, 0);
            },
            [](void *ctx, Jbyte *buf, Jsize len, Juint timeouts) -> Jint {
                auto &&client = *reinterpret_cast<CustomerCheckUnlockClient *>(ctx);
                return recv(client.socket, buf, len, 0);
            }
        );

        while ((ret = mbedtls_ssl_handshake(&this->mCustomerCheckUnlockClient.mbedtlsSslContext)) != 0)
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
        mbedtls_ssl_free(&this->mCustomerCheckUnlockClient.mbedtlsSslContext);
        mbedtls_ssl_config_free(&this->mCustomerCheckUnlockClient.mbedtlsSslConfig);
        mbedtls_x509_crt_free(&this->mCustomerCheckUnlockClient.mbedtlsX509Crt);
        this->SocketClose();
    }

    Jbool SocketInit()
    {
        Jint i = 0;

        auto &&hostT = gethostbyname(this->mCustomerCheckUnlockAttr.address);
        if (hostT == nullptr)
            return false;

        while ((*hostT).h_addr_list[i] != nullptr)
        {
            Log::Instance().Print<LogType::DEBUG>(
                "customer check unlock address parse: %s",
                inet_ntoa(*reinterpret_cast<in_addr *>((*hostT).h_addr_list[i]))
            );
            ++i;
        }

        if (i < 1)
            return false;

        this->mCustomerCheckUnlockClient.socket = socket(AF_INET, SOCK_STREAM, 0);
        this->mCustomerCheckUnlockClient.sockaddrIn.sin_family = AF_INET;
        this->mCustomerCheckUnlockClient.sockaddrIn.sin_port = htons(this->mCustomerCheckUnlockAttr.port);
        this->mCustomerCheckUnlockClient.sockaddrIn.sin_addr = *reinterpret_cast<in_addr *>((*hostT).h_addr_list[0]);

        if (connect(
            this->mCustomerCheckUnlockClient.socket,
            reinterpret_cast<sockaddr *>(&this->mCustomerCheckUnlockClient.sockaddrIn),
            sizeof(this->mCustomerCheckUnlockClient.sockaddrIn)
        ) == -1)
            return false;

        this->mCustomerCheckUnlockClient.readTimeouts.tv_sec = CUSTOMER_CHECK_UNLOCK_REQUEST_TIMES;
        this->mCustomerCheckUnlockClient.writeTimeouts.tv_sec = CUSTOMER_CHECK_UNLOCK_REQUEST_TIMES;

        if (setsockopt(
            this->mCustomerCheckUnlockClient.socket,
            SOL_SOCKET,
            SO_SNDTIMEO,
            &this->mCustomerCheckUnlockClient.writeTimeouts,
            sizeof(this->mCustomerCheckUnlockClient.writeTimeouts)
        ) != 0)
            return false;

        if (setsockopt(
            this->mCustomerCheckUnlockClient.socket,
            SOL_SOCKET,
            SO_RCVTIMEO,
            &this->mCustomerCheckUnlockClient.readTimeouts,
            sizeof(this->mCustomerCheckUnlockClient.readTimeouts)
        ) != 0)
            return false;

        auto &&flag = fcntl(this->mCustomerCheckUnlockClient.socket, F_GETFL, 0);
        fcntl(this->mCustomerCheckUnlockClient.socket, F_SETFL, (flag & ~O_NONBLOCK));
        Log::Instance().Print<LogType::INFO>("customer check unlock socket initialization successful");
        return true;
    }

    void SocketClose()
    {
        shutdown(this->mCustomerCheckUnlockClient.socket, SHUT_RDWR);
    }

    template<CustomerCheckUnlockProcedure _Procedure>
    Jbool Parse()
    {
        boost::property_tree::read_json(
            this->mCustomerCheckUnlockClient.jsonStream,
            this->mCustomerCheckUnlockClient.jsonRoot
        );
        if (_Procedure == CustomerCheckUnlockProcedure::APPLY_UNLOCK)
        {
            if (this->mCustomerCheckUnlockClient.jsonRoot.get_optional<Jint>(
                CUSTOMER_CHECK_UNLOCK_PROTOCOL_CODE
            ).get_ptr() == nullptr)
                return false;

            auto &&code = this->mCustomerCheckUnlockClient.jsonRoot.get<Jint>(CUSTOMER_CHECK_UNLOCK_PROTOCOL_CODE);
            return (code == 0);
        } else if (_Procedure == CustomerCheckUnlockProcedure::UNLOCK)
        {
            if (this->mCustomerCheckUnlockClient.jsonRoot.get_optional<Jint>(
                CUSTOMER_CHECK_UNLOCK_PROTOCOL_CODE
            ).get_ptr() == nullptr)
                return false;

            auto &&code = this->mCustomerCheckUnlockClient.jsonRoot.get<Jint>(CUSTOMER_CHECK_UNLOCK_PROTOCOL_CODE);
            return (code == 0);
        } else
        {
            return false;
        }
    }

    template<CustomerCheckUnlockProcedure _Procedure>
    Jbool Marshal()
    {
        memset(this->mCustomerCheckUnlockClient.target, 0, sizeof(this->mCustomerCheckUnlockClient.target));
        if (_Procedure == CustomerCheckUnlockProcedure::APPLY_UNLOCK)
            return (snprintf(
                this->mCustomerCheckUnlockClient.target,
                (sizeof(this->mCustomerCheckUnlockClient.target) - 1),
                CUSTOMER_CHECK_UNLOCK_URL_APPLY_UNLOCK,
                this->mCustomerCheckUnlockAttr.sn,
                this->mCustomerCheckUnlockAttr.model,
                this->mCustomerCheckUnlockAttr.customer,
                this->mCustomerCheckUnlockAttr.subCustomer,
                this->mCustomerCheckUnlockAttr.oldCustomer,
                this->mCustomerCheckUnlockAttr.oldSubCustomer,
                this->mCustomerCheckUnlockAttr.hardwareVersion,
                this->mCustomerCheckUnlockAttr.softwareVersion
            ) > 0);
        else if (_Procedure == CustomerCheckUnlockProcedure::UNLOCK)
            return (snprintf(
                this->mCustomerCheckUnlockClient.target,
                (sizeof(this->mCustomerCheckUnlockClient.target) - 1),
                CUSTOMER_CHECK_UNLOCK_URL_UNLOCK_CHECK,
                this->mCustomerCheckUnlockAttr.sn,
                this->mCustomerCheckUnlockAttr.model,
                this->mCustomerCheckUnlockAttr.customer,
                this->mCustomerCheckUnlockAttr.subCustomer,
                this->mCustomerCheckUnlockAttr.oldCustomer,
                this->mCustomerCheckUnlockAttr.oldSubCustomer,
                this->mCustomerCheckUnlockAttr.hardwareVersion,
                this->mCustomerCheckUnlockAttr.softwareVersion
            ) > 0);
        else
            return false;
    }
};

}

#endif //SMARTTOOLS_SMART_ANDROID_CUSTOMER_CHECK_UNLOCK_HPP
