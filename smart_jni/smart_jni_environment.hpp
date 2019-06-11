#ifndef SMARTTOOLS_SMART_JNI_ENVIRONMENT_HPP
#define SMARTTOOLS_SMART_JNI_ENVIRONMENT_HPP

#include "../kernel.hpp"
#include "../smart_utils/smart_utils_log.hpp"
#include "../smart_android/smart_android_environment.hpp"

#include "smart_jni_config.hpp"
#include "smart_jni_thread_loop.hpp"

#include <jni.h>

using smart::utils::log::Log;
using smart::utils::log::LogType;
using smart::android::environment::Environment;

#ifdef __cplusplus
extern "C"
{
#endif

JNIEXPORT jobject JNICALL Java_cn_smartpeak_tools_Environment_SetAsset(JNIEnv *env, jobject self, jobject assetContex)
{
    constexpr Jint CONFIG_READ_CACHE_SIZE = 200;
    constexpr Jchar CONFIG_SYSTEM_DEFAULT_PATH[] = "/system/etc/app/cn.basewin.security.defense.system.xml";
    constexpr Jchar CONFIG_APP_DEFAULT_PATH[] = "config";
    constexpr Jchar CONFIG_APP_DEFAULT_CA_CERT_PATH[] = "ca.crt";

    Jint ret = 0;
    Jchar readCache[CONFIG_READ_CACHE_SIZE];

    FILE *sysConfFile = nullptr;
    AAssetManager *assetManager = nullptr;
    AAsset *asset = nullptr;

    if ((env == nullptr) || (assetContex == nullptr))
        return self;

    Config::Instance().Reset();

    do
    {
        if (sysConfFile = fopen(CONFIG_SYSTEM_DEFAULT_PATH, "rb");sysConfFile == nullptr)
            break;

        do
        {
            ret = fread(readCache, 1, sizeof(readCache), sysConfFile);
            Config::Instance().WriteBuffer(readCache, ret);
        } while (ret == static_cast<Jint>(sizeof(readCache)));

        Log::Instance().Print<LogType::DEBUG>("read from system");
    } while (false);

    do
    {
        if (sysConfFile != nullptr)
            break;
        if (assetManager = AAssetManager_fromJava(env, assetContex);assetManager == nullptr)
            break;

        if (asset = AAssetManager_open(
                    assetManager,
                    CONFIG_APP_DEFAULT_PATH,
                    AASSET_MODE_UNKNOWN
            );asset == nullptr)
            break;

        Config::Instance().WriteBuffer(
                reinterpret_cast<Jchar *>(const_cast<void *>(AAsset_getBuffer(asset))),
                AAsset_getLength(asset)
        );

        AAsset_close(asset);

        if (asset = AAssetManager_open(
                    assetManager,
                    CONFIG_APP_DEFAULT_CA_CERT_PATH,
                    AASSET_MODE_UNKNOWN
            );asset == nullptr)
            break;

        Environment::Instance().SetCustomerCheckCert(
                reinterpret_cast<Jchar *>(const_cast<void *>(AAsset_getBuffer(asset)))
        );
    } while (false);

    if (sysConfFile != nullptr)
        fclose(sysConfFile);
    if (asset != nullptr)
        AAsset_close(asset);

    if (Config::Instance().Process())
        Log::Instance().Print<LogType::INFO>("configure is ready");
    else
        Log::Instance().Print<LogType::INFO>("configure not is ready");

    Config::Instance().Test();
    return self;
}

JNIEXPORT jobject JNICALL Java_cn_smartpeak_tools_Environment_SetSN(JNIEnv *env, jobject self, jstring sn)
{
    const Jchar *cSN = nullptr;

    if ((env == nullptr) || (sn == nullptr))
        return self;
    if (cSN = (*env).GetStringUTFChars(sn, JNI_FALSE);cSN == nullptr)
        return self;

    Environment::Instance().SetSN(cSN);
    (*env).ReleaseStringUTFChars(sn, cSN);
    return self;
}

JNIEXPORT jobject JNICALL Java_cn_smartpeak_tools_Environment_SetThreadLoopSize(JNIEnv *env, jobject self, jint size)
{
    if ((env == nullptr) || (size < 1))
        return self;

    ThreadLoop::Instance().Init(size);
    return self;
}

JNIEXPORT jobject JNICALL Java_cn_smartpeak_tools_Environment_SetDeviceModel(JNIEnv *env, jobject self, jstring model)
{
    const Jchar *cModel = nullptr;

    if ((env == nullptr) || (model == nullptr))
        return self;
    if (cModel = (*env).GetStringUTFChars(model, JNI_FALSE);cModel == nullptr)
        return self;

    Environment::Instance().SetDeviceModel(cModel);
    (*env).ReleaseStringUTFChars(model, cModel);
    return self;
}

JNIEXPORT jobject JNICALL Java_cn_smartpeak_tools_Environment_SetOldCustomer(
        JNIEnv *env,
        jobject self,
        jstring oldCustomer
)
{
    const Jchar *cOldCustomer = nullptr;

    if ((env == nullptr) || (oldCustomer == nullptr))
        return self;
    if (cOldCustomer = (*env).GetStringUTFChars(oldCustomer, JNI_FALSE);cOldCustomer == nullptr)
        return self;

    Environment::Instance().SetOldCustomer(cOldCustomer);
    (*env).ReleaseStringUTFChars(oldCustomer, cOldCustomer);
    return self;
}

JNIEXPORT jobject JNICALL Java_cn_smartpeak_tools_Environment_SetOldSubCustomer(
        JNIEnv *env,
        jobject self,
        jstring oldSubCustomer
)
{
    const Jchar *cOldSubCustomer = nullptr;

    if ((env == nullptr) || (oldSubCustomer == nullptr))
        return self;
    if (cOldSubCustomer = (*env).GetStringUTFChars(oldSubCustomer, JNI_FALSE);cOldSubCustomer == nullptr)
        return self;

    Environment::Instance().SetOldSubCustomer(cOldSubCustomer);
    (*env).ReleaseStringUTFChars(oldSubCustomer, cOldSubCustomer);
    return self;
}

JNIEXPORT jobject JNICALL Java_cn_smartpeak_tools_Environment_SetAndroidVersion(
        JNIEnv *env,
        jobject self,
        jstring androidVersion
)
{
    const Jchar *cAndroidVersion = nullptr;

    if ((env == nullptr) || (androidVersion == nullptr))
        return self;
    if (cAndroidVersion = (*env).GetStringUTFChars(androidVersion, JNI_FALSE);cAndroidVersion == nullptr)
        return self;

    Environment::Instance().SetAndroidVersion(cAndroidVersion);
    (*env).ReleaseStringUTFChars(androidVersion, cAndroidVersion);
    return self;
}

JNIEXPORT jobject JNICALL Java_cn_smartpeak_tools_Environment_SetAndroidSDKVersion(
        JNIEnv *env,
        jobject self,
        jstring androidSDKVersion
)
{
    const Jchar *cAndroidSDKVersion = nullptr;

    if ((env == nullptr) || (androidSDKVersion == nullptr))
        return self;
    if (cAndroidSDKVersion = (*env).GetStringUTFChars(androidSDKVersion, JNI_FALSE);cAndroidSDKVersion == nullptr)
        return self;

    Environment::Instance().SetAndroidSDKVersion(cAndroidSDKVersion);
    (*env).ReleaseStringUTFChars(androidSDKVersion, cAndroidSDKVersion);
    return self;
}

JNIEXPORT jobject JNICALL Java_cn_smartpeak_tools_Environment_SetAndroidID(
        JNIEnv *env,
        jobject self,
        jstring androidID
)
{
    const Jchar *cAndroidID = nullptr;

    if ((env == nullptr) || (androidID == nullptr))
        return self;
    if (cAndroidID = (*env).GetStringUTFChars(androidID, JNI_FALSE);cAndroidID == nullptr)
        return self;

    Environment::Instance().SetAndroidID(cAndroidID);
    (*env).ReleaseStringUTFChars(androidID, cAndroidID);
    return self;
}

JNIEXPORT jobject JNICALL Java_cn_smartpeak_tools_Environment_SetAndroidDevice(
        JNIEnv *env,
        jobject self,
        jstring androidDevice
)
{
    const Jchar *cAndroidDevice = nullptr;

    if ((env == nullptr) || (androidDevice == nullptr))
        return self;
    if (cAndroidDevice = (*env).GetStringUTFChars(androidDevice, JNI_FALSE);cAndroidDevice == nullptr)
        return self;

    Environment::Instance().SetAndroidDevice(cAndroidDevice);
    (*env).ReleaseStringUTFChars(androidDevice, cAndroidDevice);
    return self;
}

JNIEXPORT jobject JNICALL Java_cn_smartpeak_tools_Environment_SetAndroidBootloader(
        JNIEnv *env,
        jobject self,
        jstring androidBootloader
)
{
    const Jchar *cAndroidBootloader = nullptr;

    if ((env == nullptr) || (androidBootloader == nullptr))
        return self;
    if (cAndroidBootloader = (*env).GetStringUTFChars(androidBootloader, JNI_FALSE);cAndroidBootloader == nullptr)
        return self;

    Environment::Instance().SetAndroidBootloader(cAndroidBootloader);
    (*env).ReleaseStringUTFChars(androidBootloader, cAndroidBootloader);
    return self;
}

#ifdef __cplusplus
}
#endif

#endif //SMARTTOOLS_SMART_JNI_ENVIRONMENT_HPP
