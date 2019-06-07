#ifndef SMARTTOOLS_SMART_ANDROID_ENVIRONMENT_HPP
#define SMARTTOOLS_SMART_ANDROID_ENVIRONMENT_HPP

#include "../kernel.hpp"

#include <APosAccessoryManager.h>

namespace smart::android::environment
{

constexpr Jint ENVIRONMENT_SN_BUF_SIZE = 32;
constexpr Jint ENVIRONMENT_MAX_CALL_INTERFACE_COUNT = 10;

typedef struct
{
    Jchar sn[ENVIRONMENT_SN_BUF_SIZE];
    Jchar deviceModel[ENVIRONMENT_SN_BUF_SIZE];
    Jchar oldCustomer[ENVIRONMENT_SN_BUF_SIZE];
    Jchar oldSubCustomer[ENVIRONMENT_SN_BUF_SIZE];

    Jchar customer[ENVIRONMENT_SN_BUF_SIZE];
    Jchar subCustomer[ENVIRONMENT_SN_BUF_SIZE];
    Jchar hardwareVersion[ENVIRONMENT_SN_BUF_SIZE];
    Jchar softwareVersion[ENVIRONMENT_SN_BUF_SIZE];
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
            if (vLen = strlen(v);vLen > static_cast<Jint>(sizeof(this->mEnvironmentAttr.deviceModel) - 1 ))
                break;

            memset(this->mEnvironmentAttr.deviceModel, 0, sizeof(this->mEnvironmentAttr.deviceModel));
            memcpy(this->mEnvironmentAttr.deviceModel, v, vLen);
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

    const Jchar *GetCustomer()
    {
        this->ToGetSysInformations();
        return this->mEnvironmentAttr.customer;
    }

    const Jchar *GetSubCustomer()
    {
        this->ToGetSysInformations();
        return this->mEnvironmentAttr.subCustomer;
    }

    const Jchar *GetHardwareVersion()
    {
        this->ToGetSysInformations();
        return this->mEnvironmentAttr.hardwareVersion;
    }

    const Jchar *GetSoftwareVersion()
    {
        this->ToGetSysInformations();
        return this->mEnvironmentAttr.softwareVersion;
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

    Jbool CallCount()
    {
        if (this->mPOSSDKSupport.count < ENVIRONMENT_MAX_CALL_INTERFACE_COUNT)
        {
            ++this->mPOSSDKSupport.count;
            return false;
        } else
        {
            this->mPOSSDKSupport.count = 0;
            return true;
        }
    }

    void ToGetSysInformations()
    {
        static Jbool state = false;

        if (state)
        {
            if (!this->CallCount())
                return;
        } else
        {
            state = true;
        }

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

        if (this->mPOSSDKSupport.customer != nullptr)
            free(this->mPOSSDKSupport.customer);
        if (this->mPOSSDKSupport.subCustomer)
            free(this->mPOSSDKSupport.subCustomer);
        if (this->mPOSSDKSupport.hardware != nullptr)
            free(this->mPOSSDKSupport.hardware);
        if (this->mPOSSDKSupport.software != nullptr)
            free(this->mPOSSDKSupport.software);

        this->mPOSSDKSupport.posMutex.unlock();
    }
};

}

#endif //SMARTTOOLS_SMART_ANDROID_ENVIRONMENT_HPP
