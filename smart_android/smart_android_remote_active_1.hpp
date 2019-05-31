#ifndef SMARTTOOLS_SMART_ANDROID_REMOTE_ACTIVE_1_HPP
#define SMARTTOOLS_SMART_ANDROID_REMOTE_ACTIVE_1_HPP

#include "../kernel.hpp"

namespace smart::android::remote::active1
{

// New version, it's based on HTTP protocol to unlock request.
class RemoteActive1
{
public:
    static RemoteActive1 &Instance()
    {
        static RemoteActive1 *obj = nullptr;
        if (obj == nullptr)
            obj = new RemoteActive1();
        return (*obj);
    }

private:
    RemoteActive1() = default;
};

}

#endif //SMARTTOOLS_SMART_ANDROID_REMOTE_ACTIVE_1_HPP
