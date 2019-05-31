#ifndef SMARTTOOLS_SMART_JNI_EVERYTHING_HPP
#define SMARTTOOLS_SMART_JNI_EVERYTHING_HPP

#include "../kernel.hpp"
#include "../smart_android/smart_android_everything.hpp"

#include <jni.h>

using smart::android::everything::Everything;
using smart::android::everything::EverythingType;

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct JniEverything
{
    JNIEnv *env;
    JavaVM *javaVm;
    jobject callObject;

    jclass callClass;
    jmethodID callExecute;
    jmethodID callEnd;

    std::mutex jniLock;

    JniEverything() :
        env{},
        javaVm{},
        callObject{},
        callClass{},
        callExecute{},
        callEnd{},
        jniLock{}
    {}
} JniEverything;

static JniEverything *JniEverythingCall = nullptr;

constexpr Jchar JNI_EVERYTHING_CALLBACK_EXECUTE_METHOD_PARAMETER[] = "(I)V";
constexpr Jchar JNI_EVERYTHING_CALLBACK_END_METHOD_PARAMETER[] = "(IZ)V";
constexpr Jchar JNI_EVERYTHING_CALLBACK_EXECUTE_METHOD_NAME[] = "Execute";
constexpr Jchar JNI_EXERYTHING_CALLBACK_END_METHOD_NAME[] = "End";

JNIEXPORT void JNICALL Java_cn_smartpeak_tools_Everything_Listen(JNIEnv *env, jobject self, jobject javaCall)
{
    if ((env == nullptr) || (javaCall == nullptr))
        return;
    if (JniEverythingCall == nullptr)
        JniEverythingCall = new JniEverything();

    (*JniEverythingCall).jniLock.lock();
    if ((*JniEverythingCall).callObject != nullptr)
        (*env).DeleteGlobalRef((*JniEverythingCall).callObject);
    if ((*JniEverythingCall).javaVm != nullptr)
        (*JniEverythingCall).javaVm->DetachCurrentThread();

    Everything::Instance().NeedInit().SetExecuteThingListen(
        JniEverythingCall,
        [](void *p) {
            auto &jni = *reinterpret_cast<JniEverything *>(p);

            if (jni.javaVm->AttachCurrentThread(&jni.env, nullptr) != JNI_OK)
                return;
            if (jni.callClass = (*jni.env).GetObjectClass(jni.callObject);jni.callClass == nullptr)
                return;

            jni.callExecute = (*jni.env).GetMethodID(
                jni.callClass,
                JNI_EVERYTHING_CALLBACK_EXECUTE_METHOD_NAME,
                JNI_EVERYTHING_CALLBACK_EXECUTE_METHOD_PARAMETER
            );
            jni.callEnd = (*jni.env).GetMethodID(
                jni.callClass,
                JNI_EXERYTHING_CALLBACK_END_METHOD_NAME,
                JNI_EVERYTHING_CALLBACK_END_METHOD_PARAMETER
            );
        },
        [](void *p, Jint type) {
            auto &jni = *reinterpret_cast<JniEverything *>(p);
            if ((jni.env != nullptr) && (jni.callClass != nullptr) && (jni.callExecute != nullptr))
                (*jni.env).CallVoidMethod(jni.callClass, jni.callExecute, type);
        },
        [](void *p, Jint type, Jbool state) {
            auto &jni = *reinterpret_cast<JniEverything *>(p);
            if ((jni.env != nullptr) && (jni.callClass != nullptr) && (jni.callEnd != nullptr))
                (*jni.env).CallVoidMethod(jni.callClass, jni.callEnd, type, state);
        }
    ).Process();

    (*JniEverythingCall).jniLock.lock();
}

JNIEXPORT jobject JNICALL Java_cn_smartpeak_tools_Everything_SendThing(JNIEnv *env, jobject self, jint type)
{
    if (env == nullptr)
        return self;

    Everything::Instance().SendThing(static_cast<EverythingType>(type));
    return self;
}

#ifdef __cplusplus
}
#endif

#endif //SMARTTOOLS_SMART_JNI_EVERYTHING_HPP
