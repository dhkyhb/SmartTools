#pragma once

#ifndef SMARTTOOLS_SMART_JNI_LOG_HPP
#define SMARTTOOLS_SMART_JNI_LOG_HPP

#include "../kernel.hpp"
#include "../smart_utils/smart_utils_log.hpp"

#include <jni.h>

using smart::utils::log::Log;
using smart::utils::log::LogType;

#ifdef __cplusplus
extern "C"
{
#endif

JNIEXPORT void JNICALL Java_cn_smartpeak_tools_SmartLog_SetTag(JNIEnv *env, jclass self, jstring content)
{
    const Jchar *cContent = nullptr;

    if ((env == nullptr) || (content == nullptr))
        return;
    if (cContent = (*env).GetStringUTFChars(content, JNI_FALSE);cContent == nullptr)
        return;

    Log::Instance().SetTag(cContent);
    (*env).ReleaseStringUTFChars(content, cContent);
}

JNIEXPORT void JNICALL Java_cn_smartpeak_tools_SmartLog_Info(JNIEnv *env, jclass self, jstring content)
{
    const Jchar *cContent = nullptr;

    if ((env == nullptr) || (content == nullptr))
        return;
    if (cContent = (*env).GetStringUTFChars(content, JNI_FALSE);cContent == nullptr)
        return;

    Log::Instance().Print<LogType::INFO>(cContent);
    (*env).ReleaseStringUTFChars(content, cContent);
}

JNIEXPORT void JNICALL Java_cn_smartpeak_tools_SmartLog_Debug(JNIEnv *env, jclass self, jstring content)
{
    const Jchar *cContent = nullptr;

    if ((env == nullptr) || (content == nullptr))
        return;
    if (cContent = (*env).GetStringUTFChars(content, JNI_FALSE);cContent == nullptr)
        return;

    Log::Instance().Print<LogType::DEBUG>(cContent);
    (*env).ReleaseStringUTFChars(content, cContent);
}

JNIEXPORT void JNICALL Java_cn_smartpeak_tools_SmartLog_Error(JNIEnv *env, jclass self, jstring content)
{
    const Jchar *cContent = nullptr;

    if ((env == nullptr) || (content == nullptr))
        return;
    if (cContent = (*env).GetStringUTFChars(content, JNI_FALSE);cContent == nullptr)
        return;

    Log::Instance().Print<LogType::ERROR>(cContent);
    (*env).ReleaseStringUTFChars(content, cContent);
}

#ifdef __cplusplus
}
#endif

#endif //SMARTTOOLS_SMART_JNI_LOG_HPP
