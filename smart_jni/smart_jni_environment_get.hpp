#pragma once

#ifndef SMARTTOOLS_SMART_JNI_ENVIRONMENT_GET_HPP
#define SMARTTOOLS_SMART_JNI_ENVIRONMENT_GET_HPP

#include "../kernel.hpp"
#include "../smart_android/smart_android_environment.hpp"

#include <jni.h>

using smart::android::environment::Environment;

#ifdef __cplusplus
extern "C"
{
#endif

JNIEXPORT jstring JNICALL Java_cn_smartpeak_tools_SmartEnvironment_GetSN(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return nullptr;
    return (*env).NewStringUTF(Environment::Instance().GetSN());
}

JNIEXPORT jstring JNICALL Java_cn_smartpeak_tools_SmartEnvironment_GetDeviceModel(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return nullptr;
    return (*env).NewStringUTF(Environment::Instance().GetDeviceModel());
}

JNIEXPORT jstring JNICALL Java_cn_smartpeak_tools_SmartEnvironment_GetOldCustomer(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return nullptr;
    return (*env).NewStringUTF(Environment::Instance().GetOldCustomer());
}

JNIEXPORT jstring JNICALL Java_cn_smartpeak_tools_SmartEnvironment_GetOldSubCustomer(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return nullptr;
    return (*env).NewStringUTF(Environment::Instance().GetOldSubCustomer());
}

JNIEXPORT jstring JNICALL Java_cn_smartpeak_tools_SmartEnvironment_GetCustomer(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return nullptr;
    return (*env).NewStringUTF(Environment::Instance().GetCustomer());
}

JNIEXPORT jstring JNICALL Java_cn_smartpeak_tools_SmartEnvironment_GetSubCustomer(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return nullptr;
    return (*env).NewStringUTF(Environment::Instance().GetSubCustomer());
}

JNIEXPORT jstring JNICALL Java_cn_smartpeak_tools_SmartEnvironment_GetHardwareVersion(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return nullptr;
    return (*env).NewStringUTF(Environment::Instance().GetHardwareVersion());
}

JNIEXPORT jstring JNICALL Java_cn_smartpeak_tools_SmartEnvironment_GetSoftwareVersion(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return nullptr;
    return (*env).NewStringUTF(Environment::Instance().GetSoftwareVersion());
}

JNIEXPORT jstring JNICALL Java_cn_smartpeak_tools_SmartEnvironment_GetSPVersion(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return nullptr;
    return (*env).NewStringUTF(Environment::Instance().GetSPVersion());
}

#ifdef __cplusplus
}
#endif

#endif //SMARTTOOLS_SMART_JNI_ENVIRONMENT_GET_HPP
