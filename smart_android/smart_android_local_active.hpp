#pragma once

#ifndef SMARTTOOLS_SMART_ANDROID_LOCAL_ACTIVE_HPP
#define SMARTTOOLS_SMART_ANDROID_LOCAL_ACTIVE_HPP

#include "../kernel.hpp"
#include "../smart_utils/smart_utils_log.hpp"

#include <APosSecurityManager.h>

namespace smart::android::local::active
{

using kernel::ItType;
using smart::utils::log::Log;
using smart::utils::log::LogType;

using LocalActiveCreateCall = void (*)(void *p);
using LocalActiveUsingCall = void (*)(void *p, Jint key);
using LocalActiveFinishCall = void (*)(void *p, Jbool state);

constexpr Jint LOCAL_ACTIVE_ATTR_KEY_VALUE_SIZE = 104;

typedef struct
{
    Jbool resetPWD;

    void *createP;
    void *usingP;
    void *finishP;
    LocalActiveCreateCall createCall;
    LocalActiveUsingCall usingCall;
    LocalActiveFinishCall finishCall;

    Jbyte keyView[LOCAL_ACTIVE_ATTR_KEY_VALUE_SIZE];
} LocalActiveAttr;

typedef struct
{
    APosSecurityManager *ctx;
} LocalActivePosSecurityManager;

class LocalActive
{
public:
    static LocalActive &Instance()
    {
        static LocalActive *obj = nullptr;
        if (obj == nullptr)
            obj = new LocalActive();
        return (*obj);
    }

    Jbool Register()
    {
        if (!this->PosInit())
            return false;
        return this->PosRegister();
    }

    LocalActive &Init()
    {
        memset(&this->mLocalActiveAttr, 0, sizeof(this->mLocalActiveAttr));
        this->PosRelease();
        return (*this);
    }

    template<typename CreateView>
    LocalActive &SetCreate(void *p, ItType<CreateView> v)
    {
        this->mLocalActiveAttr.createP = p;
        this->mLocalActiveAttr.createCall = v;
        return (*this);
    }

    template<typename UsingView>
    LocalActive &SetUsing(void *p, ItType<UsingView> v)
    {
        this->mLocalActiveAttr.usingP = p;
        this->mLocalActiveAttr.usingCall = v;
        return (*this);
    }

    template<typename FinishView>
    LocalActive &SetFinish(void *p, ItType<FinishView> v)
    {
        this->mLocalActiveAttr.finishP = p;
        this->mLocalActiveAttr.finishCall = v;
        return (*this);
    }

    void SetKeyPosition(Jbyte *v, Jint vLen)
    {
        if ((v == nullptr) || (vLen < 1) || (vLen > static_cast<Jint>(sizeof(this->mLocalActiveAttr.keyView))))
            return;

        memcpy(this->mLocalActiveAttr.keyView, v, vLen);
    }

private:
    LocalActiveAttr mLocalActiveAttr;
    LocalActivePosSecurityManager mLocalActivePosSecurityManager;

    LocalActive() :
        mLocalActiveAttr{},
        mLocalActivePosSecurityManager{}
    {}

    Jbool PosInit()
    {
        memset(&this->mLocalActivePosSecurityManager, 0, sizeof(this->mLocalActivePosSecurityManager));
        this->mLocalActivePosSecurityManager.ctx = APosSecurityManager_new();
        return (this->mLocalActivePosSecurityManager.ctx != nullptr);
    }

    void PosRelease()
    {
        if (this->mLocalActivePosSecurityManager.ctx != nullptr)
            APosSecurityManager_delete(this->mLocalActivePosSecurityManager.ctx);
    }

    Jbool PosRegister()
    {
        if (APosSecurityManager_PedKeyManage(
            this->mLocalActivePosSecurityManager.ctx,
            PED_KEY_MANAGE_CMD_UNLOCK_TERMINAL_KEY
        ) != 0)
            return false;

        APosSecurityManager_setCallbacks(
            this->mLocalActivePosSecurityManager.ctx,
            [](APosSecurityManager *mgr, Jint what, Jint extra, void *p) {
                // Info call
            },
            [](APosSecurityManager *mgr, Jint what, Jint extra, void *p) {
                // Error call
                auto &&attr = *reinterpret_cast<LocalActiveAttr *>(p);

                if (attr.finishCall != nullptr)
                    attr.finishCall(attr.finishP, false);
            },
            [](APosSecurityManager *mgr, const Jchar *keys, Jint keysLen, Jint timeoutsMs, void *p) {
                // Keyboard show
                auto &&attr = *reinterpret_cast<LocalActiveAttr *>(p);

                if (attr.createCall != nullptr)
                    attr.createCall(attr.createP);

                APosSecurityManager_PedSetKeyLayout(
                    mgr,
                    reinterpret_cast<Jchar *>(attr.keyView),
                    sizeof(attr.keyView),
                    0
                );
            },
            [](APosSecurityManager *mgr, Jint numKeys, void *p) {
                // Keyboard input
                auto &&attr = *reinterpret_cast<LocalActiveAttr *>(p);

                if (attr.usingCall != nullptr)
                    attr.usingCall(attr.usingP, numKeys);
            },
            [](APosSecurityManager *mgr, Jint cmd, const Jchar *rspBuf, Jint rspBufLen, void *p) {
                // PED pinblock result
            },
            [](APosSecurityManager *mgr, Jint cmd, const Jchar *rspBuf, Jint rspBufLen, void *p) {
                // PED verify pin
            },
            [](APosSecurityManager *mgr, Jint ret, Jint checkResult, void *p) {
                // Hardware self check result
            },
            [](APosSecurityManager *mgr, Jint triggered, const Jchar *sensorValue, Jint sensorValueLen,
               const Jchar *triggerTime, Jint triggerTimeLen, void *p) {
                // Hardware sensor triggered
                auto &&attr = *reinterpret_cast<LocalActiveAttr *>(p);

                if (attr.finishCall != nullptr)
                    attr.finishCall(attr.finishP, false);
            },
            [](APosSecurityManager *mgr, Jint ret, void *p) {
                // PED key manage result
                auto &&attr = *reinterpret_cast<LocalActiveAttr *>(p);
                if (ret == 0)
                {
                    if (attr.resetPWD)
                    {
                        if (attr.createCall != nullptr)
                            attr.createCall(attr.createP);

                        attr.resetPWD = false;
                    } else
                    {
                        if (attr.finishCall != nullptr)
                            attr.finishCall(attr.finishP, true);

                        APosSecurityManager_PedCancelPinBlock(mgr);
                    }
                } else if (ret == PED_KEY_MANAGE_ERROR_CHANGE_PASSWD)
                {
                    // Reset password in here
                    attr.resetPWD = true;
                    if (attr.createCall != nullptr)
                        attr.createCall(attr.createP);
                } else if (ret == PED_KEY_MANAGE_ERROR_PASSWD)
                {
                    // You enter the wrong password
                    if (attr.finishCall != nullptr)
                        attr.finishCall(attr.finishP, false);

                    APosSecurityManager_PedCancelPinBlock(mgr);
                } else if (ret == PED_KEY_MANAGE_ERROR_TRY_PASSWD_EXCEED)
                {
                    // You enter the wrong password more than the maximum number of times.
                    if (attr.finishCall != nullptr)
                        attr.finishCall(attr.finishP, false);

                    APosSecurityManager_PedCancelPinBlock(mgr);
                }
            },
            &this->mLocalActiveAttr
        );

        return true;
    }
};

}

#endif //SMARTTOOLS_SMART_ANDROID_LOCAL_ACTIVE_HPP
