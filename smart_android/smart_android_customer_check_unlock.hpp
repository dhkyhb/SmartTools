#ifndef SMARTTOOLS_SMART_ANDROID_CUSTOMER_CHECK_UNLOCK_HPP
#define SMARTTOOLS_SMART_ANDROID_CUSTOMER_CHECK_UNLOCK_HPP

#include "../kernel.hpp"

namespace smart::android::customer::check::unlock
{

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
        return false;
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

    CustomerCheckUnlock() :
        mCustomerCheckUnlockAttr{}
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
};

}

#endif //SMARTTOOLS_SMART_ANDROID_CUSTOMER_CHECK_UNLOCK_HPP
