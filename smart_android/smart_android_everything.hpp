#ifndef SMARTTOOLS_SMART_ANDROID_EVERYTHING_HPP
#define SMARTTOOLS_SMART_ANDROID_EVERYTHING_HPP

#include "../kernel.hpp"
#include "../smart_utils/smart_utils_thread_loop.hpp"
#include "../smart_resouces/smart_resouces_status.hpp"
#include "../smart_resouces/smart_resouces_errors.hpp"
#include "../smart_resouces/smart_resouces_android_files_operator.hpp"

#include "smart_android_environment.hpp"
#include "smart_android_sensor.hpp"
#include "smart_android_remote_active_0.hpp"
#include "smart_android_remote_active_1.hpp"
#include "smart_android_customer_check_unlock.hpp"
#include "smart_android_backup_tampered_information.hpp"
#include "smart_android_sp_tampered_log.hpp"

namespace smart::android::everything
{

using kernel::ItType;
using smart::utils::thread::loop::ThreadLoop;
using smart::resouces::status::StatusType;
using smart::resouces::errors::Errors;
using smart::resouces::errors::ErrorsType;
using smart::resouces::android::files::AndroidFiles;
using smart::resouces::android::files::AndroidFilesType;
using smart::resouces::android::files::AndroidFilesOperation;

using smart::android::environment::Environment;
using smart::android::sensor::Sensor;
using smart::android::remote::active0::RemoteActive0;
using smart::android::remote::active1::RemoteActive1;
using smart::android::customer::check::unlock::CustomerCheckUnlock;
using smart::android::backup::tampered::information::BackupTamperedInformation;
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
    IF_NOTIFY_TAMPERED_USE_IT,
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

                    thing.thingExecute(thing.p, static_cast<Jint>(v.thingType));
                    if (v.thingType == EverythingType::IF_NOTIFY_TAMPERED_USE_IT)
                    {
                        Log::Instance().Print<LogType::INFO>("check the device tampered");
                        while (!Sensor::Instance().Check());
                        state = true;
                    } else if (v.thingType == EverythingType::REMOTE_ACTIVATION0)
                    {
                        Log::Instance().Print<LogType::INFO>("ready for remote activation");

                        do
                        {
                            thing.thingProcess(thing.p, StatusType::SENSOR_CHECK_READY);
                            Sensor::Instance().Check();
                            thing.thingProcess(thing.p, StatusType::SENSOR_CHECK_DONE);

                            thing.thingProcess(thing.p, StatusType::REMOTE_CLEAR_SOURCES_READY);
                            BackupTamperedInformation::Instance().SetSN(Environment::Instance().GetSN())
                                .SetModel(Environment::Instance().GetDeviceModel())
                                .SetCustomer(Environment::Instance().GetCustomer())
                                .SetSubCustomer(Environment::Instance().GetSubCustomer())
                                .SetHardwareVersion(Environment::Instance().GetHardwareVersion())
                                .SetSoftwareVersion(Environment::Instance().GetSoftwareVersion())
                                .SetAndroidVersion(Environment::Instance().GetAndroidVersion())
                                .SetAndroidSDKVersion(Environment::Instance().GetAndroidSDKVersion())
                                .SetAndroidID(Environment::Instance().GetAndroidID())
                                .SetSPVersion(Environment::Instance().GetSPVersion())
                                .SetAndroidDevice(Environment::Instance().GetAndroidDevice())
                                .SetAndroidBootloader(Environment::Instance().GetAndroidBootloader())
                                .SetCert(Environment::Instance().GetCustomerCheckCert())
                                .SetAddress(Config::Instance().GetCustomerCheckAddress())
                                .SetPort(Config::Instance().GetCustomerCheckPort())
                                .UploadTamperInformation();
                            thing.thingProcess(thing.p, StatusType::REMOTE_CLEAR_SOURCES_DONE);

                            thing.thingProcess(thing.p, StatusType::REMOTE_ACTIVATION0_UNLOCK_READY);
                            if (!RemoteActive0::Instance().SetSN(Environment::Instance().GetSN())
                                .SetAddress(Config::Instance().GetRemoteActivationAddress())
                                .SetPort(Config::Instance().GetRemoteActivationPort())
                                .SetTimeouts(Config::Instance().GetRemoteActivationTimeouts())
                                .ApplyUnlock())
                                break;
                            thing.thingProcess(thing.p, StatusType::REMOTE_ACTIVATION0_UNLOCK_DONE);

                            thing.thingProcess(thing.p, StatusType::SENSOR_ACTIVATION_READY);
                            state = Sensor::Instance().Active();
                            thing.thingProcess(thing.p, StatusType::SENSOR_ACTIVATION_DONE);
                        } while (false);
                    } else if (v.thingType == EverythingType::REMOTE_ACTIVATION1)
                    {
                        Log::Instance().Print<LogType::INFO>("ready for remote activation 1");

                        do
                        {
                            thing.thingProcess(thing.p, StatusType::SENSOR_CHECK_READY);
                            Sensor::Instance().Check();
                            thing.thingProcess(thing.p, StatusType::SENSOR_CHECK_DONE);

                            thing.thingProcess(thing.p, StatusType::REMOTE_CLEAR_SOURCES_READY);
                            BackupTamperedInformation::Instance().SetSN(Environment::Instance().GetSN())
                                .SetModel(Environment::Instance().GetDeviceModel())
                                .SetCustomer(Environment::Instance().GetCustomer())
                                .SetSubCustomer(Environment::Instance().GetSubCustomer())
                                .SetHardwareVersion(Environment::Instance().GetHardwareVersion())
                                .SetSoftwareVersion(Environment::Instance().GetSoftwareVersion())
                                .SetAndroidVersion(Environment::Instance().GetAndroidVersion())
                                .SetAndroidSDKVersion(Environment::Instance().GetAndroidSDKVersion())
                                .SetAndroidID(Environment::Instance().GetAndroidID())
                                .SetSPVersion(Environment::Instance().GetSPVersion())
                                .SetAndroidDevice(Environment::Instance().GetAndroidDevice())
                                .SetAndroidBootloader(Environment::Instance().GetAndroidBootloader())
                                .SetCert(Environment::Instance().GetCustomerCheckCert())
                                .SetAddress(Config::Instance().GetCustomerCheckAddress())
                                .SetPort(Config::Instance().GetCustomerCheckPort())
                                .UploadTamperInformation();
                            thing.thingProcess(thing.p, StatusType::REMOTE_CLEAR_SOURCES_DONE);

                            thing.thingProcess(thing.p, StatusType::REMOTE_ACTIVATION1_UNLOCK_READY);
                            if (!RemoteActive1::Instance().SetSN(Environment::Instance().GetSN())
                                .SetModel(Environment::Instance().GetDeviceModel())
                                .SetCustomer(Environment::Instance().GetCustomer())
                                .SetSubCustomer(Environment::Instance().GetSubCustomer())
                                .SetHardwareVersion(Environment::Instance().GetHardwareVersion())
                                .SetSoftwareVersion(Environment::Instance().GetSoftwareVersion())
                                .SetAddress(Config::Instance().GetRemoteActivationAddress())
                                .SetPort(Config::Instance().GetRemoteActivationPort())
                                .SetTimeouts(Config::Instance().GetRemoteActivationTimeouts())
                                .ApplyUnlock())
                                break;
                            thing.thingProcess(thing.p, StatusType::REMOTE_ACTIVATION1_UNLOCK_DONE);

                            thing.thingProcess(thing.p, StatusType::SENSOR_ACTIVATION_READY);
                            state = Sensor::Instance().Active();
                            thing.thingProcess(thing.p, StatusType::SENSOR_ACTIVATION_DONE);
                        } while (false);
                    } else if (v.thingType == EverythingType::CUSTOMER_CHECK_UNLOCK)
                    {
                        Log::Instance().Print<LogType::INFO>("ready for customer check unlock");

                        do
                        {
                            thing.thingProcess(thing.p, StatusType::CUSTOMER_CHECK_UNLOCK_READY);
                            if (state = CustomerCheckUnlock::Instance().SetSN(Environment::Instance().GetSN())
                                    .SetModel(Environment::Instance().GetDeviceModel())
                                    .SetCustomer(Environment::Instance().GetCustomer())
                                    .SetSubCustomer(Environment::Instance().GetSubCustomer())
                                    .SetOldCustomer(Environment::Instance().GetOldCustomer())
                                    .SetOldSubCustomer(Environment::Instance().GetOldSubCustomer())
                                    .SetSoftwareVersion(Environment::Instance().GetSoftwareVersion())
                                    .SetHardwareVersion(Environment::Instance().GetHardwareVersion())
                                    .SetClientCert(Environment::Instance().GetCustomerCheckCert())
                                    .SetAddress(Config::Instance().GetCustomerCheckAddress())
                                    .SetPort(Config::Instance().GetCustomerCheckPort())
                                    .SetTimeouts(Config::Instance().GetCustomerCheckTimeouts())
                                    .ApplyUnlock();!state)
                                break;
                            thing.thingProcess(thing.p, StatusType::CUSTOMER_CHECK_UNLOCK_DONE);

                            thing.thingProcess(thing.p, StatusType::RESOUCE_DELETE_CUSTOMER_READY);
                            AndroidFiles::Instance().Ready()
                                .SetType<AndroidFilesType::CUSTOMER>()
                                .SetOperation<AndroidFilesOperation::DELETE>()
                                .Execute();
                            thing.thingProcess(thing.p, StatusType::RESOUCE_DELETE_CUSTOMER_DONE);

                            thing.thingProcess(thing.p, StatusType::RESOUCE_DELETE_SUB_CUSTOMER_READY);
                            AndroidFiles::Instance().Ready()
                                .SetType<AndroidFilesType::SUB_CUSTOMER>()
                                .SetOperation<AndroidFilesOperation::DELETE>()
                                .Execute();
                            thing.thingProcess(thing.p, StatusType::RESOUCE_DELETE_SUB_CUSTOMER_DONE);
                        } while (false);
                    } else if (v.thingType == EverythingType::GET_SP_TAMPERED_LOG)
                    {
                        Log::Instance().Print<LogType::INFO>("get the SP tampered log");
                        thing.thingProcess(thing.p, StatusType::SENSOR_LOG_READER_READY);
                        state = SPTamperedLog::Instance().Process();
                        thing.thingProcess(thing.p, StatusType::SENSOR_LOG_READER_DONE);
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
