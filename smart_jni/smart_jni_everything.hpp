#pragma once

#ifndef SMARTTOOLS_SMART_JNI_EVERYTHING_HPP
#define SMARTTOOLS_SMART_JNI_EVERYTHING_HPP

#include "../kernel.hpp"
#include "../smart_android/smart_android_everything.hpp"

#include <jni.h>

using smart::resouces::status::StatusType;
using smart::resouces::errors::ErrorsType;
using smart::android::everything::Everything;
using smart::android::everything::EverythingType;

constexpr Juint SMART_JNI_EVERYTHING_STACK_SIZE = 6;
constexpr Juint SMART_JNI_EVERYTHING_STACK_NUM_1 = 2;
constexpr Juint SMART_JNI_EVERYTHING_STACK_NUM_2 = 4;
constexpr Juint SMART_JNI_EVERYTHING_STACK_NUM_3 = 8;
constexpr Juint SMART_JNI_EVERYTHING_STACK_NUM_4 = 16;
constexpr Juint SMART_JNI_EVERYTHING_STACK_NUM_5 = 32;
constexpr Juint SMART_JNI_EVERYTHING_STACK_NUM_6 = 64;

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct JniEverything
{
    JNIEnv *env[SMART_JNI_EVERYTHING_STACK_SIZE];
    JavaVM *javaVm[SMART_JNI_EVERYTHING_STACK_SIZE];
    jobject callObject[SMART_JNI_EVERYTHING_STACK_SIZE];

    jclass callClass[SMART_JNI_EVERYTHING_STACK_SIZE];
    jmethodID callExecute[SMART_JNI_EVERYTHING_STACK_SIZE];
    jmethodID callProcess[SMART_JNI_EVERYTHING_STACK_SIZE];
    jmethodID callEnd[SMART_JNI_EVERYTHING_STACK_SIZE];

    Jint id;
    std::mutex jniLock;

    JniEverything() :
        env{},
        javaVm{},
        callObject{},
        callClass{},
        callExecute{},
        callProcess{},
        callEnd{},
        id{},
        jniLock{}
    {}
} JniEverything;

typedef struct JniEverythingSend
{
    Jbool stackUse[SMART_JNI_EVERYTHING_STACK_SIZE];
    Jint stackID[SMART_JNI_EVERYTHING_STACK_SIZE];
    std::mutex lock;

    JniEverythingSend() :
        stackUse{},
        stackID{},
        lock{}
    {}
} JniEverythingSend;

static JniEverythingSend *JniEverythingSendObj = nullptr;
static JniEverything *JniEverythingCall = nullptr;

static void SmartJniEverythingStartMethod(const JniEverything &jni, const Jint &type)
{
    for (auto i = 0; i < static_cast<Jint>(sizeof((*JniEverythingSendObj).stackID)); ++i)
    {
        if (!(*JniEverythingSendObj).stackUse[i])
            continue;

        if ((static_cast<Juint>((*JniEverythingSendObj).stackID[i]) & SMART_JNI_EVERYTHING_STACK_NUM_1)
            == SMART_JNI_EVERYTHING_STACK_NUM_1)
        {
            constexpr Jint idx = 0;
            if ((jni.env[idx] != nullptr) && (jni.callObject[idx] != nullptr) && (jni.callExecute[idx] != nullptr))
                (*jni.env[idx]).CallVoidMethod(jni.callObject[idx], jni.callExecute[idx], type);
        }
        if ((static_cast<Juint>((*JniEverythingSendObj).stackID[i]) & SMART_JNI_EVERYTHING_STACK_NUM_2)
            == SMART_JNI_EVERYTHING_STACK_NUM_2)
        {
            constexpr Jint idx = 1;
            if ((jni.env[idx] != nullptr) && (jni.callObject[idx] != nullptr) && (jni.callExecute[idx] != nullptr))
                (*jni.env[idx]).CallVoidMethod(jni.callObject[idx], jni.callExecute[idx], type);
        }
        if ((static_cast<Juint>((*JniEverythingSendObj).stackID[i]) & SMART_JNI_EVERYTHING_STACK_NUM_3)
            == SMART_JNI_EVERYTHING_STACK_NUM_3)
        {
            constexpr Jint idx = 2;
            if ((jni.env[idx] != nullptr) && (jni.callObject[idx] != nullptr) && (jni.callExecute[idx] != nullptr))
                (*jni.env[idx]).CallVoidMethod(jni.callObject[idx], jni.callExecute[idx], type);
        }
        if ((static_cast<Juint>((*JniEverythingSendObj).stackID[i]) & SMART_JNI_EVERYTHING_STACK_NUM_4)
            == SMART_JNI_EVERYTHING_STACK_NUM_4)
        {
            constexpr Jint idx = 3;
            if ((jni.env[idx] != nullptr) && (jni.callObject[idx] != nullptr) && (jni.callExecute[idx] != nullptr))
                (*jni.env[idx]).CallVoidMethod(jni.callObject[idx], jni.callExecute[idx], type);
        }
        if ((static_cast<Juint>((*JniEverythingSendObj).stackID[i]) & SMART_JNI_EVERYTHING_STACK_NUM_5)
            == SMART_JNI_EVERYTHING_STACK_NUM_5)
        {
            constexpr Jint idx = 4;
            if ((jni.env[idx] != nullptr) && (jni.callObject[idx] != nullptr) && (jni.callExecute[idx] != nullptr))
                (*jni.env[idx]).CallVoidMethod(jni.callObject[idx], jni.callExecute[idx], type);
        }
        if ((static_cast<Juint>((*JniEverythingSendObj).stackID[i]) & SMART_JNI_EVERYTHING_STACK_NUM_6)
            == SMART_JNI_EVERYTHING_STACK_NUM_6)
        {
            constexpr Jint idx = 5;
            if ((jni.env[idx] != nullptr) && (jni.callObject[idx] != nullptr) && (jni.callExecute[idx] != nullptr))
                (*jni.env[idx]).CallVoidMethod(jni.callObject[idx], jni.callExecute[idx], type);
        }

        break;
    }
}

static void SmartJniEverythingProcessMethod(const JniEverything &jni, const StatusType &status)
{
    for (auto i = 0; i < static_cast<Jint>(sizeof((*JniEverythingSendObj).stackID)); ++i)
    {
        if (!(*JniEverythingSendObj).stackUse[i])
            continue;

        if ((static_cast<Juint>((*JniEverythingSendObj).stackID[i]) & SMART_JNI_EVERYTHING_STACK_NUM_1)
            == SMART_JNI_EVERYTHING_STACK_NUM_1)
        {
            constexpr Jint idx = 0;
            if ((jni.env[idx] != nullptr) && (jni.callObject[idx] != nullptr) && (jni.callProcess[idx] != nullptr))
                (*jni.env[idx]).CallVoidMethod(
                    jni.callObject[idx],
                    jni.callProcess[idx],
                    static_cast<Jint>(status)
                );
        }
        if ((static_cast<Juint>((*JniEverythingSendObj).stackID[i]) & SMART_JNI_EVERYTHING_STACK_NUM_2)
            == SMART_JNI_EVERYTHING_STACK_NUM_2)
        {
            constexpr Jint idx = 1;
            if ((jni.env[idx] != nullptr) && (jni.callObject[idx] != nullptr) && (jni.callProcess[idx] != nullptr))
                (*jni.env[idx]).CallVoidMethod(
                    jni.callObject[idx],
                    jni.callProcess[idx],
                    static_cast<Jint>(status)
                );
        }
        if ((static_cast<Juint>((*JniEverythingSendObj).stackID[i]) & SMART_JNI_EVERYTHING_STACK_NUM_3)
            == SMART_JNI_EVERYTHING_STACK_NUM_3)
        {
            constexpr Jint idx = 2;
            if ((jni.env[idx] != nullptr) && (jni.callObject[idx] != nullptr) && (jni.callProcess[idx] != nullptr))
                (*jni.env[idx]).CallVoidMethod(
                    jni.callObject[idx],
                    jni.callProcess[idx],
                    static_cast<Jint>(status)
                );
        }
        if ((static_cast<Juint>((*JniEverythingSendObj).stackID[i]) & SMART_JNI_EVERYTHING_STACK_NUM_4)
            == SMART_JNI_EVERYTHING_STACK_NUM_4)
        {
            constexpr Jint idx = 3;
            if ((jni.env[idx] != nullptr) && (jni.callObject[idx] != nullptr) && (jni.callProcess[idx] != nullptr))
                (*jni.env[idx]).CallVoidMethod(
                    jni.callObject[idx],
                    jni.callProcess[idx],
                    static_cast<Jint>(status)
                );
        }
        if ((static_cast<Juint>((*JniEverythingSendObj).stackID[i]) & SMART_JNI_EVERYTHING_STACK_NUM_5)
            == SMART_JNI_EVERYTHING_STACK_NUM_5)
        {
            constexpr Jint idx = 4;
            if ((jni.env[idx] != nullptr) && (jni.callObject[idx] != nullptr) && (jni.callProcess[idx] != nullptr))
                (*jni.env[idx]).CallVoidMethod(
                    jni.callObject[idx],
                    jni.callProcess[idx],
                    static_cast<Jint>(status)
                );
        }
        if ((static_cast<Juint>((*JniEverythingSendObj).stackID[i]) & SMART_JNI_EVERYTHING_STACK_NUM_6)
            == SMART_JNI_EVERYTHING_STACK_NUM_6)
        {
            constexpr Jint idx = 5;
            if ((jni.env[idx] != nullptr) && (jni.callObject[idx] != nullptr) && (jni.callProcess[idx] != nullptr))
                (*jni.env[idx]).CallVoidMethod(
                    jni.callObject[idx],
                    jni.callProcess[idx],
                    static_cast<Jint>(status)
                );
        }

        break;
    }
}

static void SmartJniEverythingEndMethod(
    const JniEverything &jni,
    const Jint &type,
    const Jbool &state,
    const ErrorsType &errorsType
)
{
    for (auto i = 0; i < static_cast<Jint>(sizeof((*JniEverythingSendObj).stackID)); ++i)
    {
        if (!(*JniEverythingSendObj).stackUse[i])
            continue;
        
        if ((static_cast<Juint>((*JniEverythingSendObj).stackID[i]) & SMART_JNI_EVERYTHING_STACK_NUM_1)
            == SMART_JNI_EVERYTHING_STACK_NUM_1)
        {
            constexpr Jint idx = 0;
            if ((jni.env[idx] != nullptr) && (jni.callObject[idx] != nullptr) && (jni.callEnd[idx] != nullptr))
                (*jni.env[idx]).CallVoidMethod(
                    jni.callObject[idx],
                    jni.callEnd[idx],
                    type,
                    state,
                    static_cast<Jint>(errorsType)
                );
        }
        if ((static_cast<Juint>((*JniEverythingSendObj).stackID[i]) & SMART_JNI_EVERYTHING_STACK_NUM_2)
            == SMART_JNI_EVERYTHING_STACK_NUM_2)
        {
            constexpr Jint idx = 1;
            if ((jni.env[idx] != nullptr) && (jni.callObject[idx] != nullptr) && (jni.callEnd[idx] != nullptr))
                (*jni.env[idx]).CallVoidMethod(
                    jni.callObject[idx],
                    jni.callEnd[idx],
                    type,
                    state,
                    static_cast<Jint>(errorsType)
                );
        }
        if ((static_cast<Juint>((*JniEverythingSendObj).stackID[i]) & SMART_JNI_EVERYTHING_STACK_NUM_3)
            == SMART_JNI_EVERYTHING_STACK_NUM_3)
        {
            constexpr Jint idx = 2;
            if ((jni.env[idx] != nullptr) && (jni.callObject[idx] != nullptr) && (jni.callEnd[idx] != nullptr))
                (*jni.env[idx]).CallVoidMethod(
                    jni.callObject[idx],
                    jni.callEnd[idx],
                    type,
                    state,
                    static_cast<Jint>(errorsType)
                );
        }
        if ((static_cast<Juint>((*JniEverythingSendObj).stackID[i]) & SMART_JNI_EVERYTHING_STACK_NUM_4)
            == SMART_JNI_EVERYTHING_STACK_NUM_4)
        {
            constexpr Jint idx = 3;
            if ((jni.env[idx] != nullptr) && (jni.callObject[idx] != nullptr) && (jni.callEnd[idx] != nullptr))
                (*jni.env[idx]).CallVoidMethod(
                    jni.callObject[idx],
                    jni.callEnd[idx],
                    type,
                    state,
                    static_cast<Jint>(errorsType)
                );
        }
        if ((static_cast<Juint>((*JniEverythingSendObj).stackID[i]) & SMART_JNI_EVERYTHING_STACK_NUM_5)
            == SMART_JNI_EVERYTHING_STACK_NUM_5)
        {
            constexpr Jint idx = 4;
            if ((jni.env[idx] != nullptr) && (jni.callObject[idx] != nullptr) && (jni.callEnd[idx] != nullptr))
                (*jni.env[idx]).CallVoidMethod(
                    jni.callObject[idx],
                    jni.callEnd[idx],
                    type,
                    state,
                    static_cast<Jint>(errorsType)
                );
        }
        if ((static_cast<Juint>((*JniEverythingSendObj).stackID[i]) & SMART_JNI_EVERYTHING_STACK_NUM_6)
            == SMART_JNI_EVERYTHING_STACK_NUM_6)
        {
            constexpr Jint idx = 5;
            if ((jni.env[idx] != nullptr) && (jni.callObject[idx] != nullptr) && (jni.callEnd[idx] != nullptr))
                (*jni.env[idx]).CallVoidMethod(
                    jni.callObject[idx],
                    jni.callEnd[idx],
                    type,
                    state,
                    static_cast<Jint>(errorsType)
                );
        }

        (*JniEverythingSendObj).stackID[i] = 0;
        (*JniEverythingSendObj).stackUse[i] = false;
        break;
    }
}

JNIEXPORT void JNICALL Java_cn_smartpeak_tools_SmartEverything_Listen(
    JNIEnv *env,
    jobject self,
    jint stackID,
    jobject javaCall
)
{
    constexpr static Jchar JNI_EVERYTHING_CALLBACK_EXECUTE_METHOD_PARAMETER[] = "(I)V";
    constexpr static Jchar JNI_EVERYTHING_CALLBACK_PROCESS_METHOD_PARAMETER[] = "(I)V";
    constexpr static Jchar JNI_EVERYTHING_CALLBACK_END_METHOD_PARAMETER[] = "(IZI)V";
    constexpr static Jchar JNI_EVERYTHING_CALLBACK_EXECUTE_METHOD_NAME[] = "Execute";
    constexpr static Jchar JNI_EVERYTHING_CALLBACK_PROCESS_METHOD_NAME[] = "Process";
    constexpr static Jchar JNI_EXERYTHING_CALLBACK_END_METHOD_NAME[] = "End";

    Jint i = 0;

    if ((env == nullptr) || (javaCall == nullptr))
        return;
    if (JniEverythingCall == nullptr)
        JniEverythingCall = new JniEverything();
    if (JniEverythingSendObj == nullptr)
        JniEverythingSendObj = new JniEverythingSend();

    if ((static_cast<Juint>(stackID) & SMART_JNI_EVERYTHING_STACK_NUM_1) == SMART_JNI_EVERYTHING_STACK_NUM_1)
        i = 0;
    else if ((static_cast<Juint>(stackID) & SMART_JNI_EVERYTHING_STACK_NUM_2) == SMART_JNI_EVERYTHING_STACK_NUM_2)
        i = 1;
    else if ((static_cast<Juint>(stackID) & SMART_JNI_EVERYTHING_STACK_NUM_3) == SMART_JNI_EVERYTHING_STACK_NUM_3)
        i = 2;
    else if ((static_cast<Juint>(stackID) & SMART_JNI_EVERYTHING_STACK_NUM_4) == SMART_JNI_EVERYTHING_STACK_NUM_4)
        i = 3;
    else if ((static_cast<Juint>(stackID) & SMART_JNI_EVERYTHING_STACK_NUM_5) == SMART_JNI_EVERYTHING_STACK_NUM_5)
        i = 4;
    else if ((static_cast<Juint>(stackID) & SMART_JNI_EVERYTHING_STACK_NUM_6) == SMART_JNI_EVERYTHING_STACK_NUM_6)
        i = 5;

    if (((*JniEverythingCall).javaVm[i] != nullptr) || ((*JniEverythingCall).callObject[i] != nullptr))
    {
        (*env).DeleteLocalRef((*JniEverythingCall).callObject[i]);
        (*JniEverythingCall).javaVm[i] = nullptr;
        (*JniEverythingCall).callObject[i] = nullptr;
    }

    (*JniEverythingCall).jniLock.lock();
    if ((*JniEverythingCall).javaVm[i] == nullptr)
        (*env).GetJavaVM(&(*JniEverythingCall).javaVm[i]);
    if ((*JniEverythingCall).callObject[i] == nullptr)
        (*JniEverythingCall).callObject[i] = (*env).NewGlobalRef(javaCall);

    (*JniEverythingCall).id = i;
    Everything::Instance().NeedInit().SetExecuteThingListen(
        JniEverythingCall,
        [](void *p) {
            auto &jni = *reinterpret_cast<JniEverything *>(p);

            Log::Instance().Print<LogType::INFO>("initialization everything listener");
            if ((*jni.javaVm[jni.id]).AttachCurrentThread(&jni.env[jni.id], nullptr) != JNI_OK)
                return;
            if (jni.callClass[jni.id] = (*jni.env[jni.id]).GetObjectClass(
                    jni.callObject[jni.id]
                );jni.callClass[jni.id] == nullptr)
                return;

            jni.callExecute[jni.id] = (*jni.env[jni.id]).GetMethodID(
                jni.callClass[jni.id],
                JNI_EVERYTHING_CALLBACK_EXECUTE_METHOD_NAME,
                JNI_EVERYTHING_CALLBACK_EXECUTE_METHOD_PARAMETER
            );
            jni.callProcess[jni.id] = (*jni.env[jni.id]).GetMethodID(
                jni.callClass[jni.id],
                JNI_EVERYTHING_CALLBACK_PROCESS_METHOD_NAME,
                JNI_EVERYTHING_CALLBACK_PROCESS_METHOD_PARAMETER
            );
            jni.callEnd[jni.id] = (*jni.env[jni.id]).GetMethodID(
                jni.callClass[jni.id],
                JNI_EXERYTHING_CALLBACK_END_METHOD_NAME,
                JNI_EVERYTHING_CALLBACK_END_METHOD_PARAMETER
            );

            (*jni.env[jni.id]).DeleteLocalRef(jni.callClass[jni.id]);
        },
        [](void *p, Jint type) {
            auto &jni = *reinterpret_cast<JniEverything *>(p);
            SmartJniEverythingStartMethod(jni, type);
        },
        [](void *p, StatusType status) {
            auto &jni = *reinterpret_cast<JniEverything *>(p);
            SmartJniEverythingProcessMethod(jni, status);
        },
        [](void *p, Jint type, Jbool state, ErrorsType errorsType) {
            auto &jni = *reinterpret_cast<JniEverything *>(p);
            SmartJniEverythingEndMethod(jni, type, state, errorsType);
        }
    ).Process();

    (*JniEverythingCall).jniLock.unlock();
}

JNIEXPORT jobject JNICALL Java_cn_smartpeak_tools_SmartEverything_SendThing(
    JNIEnv *env,
    jobject self,
    jint stackID,
    jint type
)
{
    Jint i = 0;
    Jbool state = false;

    if ((env == nullptr) || (JniEverythingSendObj == nullptr))
        return self;

    (*JniEverythingSendObj).lock.lock();
    for (i = 0; i < static_cast<Jint>(sizeof((*JniEverythingSendObj).stackID)); ++i)
    {
        if ((*JniEverythingSendObj).stackUse[i])
            continue;

        (*JniEverythingSendObj).stackUse[i] = true;
        (*JniEverythingSendObj).stackID[i] = stackID;
        state = true;
        break;
    }

    if (state)
        Everything::Instance().SendThing(static_cast<EverythingType>(type));
    (*JniEverythingSendObj).lock.unlock();
    return self;
}

#ifdef __cplusplus
}
#endif

#endif //SMARTTOOLS_SMART_JNI_EVERYTHING_HPP
