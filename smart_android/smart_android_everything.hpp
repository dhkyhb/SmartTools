#ifndef SMARTTOOLS_SMART_ANDROID_EVERYTHING_HPP
#define SMARTTOOLS_SMART_ANDROID_EVERYTHING_HPP

#include "../kernel.hpp"
#include "../smart_utils/smart_utils_thread_loop.hpp"

#include "smart_android_sensor.hpp"
#include "smart_android_remote_active_0.hpp"
#include "smart_android_remote_active_1.hpp"

namespace smart::android::everything
{

using kernel::ItType;
using smart::utils::thread::loop::ThreadLoop;
using smart::android::sensor::Sensor;

constexpr Jint EVERYTHING_LINE_UP_MAX_SIZE = 256;

using EverythingInitCall = void (*)(void *p);
using EverythingExecuteCall = void (*)(void *p, Jint thingType);
using EverythingEndCall = void (*)(void *p, Jint thingType, Jbool state);

enum class EverythingType
{
    SENSOR_CHECK = 0,
    SENSOR_ACTIVE,
};

typedef struct
{
    Jbool isUing;
    EverythingType thingType;
} EverythingAttr;

typedef struct
{
    Jbool needInit;
    void *p;

    EverythingInitCall thingInit;
    EverythingEndCall thingEnd;
    EverythingExecuteCall thingExecute;
    EverythingAttr things[EVERYTHING_LINE_UP_MAX_SIZE];
    std::mutex lineUpMutex;
} EverythingLineUp;

class Everything
{
public:
    static Everything &Instance()
    {
        static Everything *obj = nullptr;
        if (obj == nullptr)
            obj = new Everything();
        return (*obj);
    }

    void Process()
    {
        this->Listen();
    }

    template<typename Init, typename Execute, typename End>
    Everything &SetExecuteThingListen(void *p, ItType<Init> fun0, ItType<Execute> fun1, ItType<End> fun2)
    {
        this->mEverythingLineUp.p = p;
        this->mEverythingLineUp.thingInit = fun0;
        this->mEverythingLineUp.thingExecute = fun1;
        this->mEverythingLineUp.thingEnd = fun2;
        return (*this);
    }

    Everything &NeedInit()
    {
        this->mEverythingLineUp.needInit = true;
        return (*this);
    }

    Everything &SendThing(EverythingType type)
    {
        this->mEverythingLineUp.lineUpMutex.lock();

        for (EverythingAttr &thing : this->mEverythingLineUp.things)
        {
            if (thing.isUing)
                continue;

            thing.thingType = type;
            thing.isUing = true;
        }

        this->mEverythingLineUp.lineUpMutex.unlock();
    }

private:
    EverythingLineUp mEverythingLineUp;

    Everything() : mEverythingLineUp{}
    {}

    void Listen()
    {
        static Jbool isRun = false;

        if (isRun)
            return;

        isRun = true;
        ThreadLoop::Instance().Insert(&this->mEverythingLineUp, [](Jint id, void *p) {
            Jint i = 0;
            Jbool state = false;
            auto &thing = *reinterpret_cast<EverythingLineUp *>(p);

            while (true)
            {
                state = false;

                if (thing.needInit)
                {
                    thing.thingInit(thing.p);
                    thing.needInit = false;
                }

                for (EverythingAttr &v : thing.things)
                {
                    if (!v.isUing)
                        continue;

                    if (thing.thingExecute != nullptr)
                        thing.thingExecute(thing.p, static_cast<Jint>(v.thingType));

                    if (v.thingType == EverythingType::SENSOR_CHECK)
                    {
                        Sensor::Instance().Check();
                        state = true;
                    } else if (v.thingType == EverythingType::SENSOR_ACTIVE)
                    {
                        state = Sensor::Instance().Active();
                    }

                    if (thing.thingEnd != nullptr)
                        thing.thingEnd(thing.p, static_cast<Jint>(v.thingType), state);
                }

                std::this_thread::sleep_for(std::chrono::microseconds(200));
            }
        }).Process();
    }
};

}

#endif //SMARTTOOLS_SMART_ANDROID_EVERYTHING_HPP
