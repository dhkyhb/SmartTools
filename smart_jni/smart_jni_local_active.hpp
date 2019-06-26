#pragma once

#ifndef SMARTTOOLS_SMART_JNI_LOCAL_ACTIVE_HPP
#define SMARTTOOLS_SMART_JNI_LOCAL_ACTIVE_HPP

#include "../kernel.hpp"
#include "../smart_android/smart_android_local_active.hpp"

#include <jni.h>

using smart::android::local::active::LocalActive;

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    JavaVM *createVM;
    jobject createObj;
    jclass createCls;
    jmethodID createListen;

    JavaVM *usingVM;
    jobject usingObj;
    jclass usingCls;
    jmethodID usingListen;

    JavaVM *finishVM;
    jobject finishObj;
    jclass finishCls;
    jmethodID finishListen;
} JniLocalActive;

static JniLocalActive JniLocalActiveObj;

constexpr Jchar SMART_JNI_LOCAL_ACTIVE_CREATE_CALL_METHOD[] = "Listen";
constexpr Jchar SMART_JNI_LOCAL_ACTIVE_CREATE_CALL_METHOD_SIG[] = "()V";
constexpr Jchar SMART_JNI_LOCAL_ACTIVE_USING_CALL_METHOD[] = "Listen";
constexpr Jchar SMART_JNI_LOCAL_ACTIVE_USING_CALL_METHOD_SIG[] = "(I)V";
constexpr Jchar SMART_JNI_LOCAL_ACTIVE_FINISH_CALL_METHOD[] = "Listen";
constexpr Jchar SMART_JNI_LOCAL_ACTIVE_FINISH_CALL_METHOD_SIG[] = "(Z)V";

JNIEXPORT jobject JNICALL Java_cn_smartpeak_tools_SmartLocalActive_Init(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return self;
    LocalActive::Instance().Init();
    return self;
}

JNIEXPORT jobject JNICALL Java_cn_smartpeak_tools_SmartLocalActive_SetCreateCall(
    JNIEnv *env,
    jobject self,
    jobject call
)
{
    if ((env == nullptr) || (call == nullptr))
        return self;
    if (JniLocalActiveObj.createObj != nullptr)
        (*env).DeleteGlobalRef(JniLocalActiveObj.createObj);

    (*env).GetJavaVM(&JniLocalActiveObj.createVM);
    JniLocalActiveObj.createObj = (*env).NewGlobalRef(call);
    return self;
}

JNIEXPORT jobject JNICALL Java_cn_smartpeak_tools_SmartLocalActive_SetUsingCall(
    JNIEnv *env,
    jobject self,
    jobject call
)
{
    if ((env == nullptr) || (call == nullptr))
        return self;
    if (JniLocalActiveObj.usingObj != nullptr)
        (*env).DeleteGlobalRef(JniLocalActiveObj.usingObj);

    (*env).GetJavaVM(&JniLocalActiveObj.usingVM);
    JniLocalActiveObj.usingObj = (*env).NewGlobalRef(call);
    return self;
}

JNIEXPORT jobject JNICALL Java_cn_smartpeak_tools_SmartLocalActive_SetFinishCall(
    JNIEnv *env,
    jobject self,
    jobject call
)
{
    if ((env == nullptr) || (call == nullptr))
        return self;
    if (JniLocalActiveObj.finishObj != nullptr)
        (*env).DeleteGlobalRef(JniLocalActiveObj.finishObj);

    (*env).GetJavaVM(&JniLocalActiveObj.finishVM);
    JniLocalActiveObj.finishObj = (*env).NewGlobalRef(call);
    return self;
}

JNIEXPORT void JNICALL Java_cn_smartpeak_tools_SmartLocalActive_Register(JNIEnv *env, jobject self)
{
    if (env == nullptr)
        return;

    LocalActive::Instance().Init()
        .SetCreate(&JniLocalActiveObj, [](void *p) {
            JNIEnv *env = nullptr;

            if (p == nullptr)
                return;

            auto &&ctx = *reinterpret_cast<JniLocalActive *>(p);
            if (ctx.createVM == nullptr)
                return;

            do
            {
                if ((*ctx.createVM).AttachCurrentThread(&env, nullptr);env == nullptr)
                    break;
                if (ctx.createCls = (*env).GetObjectClass(ctx.createObj);ctx.createCls == nullptr)
                    break;

                if (ctx.createListen = (*env).GetMethodID(
                        ctx.createCls,
                        SMART_JNI_LOCAL_ACTIVE_CREATE_CALL_METHOD,
                        SMART_JNI_LOCAL_ACTIVE_CREATE_CALL_METHOD_SIG
                    );ctx.createListen == nullptr)
                    break;

                (*env).CallVoidMethod(ctx.createObj, ctx.createListen);
            } while (false);

            if (ctx.createCls != nullptr)
                (*env).DeleteLocalRef(ctx.createCls);
            (*ctx.createVM).DetachCurrentThread();
        }).SetUsing(&JniLocalActiveObj, [](void *p, Jint key) {
            JNIEnv *env = nullptr;

            if (p == nullptr)
                return;

            auto &&ctx = *reinterpret_cast<JniLocalActive *>(p);
            if (ctx.usingVM == nullptr)
                return;

            do
            {
                if ((*ctx.usingVM).AttachCurrentThread(&env, nullptr);env == nullptr)
                    break;
                if (ctx.usingCls = (*env).GetObjectClass(ctx.usingObj);ctx.usingCls == nullptr)
                    break;

                if (ctx.usingListen = (*env).GetMethodID(
                        ctx.usingCls,
                        SMART_JNI_LOCAL_ACTIVE_USING_CALL_METHOD,
                        SMART_JNI_LOCAL_ACTIVE_USING_CALL_METHOD_SIG
                    );ctx.usingListen == nullptr)
                    break;

                (*env).CallVoidMethod(ctx.usingObj, ctx.usingListen, key);
            } while (false);

            if (ctx.usingCls != nullptr)
                (*env).DeleteLocalRef(ctx.usingCls);
            (*ctx.usingVM).DetachCurrentThread();
        }).SetFinish(&JniLocalActiveObj, [](void *p, Jbool state) {
            JNIEnv *env = nullptr;

            if (p == nullptr)
                return;

            auto &&ctx = *reinterpret_cast<JniLocalActive *>(p);
            if (ctx.finishVM == nullptr)
                return;

            do
            {
                if ((*ctx.finishVM).AttachCurrentThread(&env, nullptr);env == nullptr)
                    break;
                if (ctx.finishCls = (*env).GetObjectClass(ctx.finishObj);ctx.finishCls == nullptr)
                    break;

                if (ctx.finishListen = (*env).GetMethodID(
                        ctx.finishCls,
                        SMART_JNI_LOCAL_ACTIVE_FINISH_CALL_METHOD,
                        SMART_JNI_LOCAL_ACTIVE_FINISH_CALL_METHOD_SIG
                    );ctx.finishListen == nullptr)
                    return;

                (*env).CallVoidMethod(ctx.finishObj, ctx.finishListen, state);
            } while (false);

            if (ctx.finishCls != nullptr)
                (*env).DeleteLocalRef(ctx.finishCls);
            (*ctx.finishVM).DetachCurrentThread();
        }).Register();
}

JNIEXPORT void JNICALL Java_cn_smartpeak_tools_SmartLocalActive_SetKeyPosition(
    JNIEnv *env,
    jobject self,
    jbyteArray array,
    jint arrayLen
)
{
    if ((env == nullptr) || (array == nullptr))
        return;

    auto &&v = (*env).GetByteArrayElements(array, JNI_FALSE);
    LocalActive::Instance().SetKeyPosition(reinterpret_cast<Jbyte *>(v), arrayLen);
    (*env).ReleaseByteArrayElements(array, v, 0);
}

#ifdef __cplusplus
}
#endif

#endif //SMARTTOOLS_SMART_JNI_LOCAL_ACTIVE_HPP
