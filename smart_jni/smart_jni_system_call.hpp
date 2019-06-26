#pragma once

#ifndef SMARTTOOLS_SMART_JNI_SYSTEM_CALL_HPP
#define SMARTTOOLS_SMART_JNI_SYSTEM_CALL_HPP

#include "../kernel.hpp"
#include "../smart_utils/smart_utils_log.hpp"

#include <jni.h>

using smart::utils::log::Log;
using smart::utils::log::LogType;

#ifdef __cplusplus
extern "C"
{
#endif

JNIEXPORT jstring JNICALL Java_cn_smartpeak_tools_SmartSystem_AppVersion(JNIEnv *env, jclass self, jobject context)
{
    constexpr Jint PACKAGE_MANAGER_STATIC_GET_CONFIGURATIONS = 16384;

    constexpr Jchar CONTEXT_METHOD_GET_APPLICATION_CONTEXT[] = "getApplicationContext";
    constexpr Jchar CONTEXT_METHOD_GET_APPLICATION_CONTEXT_SIG[] = "()Landroid/content/Context;";
    constexpr Jchar CONTEXT_METHOD_GET_PACKAGE_MANAGER[] = "getPackageManager";
    constexpr Jchar CONTEXT_METHOD_GET_PACKAGE_MANAGER_SIG[] = "()Landroid/content/pm/PackageManager;";
    constexpr Jchar CONTEXT_METHOD_GET_PACKAGE_NAME[] = "getPackageName";
    constexpr Jchar CONTEXT_METHOD_GET_PACKAGE_NAME_SIG[] = "()Ljava/lang/String;";

    constexpr Jchar PACKAGE_MANAGER_METHOD_GET_PACKAGE_ARCHIVE_INFO[] = "getPackageInfo";
    constexpr Jchar PACKAGE_MANAGER_METHOD_GET_PACKAGE_ARCHIVE_INFO_SIG[] = "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;";

    constexpr Jchar PACKAGE_INFO_FIELD_VERSION_NAME[] = "versionName";
    constexpr Jchar PACKAGE_INFO_FIELD_VERSION_NAME_SIG[] = "Ljava/lang/String;";

    jclass ctxCls = nullptr;
    jobject ctxObj = nullptr;
    jmethodID getApplicationContext = nullptr;
    jmethodID getPackageManager = nullptr;
    jmethodID getPackageName = nullptr;

    jclass pmCls = nullptr;
    jobject pmObj = nullptr;
    jmethodID getPackageArchiveInfo = nullptr;

    jclass piCls = nullptr;
    jobject piObj = nullptr;
    jfieldID piVVersion = nullptr;

    if ((env == nullptr) || (context == nullptr))
        return nullptr;
    if (ctxCls = (*env).GetObjectClass(context);ctxCls == nullptr)
        return nullptr;

    if (getApplicationContext = (*env).GetMethodID(
            ctxCls,
            CONTEXT_METHOD_GET_APPLICATION_CONTEXT,
            CONTEXT_METHOD_GET_APPLICATION_CONTEXT_SIG
        );getApplicationContext == nullptr)
        return nullptr;
    if (getPackageManager = (*env).GetMethodID(
            ctxCls,
            CONTEXT_METHOD_GET_PACKAGE_MANAGER,
            CONTEXT_METHOD_GET_PACKAGE_MANAGER_SIG
        );getPackageManager == nullptr)
        return nullptr;
    if (getPackageName = (*env).GetMethodID(
            ctxCls,
            CONTEXT_METHOD_GET_PACKAGE_NAME,
            CONTEXT_METHOD_GET_PACKAGE_NAME_SIG
        );getPackageName == nullptr)
        return nullptr;

    if (ctxObj = (*env).CallObjectMethod(context, getApplicationContext);ctxObj == nullptr)
        return nullptr;
    if (pmObj = (*env).CallObjectMethod(ctxObj, getPackageManager);pmObj == nullptr)
        return nullptr;
    if (pmCls = (*env).GetObjectClass(pmObj);pmCls == nullptr)
        return nullptr;

    if (getPackageArchiveInfo = (*env).GetMethodID(
            pmCls,
            PACKAGE_MANAGER_METHOD_GET_PACKAGE_ARCHIVE_INFO,
            PACKAGE_MANAGER_METHOD_GET_PACKAGE_ARCHIVE_INFO_SIG
        );getPackageArchiveInfo == nullptr)
        return nullptr;

    if (piObj = (*env).CallObjectMethod(
            pmObj,
            getPackageArchiveInfo,
            (*env).CallObjectMethod(ctxObj, getPackageName),
            PACKAGE_MANAGER_STATIC_GET_CONFIGURATIONS
        );piObj == nullptr)
        return nullptr;

    if (piCls = (*env).GetObjectClass(piObj);piCls == nullptr)
        return nullptr;
    if (piVVersion = (*env).GetFieldID(
            piCls,
            PACKAGE_INFO_FIELD_VERSION_NAME,
            PACKAGE_INFO_FIELD_VERSION_NAME_SIG
        );piVVersion == nullptr)
        return nullptr;

    return reinterpret_cast<jstring>((*env).GetObjectField(piObj, piVVersion));
}

JNIEXPORT void JNICALL Java_cn_smartpeak_tools_SmartSystem_Reboot(JNIEnv *env, jclass self, jobject context)
{
    constexpr Jchar SMART_SYSTEM_CALL_POWER_MANAGER_NAME[] = "power";
    constexpr Jchar SMART_SYSTEM_CALL_REBOOT_NAME[] = "SecurityDefenseSystem2 make the device to reboot";

    constexpr Jchar CONTEXT_METHOD_GET_APPLICATION_CONTEXT[] = "getApplicationContext";
    constexpr Jchar CONTEXT_METHOD_GET_APPLICATION_CONTEXT_SIG[] = "()Landroid/content/Context;";
    constexpr Jchar CONTEXT_METHOD_GET_SYSTEM_SERVICE[] = "getSystemService";
    constexpr Jchar CONTENT_METHOD_GET_SYSTEM_SERVICE_SIG[] = "(Ljava/lang/String;)Ljava/lang/Object;";
    constexpr Jchar POWER_MANAGER_METHOD_REBOOT[] = "reboot";
    constexpr Jchar POWER_MANAGER_METHOD_REBOOT_SIG[] = "(Ljava/lang/String;)V";

    jclass ctxCls = nullptr;
    jobject ctxObj = nullptr;
    jmethodID getApplicationContext = nullptr;
    jmethodID getSystemService = nullptr;

    jobject powerManagerObj = nullptr;
    jclass powerManagerCls = nullptr;
    jmethodID reboot = nullptr;

    if ((env == nullptr) || (context == nullptr))
        return;
    if (ctxCls = (*env).GetObjectClass(context);ctxCls == nullptr)
        return;

    if (getApplicationContext = (*env).GetMethodID(
            ctxCls,
            CONTEXT_METHOD_GET_APPLICATION_CONTEXT,
            CONTEXT_METHOD_GET_APPLICATION_CONTEXT_SIG
        );getApplicationContext == nullptr)
        return;

    if (getSystemService = (*env).GetMethodID(
            ctxCls,
            CONTEXT_METHOD_GET_SYSTEM_SERVICE,
            CONTENT_METHOD_GET_SYSTEM_SERVICE_SIG
        );getSystemService == nullptr)
        return;

    if (ctxObj = (*env).CallObjectMethod(context, getApplicationContext);ctxObj == nullptr)
        return;

    if (powerManagerObj = (*env).CallObjectMethod(
            ctxObj,
            getSystemService,
            (*env).NewStringUTF(SMART_SYSTEM_CALL_POWER_MANAGER_NAME)
        );powerManagerObj == nullptr)
        return;

    (*env).DeleteLocalRef(ctxCls);
    (*env).DeleteLocalRef(ctxObj);
    if (powerManagerCls = (*env).GetObjectClass(powerManagerObj);powerManagerCls == nullptr)
        return;

    if (reboot = (*env).GetMethodID(
        powerManagerCls,
        POWER_MANAGER_METHOD_REBOOT,
        POWER_MANAGER_METHOD_REBOOT_SIG
    ), reboot == nullptr)
        return;

    (*env).CallVoidMethod(powerManagerObj, reboot, (*env).NewStringUTF(SMART_SYSTEM_CALL_REBOOT_NAME));
}

#ifdef __cplusplus
}
#endif

#endif //SMARTTOOLS_SMART_JNI_SYSTEM_CALL_HPP
