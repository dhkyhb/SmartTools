#ifndef SMARTTOOLS_SMART_ANDROID_EVERYTHING_HPP
#define SMARTTOOLS_SMART_ANDROID_EVERYTHING_HPP

#include "../kernel.hpp"
#include "../smart_utils/smart_utils_thread_loop.hpp"
#include "../smart_resouces/smart_resouces_status.hpp"
#include "../smart_resouces/smart_resouces_errors.hpp"

#include "smart_android_environment.hpp"
#include "smart_android_sensor.hpp"
#include "smart_android_remote_active_0.hpp"
#include "smart_android_remote_active_1.hpp"
#include "smart_android_customer_check_unlock.hpp"
#include "smart_android_sp_tampered_log.hpp"

namespace smart::android::everything
{

using kernel::ItType;
using smart::utils::thread::loop::ThreadLoop;
using smart::resouces::status::StatusType;
using smart::resouces::errors::Errors;
using smart::resouces::errors::ErrorsType;

using smart::android::environment::Environment;
using smart::android::sensor::Sensor;
using smart::android::remote::active0::RemoteActive0;
using smart::android::remote::active1::RemoteActive1;
using smart::android::customer::check::unlock::CustomerCheckUnlock;
using smart::android::sp::tampered::log::SPTamperedLog;

constexpr Jint EVERYTHING_LINE_UP_MAX_SIZE = 256;

using EverythingInitCall = void (*)(void *p);
using EverythingExecuteCall = void (*)(void *p, Jint thingType);
using EverythingProcessCall = void (*)(void *p, StatusType status);
using EverythingEndCall = void (*)(void *p, Jint thingType, Jbool state, ErrorsType type);

enum class EverythingType
{
    REMOTE_ACTIVATION0 = 0,
    REMOTE_ACTIVATION1,
    CUSTOMER_CHECK_UNLOCK,
    GET_SP_TAMPERED_LOG,
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
    EverythingProcessCall thingProcess;

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

    template<typename Init, typename Execute, typename Process, typename End>
    Everything &SetExecuteThingListen(
            void *p,
            ItType<Init> fun0,
            ItType<Execute> fun1,
            ItType<Process> fun2,
            ItType<End> fun3
    )
    {
        this->mEverythingLineUp.p = p;
        this->mEverythingLineUp.thingInit = fun0;
        this->mEverythingLineUp.thingExecute = fun1;
        this->mEverythingLineUp.thingProcess = fun2;
        this->mEverythingLineUp.thingEnd = fun3;
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
            break;
        }

        this->mEverythingLineUp.lineUpMutex.unlock();
        return (*this);
    }

private:
    EverythingLineUp mEverythingLineUp;

    Everything() :
            mEverythingLineUp{}
    {}

    void Listen()
    {
        static Jbool isRun = false;

        if (isRun)
            return;

        isRun = true;
        ThreadLoop::Instance().Insert(&this->mEverythingLineUp, [](Jint id, void *p) {
            Jbool state = false;
            auto &thing = *reinterpret_cast<EverythingLineUp *>(p);

            while (true)
            {
                if (thing.needInit)
                {
                    thing.thingInit(thing.p);
                    thing.needInit = false;
                }

                for (EverythingAttr &v : thing.things)
                {
                    state = false;
                    if (!v.isUing)
                        continue;

                    if (v.thingType == EverythingType::REMOTE_ACTIVATION0)
                    {
                        Log::Instance().Print<LogType::INFO>("ready for remote activation");
                        thing.thingExecute(thing.p, static_cast<Jint>(v.thingType));

                        do
                        {
                            thing.thingProcess(thing.p, StatusType::SENSOR_CHECK_READY);
                            Sensor::Instance().Check();
                            thing.thingProcess(thing.p, StatusType::SENSOR_CHECK_DONE);

                            thing.thingProcess(thing.p, StatusType::REMOTE_ACTIVATION0_UNLOCK_READY);
                            if (!RemoteActive0::Instance().SetSN(Environment::Instance().GetSN()).ApplyUnlock())
                                break;
                            thing.thingProcess(thing.p, StatusType::REMOTE_ACTIVATION0_UNLOCK_DONE);

                            thing.thingProcess(thing.p, StatusType::SENSOR_ACTIVATION_READY);
                            state = Sensor::Instance().Active();
                            thing.thingProcess(thing.p, StatusType::SENSOR_ACTIVATION_DONE);
                        } while (false);
                    } else if (v.thingType == EverythingType::REMOTE_ACTIVATION1)
                    {
                        Log::Instance().Print<LogType::INFO>("ready for remote activation 1");
                        thing.thingExecute(thing.p, static_cast<Jint>(v.thingType));

                        do
                        {
                            thing.thingProcess(thing.p, StatusType::SENSOR_CHECK_READY);
                            Sensor::Instance().Check();
                            thing.thingProcess(thing.p, StatusType::SENSOR_CHECK_DONE);

                            thing.thingProcess(thing.p, StatusType::REMOTE_ACTIVATION1_UNLOCK_READY);
                            if (!RemoteActive1::Instance().SetSN(Environment::Instance().GetSN())
                                    .SetModel(Environment::Instance().GetDeviceModel())
                                    .SetCustomer(Environment::Instance().GetCustomer())
                                    .SetSubCustomer(Environment::Instance().GetSubCustomer())
                                    .SetHardwareVersion(Environment::Instance().GetHardwareVersion())
                                    .SetSoftwareVersion(Environment::Instance().GetSoftwareVersion())
                                    .ApplyUnlock())
                                break;
                            thing.thingProcess(thing.p, StatusType::REMOTE_ACTIVATION1_UNLOCK_DONE);

                            thing.thingProcess(thing.p, StatusType::REMOTE_ACTIVATION1_UNLOCK_READY);
                            state = Sensor::Instance().Active();
                            thing.thingProcess(thing.p, StatusType::REMOTE_ACTIVATION1_UNLOCK_DONE);
                        } while (false);
                    } else if (v.thingType == EverythingType::CUSTOMER_CHECK_UNLOCK)
                    {

                    } else if (v.thingType == EverythingType::GET_SP_TAMPERED_LOG)
                    {
                        Log::Instance().Print<LogType::INFO>("get the SP tampered log");
                        thing.thingExecute(thing.p, static_cast<Jint>(v.thingType));
                        state = SPTamperedLog::Instance().Process();
                    }

                    v.isUing = false;
                    thing.thingEnd(
                            thing.p,
                            static_cast<Jint>(v.thingType),
                            state,
                            (state ? ErrorsType::SUCCESS : Errors::Instance().GetErrorType())
                    );
                }

                std::this_thread::sleep_for(std::chrono::microseconds(200));
            }
        }).Process();
    }
};

}

#endif //SMARTTOOLS_SMART_ANDROID_EVERYTHING_HPP
