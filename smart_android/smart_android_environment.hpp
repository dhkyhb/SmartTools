#pragma once

#ifndef SMARTTOOLS_SMART_ANDROID_ENVIRONMENT_HPP
#define SMARTTOOLS_SMART_ANDROID_ENVIRONMENT_HPP

#include "../kernel.hpp"

#include <APosAccessoryManager.h>

namespace smart::android::environment
{

constexpr Jint ENVIRONMENT_SN_BUF_SIZE = 64;
constexpr Jint ENVIRONMENT_MAX_CALL_INTERFACE_COUNT = 10;
constexpr Jint ENVIRONMENT_CERT_BUF_SIZE = 4096;

typedef struct
{
    Jchar sn[ENVIRONMENT_SN_BUF_SIZE];
    Jchar deviceModel[ENVIRONMENT_SN_BUF_SIZE];
    Jchar oldCustomer[ENVIRONMENT_SN_BUF_SIZE];
    Jchar oldSubCustomer[ENVIRONMENT_SN_BUF_SIZE];
    Jchar androidVersion[ENVIRONMENT_SN_BUF_SIZE];
    Jchar androidSDKVersion[ENVIRONMENT_SN_BUF_SIZE];
    Jchar androidID[ENVIRONMENT_SN_BUF_SIZE];
    Jchar androidDevice[ENVIRONMENT_SN_BUF_SIZE];
    Jchar androidBootloader[ENVIRONMENT_SN_BUF_SIZE];

    Jchar customer[ENVIRONMENT_SN_BUF_SIZE];
    Jchar subCustomer[ENVIRONMENT_SN_BUF_SIZE];
    Jchar hardwareVersion[ENVIRONMENT_SN_BUF_SIZE];
    Jchar softwareVersion[ENVIRONMENT_SN_BUF_SIZE];
    Jchar spVersion[ENVIRONMENT_SN_BUF_SIZE];

    Jchar customerCheckCert[ENVIRONMENT_CERT_BUF_SIZE];
} EnvironmentAttr;

typedef struct
{
    Jint customerLen;
    Jchar *customer;
    Jint subCustomerLen;
    Jchar *subCustomer;
    Jint hardwareLen;
    Jchar *hardware;
    Jint softwareLen;
    Jchar *software;
    Jint spVersionLen;
    Jchar *spVersion;
    APosAccessoryManager *context;

    Jint count;
    std::mutex posMutex;
} POSSDKSupport;

class Environment
{
public:
    static Environment &Instance()
    {
        static Environment *obj = nullptr;
        if (obj == nullptr)
            obj = new Environment();
        return (*obj);
    }

    Environment &SetSN(const Jchar *v)
    {
        Jint vLen = 0;

        do
        {
            if (v == nullptr)
                break;
            if (vLen = strlen(v);vLen > static_cast<Jint>(sizeof(this->mEnvironmentAttr.sn) - 1))
                break;

            memset(this->mEnvironmentAttr.sn, 0, sizeof(this->mEnvironmentAttr.sn));
            memcpy(this->mEnvironmentAttr.sn, v, vLen);
        } while (false);
        return (*this);
    }

    Environment &SetDeviceModel(const Jchar *v)
    {
        Jint vLen = 0;

        do
        {
            if (v == nullptr)
                break;
            if (vLen = strlen(v);vLen > static_cast<Jint>(sizeof(this->mEnvironmentAttr.deviceModel) - 1))
                break;

            memset(this->mEnvironmentAttr.deviceModel, 0, sizeof(this->mEnvironmentAttr.deviceModel));
            memcpy(this->mEnvironmentAttr.deviceModel, v, vLen);
        } while (false);
        return (*this);
    }

    Environment &SetCustomer(const Jchar *v)
    {
        Jint vLen = 0;

        do
        {
            if (v == nullptr)
                break;
            if (vLen = strlen(v);vLen > static_cast<Jint>(sizeof(this->mEnvironmentAttr.customer) - 1))
                break;

            memset(this->mEnvironmentAttr.customer, 0, sizeof(this->mEnvironmentAttr.customer));
            memcpy(this->mEnvironmentAttr.customer, v, vLen);
        } while (false);
        return (*this);
    }

    Environment &SetSubCustomer(const Jchar *v)
    {
        Jint vLen = 0;

        do
        {
            if (v == nullptr)
                break;
            if (vLen = strlen(v);vLen > static_cast<Jint>(sizeof(this->mEnvironmentAttr.subCustomer) - 1))
                break;

            memset(this->mEnvironmentAttr.subCustomer, 0, sizeof(this->mEnvironmentAttr.subCustomer));
            memcpy(this->mEnvironmentAttr.subCustomer, v, vLen);
        } while (false);
        return (*this);
    }

    Environment &SetOldCustomer(const Jchar *v)
    {
        Jint vLen = 0;

        do
        {
            if (v == nullptr)
                break;
            if (vLen = strlen(v);vLen > static_cast<Jint>(sizeof(this->mEnvironmentAttr.oldCustomer) - 1))
                break;

            memset(this->mEnvironmentAttr.oldCustomer, 0, sizeof(this->mEnvironmentAttr.oldCustomer));
            memcpy(this->mEnvironmentAttr.oldCustomer, v, vLen);
        } while (false);
        return (*this);
    }

    Environment &SetOldSubCustomer(const Jchar *v)
    {
        Jint vLen = 0;

        do
        {
            if (v == nullptr)
                break;
            if (vLen = strlen(v);vLen > static_cast<Jint>(sizeof(this->mEnvironmentAttr.oldCustomer) - 1))
                break;

            memset(this->mEnvironmentAttr.oldSubCustomer, 0, sizeof(this->mEnvironmentAttr.oldSubCustomer));
            memcpy(this->mEnvironmentAttr.oldSubCustomer, v, vLen);
        } while (false);
        return (*this);
    }

    Environment &SetAndroidVersion(const Jchar *v)
    {
        Jint vLen = 0;

        do
        {
            if (v == nullptr)
                break;
            if (vLen = strlen(v);vLen > static_cast<Jint>(sizeof(this->mEnvironmentAttr.androidVersion) - 1))
                break;

            memset(this->mEnvironmentAttr.androidVersion, 0, sizeof(this->mEnvironmentAttr.androidVersion));
            memcpy(this->mEnvironmentAttr.androidVersion, v, vLen);
        } while (false);
        return (*this);
    }

    Environment &SetAndroidSDKVersion(const Jchar *v)
    {
        Jint vLen = 0;

        do
        {
            if (v == nullptr)
                break;
            if (vLen = strlen(v);vLen > static_cast<Jint>(sizeof(this->mEnvironmentAttr.androidSDKVersion) - 1))
                break;

            memset(this->mEnvironmentAttr.androidSDKVersion, 0, sizeof(this->mEnvironmentAttr.androidSDKVersion));
            memcpy(this->mEnvironmentAttr.androidSDKVersion, v, vLen);
        } while (false);
        return (*this);
    }

    Environment &SetAndroidID(const Jchar *v)
    {
        Jint vLen = 0;

        do
        {
            if (v == nullptr)
                break;
            if (vLen = strlen(v);vLen > static_cast<Jint>(sizeof(this->mEnvironmentAttr.androidID) - 1))
                break;

            memset(this->mEnvironmentAttr.androidID, 0, sizeof(this->mEnvironmentAttr.androidID));
            memcpy(this->mEnvironmentAttr.androidID, v, vLen);
        } while (false);
        return (*this);
    }

    Environment &SetAndroidDevice(const Jchar *v)
    {
        Jint vLen = 0;

        do
        {
            if (v == nullptr)
                break;
            if (vLen = strlen(v);vLen > static_cast<Jint>(sizeof(this->mEnvironmentAttr.androidDevice) - 1))
                break;

            memset(this->mEnvironmentAttr.androidDevice, 0, sizeof(this->mEnvironmentAttr.androidDevice));
            memcpy(this->mEnvironmentAttr.androidDevice, v, vLen);
        } while (false);
        return (*this);
    }

    Environment &SetAndroidBootloader(const Jchar *v)
    {
        Jint vLen = 0;

        do
        {
            if (v == nullptr)
                break;
            if (vLen = strlen(v);vLen > static_cast<Jint>(sizeof(this->mEnvironmentAttr.androidBootloader) - 1))
                break;

            memset(this->mEnvironmentAttr.androidBootloader, 0, sizeof(this->mEnvironmentAttr.androidBootloader));
            memcpy(this->mEnvironmentAttr.androidBootloader, v, vLen);
        } while (false);
        return (*this);
    }

    Environment &SetCustomerCheckCert(const Jchar *v)
    {
        Jint vLen = 0;

        do
        {
            if (v == nullptr)
                break;
            if (vLen = strlen(v);vLen > static_cast<Jint>(sizeof(this->mEnvironmentAttr.customerCheckCert) - 1))
                break;

            memset(this->mEnvironmentAttr.customerCheckCert, 0, sizeof(this->mEnvironmentAttr.customerCheckCert));
            memcpy(this->mEnvironmentAttr.customerCheckCert, v, vLen);
        } while (false);
        return (*this);
    }

    const Jchar *GetSN()
    {
        return this->mEnvironmentAttr.sn;
    }

    const Jchar *GetDeviceModel()
    {
        return this->mEnvironmentAttr.deviceModel;
    }

    const Jchar *GetOldCustomer()
    {
        return this->mEnvironmentAttr.oldCustomer;
    }

    const Jchar *GetOldSubCustomer()
    {
        return this->mEnvironmentAttr.oldSubCustomer;
    }

    const Jchar *GetCustomerCheckCert()
    {
        return this->mEnvironmentAttr.customerCheckCert;
    }

    const Jchar *GetAndroidVersion()
    {
        return this->mEnvironmentAttr.androidVersion;
    }

    const Jchar *GetAndroidSDKVersion()
    {
        return this->mEnvironmentAttr.androidSDKVersion;
    }

    const Jchar *GetAndroidID()
    {
        return this->mEnvironmentAttr.androidID;
    }

    const Jchar *GetAndroidDevice()
    {
        return this->mEnvironmentAttr.androidDevice;
    }

    const Jchar *GetAndroidBootloader()
    {
        return this->mEnvironmentAttr.androidBootloader;
    }

    const Jchar *GetCustomer()
    {
        if (strlen(this->mEnvironmentAttr.customer) == 0)
            this->ToGetSysInformations();
        return this->mEnvironmentAttr.customer;
    }

    const Jchar *GetSubCustomer()
    {
        if (strlen(this->mEnvironmentAttr.subCustomer) == 0)
            this->ToGetSysInformations();
        return this->mEnvironmentAttr.subCustomer;
    }

    const Jchar *GetHardwareVersion()
    {
        if (strlen(this->mEnvironmentAttr.hardwareVersion) == 0)
            this->ToGetSysInformations();
        return this->mEnvironmentAttr.hardwareVersion;
    }

    const Jchar *GetSoftwareVersion()
    {
        if (strlen(this->mEnvironmentAttr.softwareVersion) == 0)
            this->ToGetSysInformations();
        return this->mEnvironmentAttr.softwareVersion;
    }

    const Jchar *GetSPVersion()
    {
        if (strlen(this->mEnvironmentAttr.spVersion) == 0)
            this->ToGetSysInformations();
        return this->mEnvironmentAttr.spVersion;
    }

private:
    POSSDKSupport mPOSSDKSupport;
    EnvironmentAttr mEnvironmentAttr;

    Environment() :
        mPOSSDKSupport{},
        mEnvironmentAttr{}
    {
        this->mPOSSDKSupport.context = APosAccessoryManager_new();
    }

    Jbool Check()
    {
        return (this->mPOSSDKSupport.context != nullptr);
    }

    void ToGetSysInformations()
    {
        if (!this->Check())
            return;

        this->mPOSSDKSupport.posMutex.lock();

        APosAccessoryManager_getVersion(
            this->mPOSSDKSupport.context,
            VERSION_TYPE_HW,
            &this->mPOSSDKSupport.hardware,
            &this->mPOSSDKSupport.hardwareLen
        );
        APosAccessoryManager_getVersion(
            this->mPOSSDKSupport.context,
            VERSION_TYPE_AP,
            &this->mPOSSDKSupport.software,
            &this->mPOSSDKSupport.softwareLen
        );
        APosAccessoryManager_getVersion(
            this->mPOSSDKSupport.context,
            VERSION_TYPE_CUSTOMER_NAME,
            &this->mPOSSDKSupport.customer,
            &this->mPOSSDKSupport.customerLen
        );
        APosAccessoryManager_getVersion(
            this->mPOSSDKSupport.context,
            VERSION_TYPE_CUSTOMER_SUBNAME,
            &this->mPOSSDKSupport.subCustomer,
            &this->mPOSSDKSupport.subCustomerLen
        );
        APosAccessoryManager_getVersion(
            this->mPOSSDKSupport.context,
            VERSION_TYPE_SP,
            &this->mPOSSDKSupport.spVersion,
            &this->mPOSSDKSupport.spVersionLen
        );

        if ((this->mPOSSDKSupport.hardware != nullptr) && (this->mPOSSDKSupport.hardwareLen > 0))
        {
            memset(this->mEnvironmentAttr.hardwareVersion, 0, sizeof(this->mEnvironmentAttr.hardwareVersion));
            memcpy(
                this->mEnvironmentAttr.hardwareVersion,
                this->mPOSSDKSupport.hardware,
                this->mPOSSDKSupport.hardwareLen
            );
        }

        if ((this->mPOSSDKSupport.software != nullptr) && (this->mPOSSDKSupport.softwareLen > 0))
        {
            memset(this->mEnvironmentAttr.softwareVersion, 0, sizeof(this->mEnvironmentAttr.softwareVersion));
            memcpy(
                this->mEnvironmentAttr.softwareVersion,
                this->mPOSSDKSupport.software,
                this->mPOSSDKSupport.softwareLen
            );
        }

        if ((this->mPOSSDKSupport.customer != nullptr) && (this->mPOSSDKSupport.customerLen > 0))
        {
            memset(this->mEnvironmentAttr.customer, 0, sizeof(this->mEnvironmentAttr.customer));
            memcpy(
                this->mEnvironmentAttr.customer,
                this->mPOSSDKSupport.customer,
                this->mPOSSDKSupport.customerLen
            );
        }

        if ((this->mPOSSDKSupport.subCustomer != nullptr) && (this->mPOSSDKSupport.subCustomerLen > 0))
        {
            memset(this->mEnvironmentAttr.subCustomer, 0, sizeof(this->mEnvironmentAttr.subCustomer));
            memcpy(
                this->mEnvironmentAttr.subCustomer,
                this->mPOSSDKSupport.subCustomer,
                this->mPOSSDKSupport.subCustomerLen
            );
        }

        if ((this->mPOSSDKSupport.spVersion != nullptr) && (this->mPOSSDKSupport.spVersionLen > 0))
        {
            memset(this->mEnvironmentAttr.spVersion, 0, sizeof(this->mEnvironmentAttr.spVersion));
            memcpy(
                this->mEnvironmentAttr.spVersion,
                this->mPOSSDKSupport.spVersion,
                this->mPOSSDKSupport.spVersionLen
            );
        }

        if (this->mPOSSDKSupport.customer != nullptr)
            free(this->mPOSSDKSupport.customer);
        if (this->mPOSSDKSupport.subCustomer)
            free(this->mPOSSDKSupport.subCustomer);
        if (this->mPOSSDKSupport.hardware != nullptr)
            free(this->mPOSSDKSupport.hardware);
        if (this->mPOSSDKSupport.software != nullptr)
            free(this->mPOSSDKSupport.software);
        if (this->mPOSSDKSupport.spVersion != nullptr)
            free(this->mPOSSDKSupport.spVersion);

        this->mPOSSDKSupport.posMutex.unlock();
    }
};

}

#endif //SMARTTOOLS_SMART_ANDROID_ENVIRONMENT_HPP
