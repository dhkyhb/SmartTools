#ifndef SMARTTOOLS_SMART_ANDROID_REMOTE_ACTIVE_0_HPP
#define SMARTTOOLS_SMART_ANDROID_REMOTE_ACTIVE_0_HPP

#include "../kernel.hpp"

#include <APosSecurityManager.h>

namespace smart::android::remote::active0
{

constexpr Jint REMOTEACTIVE0_DAM_DATA_AREA_SN_SIZE = 32;
constexpr Jint REMOTEACTIVE0_DAM_DATA_AREA_CACHE_SIZE = 1024;

typedef struct
{
    Jint cacheLen;
    Jbyte cache[REMOTEACTIVE0_DAM_DATA_AREA_CACHE_SIZE];
    Jchar sn[REMOTEACTIVE0_DAM_DATA_AREA_SN_SIZE];
} RemoteActive0DAMDataArea;

typedef struct
{
    Jbyte stx;
    Jbyte lenH;
    Jbyte lenL;
    Jbyte crcH;
    Jbyte crcL;

    RemoteActive0DAMDataArea damDataArea;
} RemoteActive0DAM;

typedef struct
{
    Jint activeCodeLen;
    Jchar *activeCode;
    APosSecurityManager *context;

    std::mutex posMutex;
} POSSDKSupport;

constexpr Jint REMOTEACTIVE0_APPLY_ACTIVE_CODE_BY_SP_OF_MODE = 3;

// Old version, it's based on Socket to unlock request.
class RemoteActive0
{
public:
    static RemoteActive0 &Instance()
    {
        static RemoteActive0 *obj = nullptr;
        if (obj == nullptr)
            obj = new RemoteActive0();
        return (*obj);
    }

    Jbool ApplyUnlock()
    {
        Jbool state = false;

        do
        {
            if (!this->POSInit())
                break;
            if (!this->GetActiveCodeBySP())
                break;

            state = true;
        } while (false);

        this->PosRelease();
        return state;
    }

    RemoteActive0 &SetSN(const Jchar *sn)
    {
        Jint snLen = 0;

        memset(this->mRemoteActive0DAM.damDataArea.sn, 0, sizeof(this->mRemoteActive0DAM.damDataArea.sn));

        do
        {
            if (sn == nullptr)
                break;
            if (snLen = strlen(sn);snLen > sizeof(this->mRemoteActive0DAM.damDataArea.sn))
                break;

            memcpy(this->mRemoteActive0DAM.damDataArea.sn, sn, snLen);
        } while (false);

        return (*this);
    }

private:
    POSSDKSupport mPOSSDKSupport;
    RemoteActive0DAM mRemoteActive0DAM;

    RemoteActive0() :
        mPOSSDKSupport{},
        mRemoteActive0DAM{}
    {}

    Jbool POSInit()
    {
        this->mPOSSDKSupport.posMutex.lock();
        memset(&this->mPOSSDKSupport, 0, sizeof(this->mPOSSDKSupport));
        this->mPOSSDKSupport.context = APosSecurityManager_new();
        return (this->mPOSSDKSupport.context != nullptr);
    }

    void PosRelease()
    {
        if (this->mPOSSDKSupport.activeCode != nullptr)
            free(this->mPOSSDKSupport.activeCode);
        if (this->mPOSSDKSupport.context != nullptr)
            APosSecurityManager_delete(this->mPOSSDKSupport.context);

        this->mPOSSDKSupport.posMutex.unlock();
    }

    Jbool GetActiveCodeBySP()
    {
        if (APosSecurityManager_SysRemoteUnlockReq(
            this->mPOSSDKSupport.context,
            REMOTEACTIVE0_APPLY_ACTIVE_CODE_BY_SP_OF_MODE,
            &this->mPOSSDKSupport.activeCode,
            &this->mPOSSDKSupport.activeCodeLen
        ) == 0)
            return false;

        if (this->mPOSSDKSupport.activeCode == nullptr)
            return false;
        if (this->mPOSSDKSupport.activeCodeLen > sizeof(this->mRemoteActive0DAM.damDataArea.cache))
            return false;

        this->mRemoteActive0DAM.damDataArea.cacheLen = this->mPOSSDKSupport.activeCodeLen;

        memcpy(
            this->mRemoteActive0DAM.damDataArea.cache,
            &this->mPOSSDKSupport.activeCode[3],
            this->mRemoteActive0DAM.damDataArea.cacheLen
        );

        return true;
    }
};

}

#endif //SMARTTOOLS_SMART_ANDROID_REMOTE_ACTIVE_0_HPP
