#ifndef SMARTTOOLS_SMART_ANDROID_CUSTOMER_CHECK_UNLOCK_HPP
#define SMARTTOOLS_SMART_ANDROID_CUSTOMER_CHECK_UNLOCK_HPP

#include "../kernel.hpp"
#include "../smart_utils/smart_utils_simple_http.hpp"
#include "../smart_resouces/smart_resouces_errors.hpp"

#include <mbedtls/ssl.h>

namespace smart::android::customer::check::unlock
{

using smart::utils::simple::http::Http;
using smart::utils::simple::http::HttpMethod;
using smart::resouces::errors::Errors;
using smart::resouces::errors::ErrorsType;

constexpr Jint CUSTOMER_CHECK_UNLOCK_BUF_SIZE = 2048;

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
} CustomerCheckUnlockAttr;

typedef struct
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

    Http http;
} CustomerCheckUnlockClient;

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
            return false;
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
                && (this->mCustomerCheckUnlockAttr.softwareVersion != nullptr));
    }

    Jbool ExecuteUnlock()
    {
        return true;
    }

    Jbool HttpRequest()
    {
        return false;
    }

    Jbool SSLInit()
    {
        return false;
    }

    Jbool SSLRelease()
    {
        return false;
    }

    Jbool SocketInit()
    {
        return false;
    }

    Jbool SocketClose()
    {
        return false;
    }
};

}

#endif //SMARTTOOLS_SMART_ANDROID_CUSTOMER_CHECK_UNLOCK_HPP
