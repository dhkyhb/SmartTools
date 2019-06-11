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

JNIEXPORT jobjectArray JNICALL Java_cn_smartpeak_tools_SPRecordLog_GetRecords(JNIEnv *env, jobject self)
{
    Jint i = 0;
    jobjectArray objectArr = nullptr;

    jclass recordCls = nullptr;
    jmethodID clsInit = nullptr;
    jfieldID clsTime = nullptr;
    jfieldID clsCategory = nullptr;
    jfieldID clsType = nullptr;

    jobject recordObj = nullptr;
    jstring timeStr = nullptr;

    if (env == nullptr)
        return nullptr;
    if (recordCls = (*env).FindClass("cn/smartpeak/tools/Record");recordCls == nullptr)
        return nullptr;

    do
    {
        if (objectArr = (*env).NewObjectArray(
                    SPTamperedLog::Instance().GetRecordLength(),
                    recordCls,
                    nullptr
            );objectArr == nullptr)
            break;

        if (clsInit = (*env).GetMethodID(recordCls, "<init>", "()V");clsInit == nullptr)
            break;
        if (clsTime = (*env).GetFieldID(recordCls, "mTime", "Ljava/lang/String;");clsTime == nullptr)
            break;
        if (clsCategory = (*env).GetFieldID(recordCls, "mCategory", "I");clsCategory == nullptr)
            break;
        if (clsType = (*env).GetFieldID(recordCls, "mType", "I");clsType == nullptr)
            break;

        for (i = 0; i < SPTamperedLog::Instance().GetRecordLength(); ++i)
        {
            if (recordObj = (*env).NewObject(recordCls, clsInit);recordObj == nullptr)
                continue;

            do
            {
                if (timeStr = (*env).NewStringUTF(SPTamperedLog::Instance().GetRecords()[i].time);timeStr == nullptr)
                    break;

                (*env).SetObjectField(recordObj, clsTime, timeStr);
                (*env).SetIntField(
                        recordObj,
                        clsCategory,
                        static_cast<Jint>(SPTamperedLog::Instance().GetRecords()[i].category)
                );
                (*env).SetIntField(recordObj, clsType, SPTamperedLog::Instance().GetRecords()[i].type);
                (*env).SetObjectArrayElement(objectArr, i, recordObj);
            } while (false);

            (*env).DeleteLocalRef(timeStr);
            (*env).DeleteLocalRef(recordObj);
        }
    } while (false);

    return objectArr;
}

#ifdef __cplusplus
}
#endif

#endif //SMARTTOOLS_SMART_JNI_SP_TAMPERED_LOG_HPP
