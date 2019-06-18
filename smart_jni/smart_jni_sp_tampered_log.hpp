#pragma once

#ifndef SMARTTOOLS_SMART_JNI_SP_TAMPERED_LOG_HPP
#define SMARTTOOLS_SMART_JNI_SP_TAMPERED_LOG_HPP

#include "../kernel.hpp"
#include "../smart_android/smart_android_sp_tampered_log.hpp"

#include <jni.h>

using smart::android::sp::tampered::log::SPTamperedLog;

#ifdef __cplusplus
extern "C"
{
#endif

JNIEXPORT jobjectArray JNICALL Java_cn_smartpeak_tools_SmartSPRecordLog_GetRecords(JNIEnv *env, jobject self)
{
    constexpr Jchar JNI_OBJ_INIT_METHOD[] = "<init>";
    constexpr Jchar JNI_OBJ_INIT_METHOD_PARAMETER[] = "()V";
    constexpr Jchar JNI_FIELD_INT_TYPE[] = "I";
    constexpr Jchar JNI_FIELD_STRING_TYPE[] = "Ljava/lang/String;";

    constexpr Jchar SP_RECORD_LOG_RECORD_CLASS_PATH[] = "cn/smartpeak/tools/SmartSPRecordLog$Record";
    constexpr Jchar SP_RECORD_LOG_RECORD_CLASS_INIT_METHOD_PATH[] = "(Lcn/smartpeak/tools/SmartSPRecordLog;)V";

    constexpr Jchar SP_RECORD_LOG_RECORD_FIELD_TIME[] = "mTime";
    constexpr Jchar SP_RECORD_LOG_RECORD_FIELD_CATEGORY[] = "mCategory";
    constexpr Jchar SP_RECORD_LOG_RECORD_FIELD_TYPE[] = "mType";

    Jint i = 0;
    jobjectArray SPRecordLogRecordArr = nullptr;

    jclass SPRecordLogOfSonCls = nullptr;
    jobject SPRecordLogOfSonObj = nullptr;
    jmethodID SPRecordLogRecordOfInit = nullptr;
    jfieldID SPRecordLogRecordPTime = nullptr;
    jfieldID SPRecordLogRecordPCategory = nullptr;
    jfieldID SPRecordLogRecordPType = nullptr;

    jclass SPRecordLogCls = nullptr;
    jobject SPRecordLogObj = nullptr;
    jmethodID SPRecordLogOfInit = nullptr;

    if (env == nullptr)
        return nullptr;
    if (SPRecordLogCls = (*env).GetObjectClass(self);SPRecordLogCls == nullptr)
        return nullptr;
    if (SPRecordLogOfSonCls = (*env).FindClass(SP_RECORD_LOG_RECORD_CLASS_PATH);SPRecordLogOfSonCls == nullptr)
        return nullptr;

    if (SPRecordLogRecordArr = (*env).NewObjectArray(
            SPTamperedLog::Instance().GetRecordLength(),
            SPRecordLogOfSonCls,
            nullptr
        );SPRecordLogRecordArr == nullptr)
        return nullptr;

    if (SPRecordLogOfInit = (*env).GetMethodID(
            SPRecordLogCls,
            JNI_OBJ_INIT_METHOD,
            JNI_OBJ_INIT_METHOD_PARAMETER
        );SPRecordLogOfInit == nullptr)
        return nullptr;

    if (SPRecordLogRecordOfInit = (*env).GetMethodID(
            SPRecordLogOfSonCls,
            JNI_OBJ_INIT_METHOD,
            SP_RECORD_LOG_RECORD_CLASS_INIT_METHOD_PATH
        );SPRecordLogRecordOfInit == nullptr)
        return nullptr;

    if (SPRecordLogRecordPTime = (*env).GetFieldID(
            SPRecordLogOfSonCls,
            SP_RECORD_LOG_RECORD_FIELD_TIME,
            JNI_FIELD_STRING_TYPE
        );SPRecordLogRecordPTime == nullptr)
        return nullptr;

    if (SPRecordLogRecordPCategory = (*env).GetFieldID(
            SPRecordLogOfSonCls,
            SP_RECORD_LOG_RECORD_FIELD_CATEGORY,
            JNI_FIELD_INT_TYPE
        );SPRecordLogRecordPCategory == nullptr)
        return nullptr;

    if (SPRecordLogRecordPType = (*env).GetFieldID(
            SPRecordLogOfSonCls,
            SP_RECORD_LOG_RECORD_FIELD_TYPE,
            JNI_FIELD_INT_TYPE
        );SPRecordLogRecordPType == nullptr)
        return nullptr;

    if (SPRecordLogObj = (*env).NewObject(SPRecordLogCls, SPRecordLogOfInit);SPRecordLogObj == nullptr)
        return nullptr;

    for (i = 0; i < SPTamperedLog::Instance().GetRecordLength(); ++i)
    {
        if (SPRecordLogOfSonObj = (*env).NewObject(
                SPRecordLogOfSonCls,
                SPRecordLogRecordOfInit,
                SPRecordLogObj
            );SPRecordLogOfSonObj == nullptr)
            continue;

        do
        {
            auto &&mTimeStr = (*env).NewStringUTF(SPTamperedLog::Instance().GetRecords()[i].time);
            if (mTimeStr == nullptr)
                break;

            (*env).SetObjectField(SPRecordLogOfSonObj, SPRecordLogRecordPTime, mTimeStr);
            (*env).SetIntField(
                SPRecordLogOfSonObj,
                SPRecordLogRecordPType,
                SPTamperedLog::Instance().GetRecords()[i].type
            );
            (*env).SetIntField(
                SPRecordLogOfSonObj,
                SPRecordLogRecordPCategory,
                static_cast<Jint>(SPTamperedLog::Instance().GetRecords()[i].category)
            );

            (*env).SetObjectArrayElement(SPRecordLogRecordArr, i, SPRecordLogOfSonObj);
            (*env).DeleteLocalRef(mTimeStr);
        }while(false);

        (*env).DeleteLocalRef(SPRecordLogOfSonObj);
    }

    return SPRecordLogRecordArr;
}

#ifdef __cplusplus
}
#endif

#endif //SMARTTOOLS_SMART_JNI_SP_TAMPERED_LOG_HPP
