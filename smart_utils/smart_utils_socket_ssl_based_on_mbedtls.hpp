#pragma once

#ifndef SMARTTOOLS_SMART_UTILS_SOCKET_SSL_BASED_ON_MBEDTLS_HPP
#define SMARTTOOLS_SMART_UTILS_SOCKET_SSL_BASED_ON_MBEDTLS_HPP

#include "../kernel.hpp"

#include <mbedtls/ssl.h>

namespace smart::utils::soket::ssl::based::on::mbedlts
{

typedef struct
{
    Jint socket;
    Jbool hasHandshake;
    const Jchar *hostName;
    const Jchar *cert;

    mbedtls_x509_crt mbedtlsX509Crt;
    mbedtls_ssl_config mbedtlsSslConfig;
    mbedtls_ssl_context mbedtlsSslContext;
} SocketSSLAttr;

class SocketSSL
{
public:
    SocketSSL() :
        mSocketSSLAttr{}
    {}

    ~SocketSSL() = default;

    Jint Write(const Jbyte *v, Jint vLen)
    {
        if (!this->mSocketSSLAttr.hasHandshake)
            return 0;
        return mbedtls_ssl_write(&this->mSocketSSLAttr.mbedtlsSslContext, v, vLen);
    }

    Jint Read(Jbyte *v, Jint vLen)
    {
        if (!this->mSocketSSLAttr.hasHandshake)
            return 0;
        return mbedtls_ssl_read(&this->mSocketSSLAttr.mbedtlsSslContext, v, vLen);
    }

    Jbool Connect()
    {
        this->mSocketSSLAttr.hasHandshake = false;
        if (!this->Check())
            return this->mSocketSSLAttr.hasHandshake;

        this->Init();

        do
        {
            if (!this->Parameter())
                break;
            this->mSocketSSLAttr.hasHandshake = this->Handshake();
        } while (false);
        return this->mSocketSSLAttr.hasHandshake;
    }

    void DisConnect()
    {
        this->Release();
    }

    SocketSSL &SetSocketID(Jint socket)
    {
        this->mSocketSSLAttr.socket = socket;
        return (*this);
    }

    SocketSSL &SetCert(const Jchar *v)
    {
        this->mSocketSSLAttr.cert = v;
        return (*this);
    }

    SocketSSL &SetHostName(const Jchar *v)
    {
        this->mSocketSSLAttr.hostName = v;
        return (*this);
    }

private:
    SocketSSLAttr mSocketSSLAttr;

    Jbool Check()
    {
        return ((this->mSocketSSLAttr.cert != nullptr)
                && (this->mSocketSSLAttr.hostName != nullptr));
    }

    void Init()
    {
        mbedtls_x509_crt_init(&this->mSocketSSLAttr.mbedtlsX509Crt);
        mbedtls_ssl_config_init(&this->mSocketSSLAttr.mbedtlsSslConfig);
        mbedtls_ssl_init(&this->mSocketSSLAttr.mbedtlsSslContext);
    }

    void Release()
    {
        mbedtls_ssl_free(&this->mSocketSSLAttr.mbedtlsSslContext);
        mbedtls_ssl_config_free(&this->mSocketSSLAttr.mbedtlsSslConfig);
        mbedtls_x509_crt_free(&this->mSocketSSLAttr.mbedtlsX509Crt);
    }

    Jbool Parameter()
    {
        Jint ret = 0;

        if (ret = mbedtls_x509_crt_parse(
                &this->mSocketSSLAttr.mbedtlsX509Crt,
                reinterpret_cast<Jbyte *>(const_cast<Jchar *>(this->mSocketSSLAttr.cert)),
                (strlen(this->mSocketSSLAttr.cert) + 1)
            );
            ret < 0)
        {
            Log::Instance().Print<LogType::ERROR>("ssl cert parse fail, ret: %d", ret);
            return false;
        }

        if (ret = mbedtls_ssl_config_defaults(
                &this->mSocketSSLAttr.mbedtlsSslConfig,
                MBEDTLS_SSL_IS_CLIENT,
                MBEDTLS_SSL_TRANSPORT_STREAM,
                MBEDTLS_SSL_PRESET_DEFAULT
            );ret != 0)
        {
            Log::Instance().Print<LogType::ERROR>("ssl config fail, ret: %d", ret);
            return false;
        }

        mbedtls_ssl_conf_authmode(&this->mSocketSSLAttr.mbedtlsSslConfig, MBEDTLS_SSL_VERIFY_OPTIONAL);
        mbedtls_ssl_conf_ca_chain(
            &this->mSocketSSLAttr.mbedtlsSslConfig,
            &this->mSocketSSLAttr.mbedtlsX509Crt,
            nullptr
        );

        mbedtls_ssl_conf_rng(
            &this->mSocketSSLAttr.mbedtlsSslConfig,
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
                &this->mSocketSSLAttr.mbedtlsSslContext,
                &this->mSocketSSLAttr.mbedtlsSslConfig
            );ret != 0)
        {
            Log::Instance().Print<LogType::ERROR>("step config to ssl context is fail, ret: %d", ret);
            return false;
        }

        if (ret = mbedtls_ssl_set_hostname(
                &this->mSocketSSLAttr.mbedtlsSslContext,
                this->mSocketSSLAttr.hostName
            );ret != 0)
        {
            Log::Instance().Print<LogType::ERROR>("ssl set hostname fail, ret: %d", ret);
            return false;
        }

        mbedtls_ssl_set_bio(
            &this->mSocketSSLAttr.mbedtlsSslContext,
            &this->mSocketSSLAttr,
            [](void *ctx, const Jbyte *buf, Jsize len) -> Jint {
                auto &&client = *reinterpret_cast<SocketSSLAttr *>(ctx);
                return send(client.socket, buf, len, 0);
            },
            [](void *ctx, Jbyte *buf, Jsize len) -> Jint {
                auto &&client = *reinterpret_cast<SocketSSLAttr *>(ctx);
                return recv(client.socket, buf, len, 0);
            },
            [](void *ctx, Jbyte *buf, Jsize len, Juint timeouts) -> Jint {
                auto &&client = *reinterpret_cast<SocketSSLAttr *>(ctx);
                return recv(client.socket, buf, len, 0);
            }
        );

        return true;
    }

    Jbool Handshake()
    {
        Jint ret = 0;
        Jbool state = true;

        while ((ret = mbedtls_ssl_handshake(&this->mSocketSSLAttr.mbedtlsSslContext)) != 0)
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

};

}

#endif //SMARTTOOLS_SMART_UTILS_SOCKET_SSL_BASED_ON_MBEDTLS_HPP
