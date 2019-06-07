#ifndef SMARTTOOLS_SMART_JNI_CONFIG_HPP
#define SMARTTOOLS_SMART_JNI_CONFIG_HPP

#include "../kernel.hpp"
#include "../smart_android/smart_android_config.hpp"

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

using smart::android::config::Config;

#ifdef __cplusplus
extern "C"
{
#endif

JNIEXPORT jstring JNICALL Java_cn_smartpeak_tools_Config_GetRemoteActivationAddress(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return nullptr;

    return (*env).NewStringUTF(Config::Instance().GetRemoteActivationAddress());
}

JNIEXPORT jint JNICALL Java_cn_smartpeak_tools_Config_GetRemoteActivationPort(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return 0;

    return Config::Instance().GetRemoteActivationPort();
}

JNIEXPORT jint JNICALL Java_cn_smartpeak_tools_Config_GetRemoteActivationTimeouts(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return 0;

    return Config::Instance().GetRemoteActivationTimeouts();
}

JNIEXPORT jstring JNICALL Java_cn_smartpeak_tools_Config_GetCustomerCheckAddress(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return nullptr;

    return env->NewStringUTF(Config::Instance().GetCustomerCheckAddress());
}

JNIEXPORT jint JNICALL Java_cn_smartpeak_tools_Config_GetCustomerCheckPort(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return 0;

    return Config::Instance().GetCustomerCheckPort();
}

JNIEXPORT jint JNICALL Java_cn_smartpeak_tools_Config_GetCustomerCheckTimeouts(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return 0;

    return Config::Instance().GetCustomerCheckTimeouts();
}

JNIEXPORT jint JNICALL Java_cn_smartpeak_tools_Config_GetModel(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return 0;

    return Config::Instance().GetModel();
}

JNIEXPORT jboolean JNICALL Java_cn_smartpeak_tools_Config_GetWhetherToOpenAudio(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return false;

    return Config::Instance().GetWhetherToOpenAudio();
}

JNIEXPORT jboolean JNICALL Java_cn_smartpeak_tools_Config_GetWhetherToOpenSignVerify(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return false;

    return Config::Instance().GetWhetherToOpenSignVerify();
}

JNIEXPORT jboolean JNICALL Java_cn_smartpeak_tools_Config_GetWhetherToOpenDeviceTampered(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return false;

    return Config::Instance().GetWhetherToOpenDeviceTampered();
}

JNIEXPORT jboolean JNICALL Java_cn_smartpeak_tools_Config_GetWhetherTheDeviceOnStartUpToBeCheckSensor(
    JNIEnv *env,
    jobject self
)
{
    if (env == nullptr)
        return false;

    return Config::Instance().GetWhetherTheDeviceOnStartUpToBeCheckSensor();
}

JNIEXPORT jboolean JNICALL Java_cn_smartpeak_tools_Config_GetWhetherToOpenCustomerCheck(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return false;

    return Config::Instance().GetWhetherToOpenCustomerCheck();
}

JNIEXPORT jstring JNICALL Java_cn_smartpeak_tools_Config_GetVersion(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return nullptr;

    return (*env).NewStringUTF(Config::Instance().GetVersion());
}

#ifdef __cplusplus
}
#endif

#endif //SMARTTOOLS_SMART_JNI_CONFIG_HPP
