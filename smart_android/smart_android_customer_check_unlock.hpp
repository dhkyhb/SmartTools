#ifndef SMARTTOOLS_SMART_ANDROID_CUSTOMER_CHECK_UNLOCK_HPP
#define SMARTTOOLS_SMART_ANDROID_CUSTOMER_CHECK_UNLOCK_HPP

#include "../kernel.hpp"

namespace smart::android::customer::check::unlock
{

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

private:
};

}

#endif //SMARTTOOLS_SMART_ANDROID_CUSTOMER_CHECK_UNLOCK_HPP
