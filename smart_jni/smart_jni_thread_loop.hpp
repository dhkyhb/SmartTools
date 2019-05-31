#ifndef SMARTTOOLS_SMART_JNI_THREAD_LOOP_HPP
#define SMARTTOOLS_SMART_JNI_THREAD_LOOP_HPP

#include "../kernel.hpp"
#include "../smart_utils/smart_utils_thread_loop.hpp"

#include <jni.h>

using smart::utils::thread::loop::ThreadLoop;

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct JniAttr
{
    Jbool check;

    JavaVM *javaVm;
    jobject jniObject;

    std::mutex *jniLock;

    JniAttr() :
            check{},
            javaVm{},
            jniObject{},
            jniLock{new std::mutex()}
    {}

    JniAttr(const JniAttr &self) = default;
} JniAttr;

static JniAttr *JniAttrList = nullptr;

constexpr Jchar JNI_THREAD_LOOP_CALLBACK_METHOD_NAME[] = "OnProcess";
constexpr Jchar JNI_THREAD_LOOP_CALLBACK_METHOD_PARAMETER[] = "(I)V";

JNIEXPORT jobject JNICALL Java_cn_smartpeak_tools_ThreadLoop_Init(JNIEnv *env, jobject self, jint loopSize)
{
    if ((env == nullptr) || (loopSize < 1))
        return self;
    if (JniAttrList == nullptr)
        JniAttrList = new JniAttr();
    if ((*JniAttrList).javaVm == nullptr)
        (*env).GetJavaVM(&(*JniAttrList).javaVm);

    ThreadLoop::Instance().Init(loopSize);
    return self;
}

JNIEXPORT jobject JNICALL Java_cn_smartpeak_tools_ThreadLoop_Insert(JNIEnv *env, jobject self, jobject method)
{
    if ((JniAttrList == nullptr) || ((*JniAttrList).javaVm == nullptr))
        return self;

    (*JniAttrList).jniLock->lock();
    (*JniAttrList).jniObject = (*env).NewGlobalRef(method);
    (*JniAttrList).check = false;
    ThreadLoop::Instance().Insert(JniAttrList, [](Jint id, void *p) {
        JNIEnv *env = nullptr;
        jclass callback = nullptr;
        jmethodID processId = nullptr;
        auto jni = *reinterpret_cast<JniAttr *>(p);

        (*JniAttrList).check = true;
        if (jni.javaVm->AttachCurrentThread(&env, nullptr) != JNI_OK)
            return;

        do
        {
            if (callback = (*env).GetObjectClass(jni.jniObject);callback == nullptr)
                break;

            if (processId = (*env).GetMethodID(
                        callback,
                        JNI_THREAD_LOOP_CALLBACK_METHOD_NAME,
                        JNI_THREAD_LOOP_CALLBACK_METHOD_PARAMETER
                );processId == nullptr)
                break;

            (*env).CallVoidMethod(callback, processId, id);
        } while (false);

        if (jni.jniObject != nullptr)
            (*env).DeleteGlobalRef(jni.jniObject);
        jni.javaVm->DetachCurrentThread();
    });

    return self;
}

JNIEXPORT void JNICALL Java_cn_smartpeak_tools_ThreadLoop_Run(JNIEnv *env, jobject self)
{
    if ((env == nullptr) || (JniAttrList == nullptr))
        return;

    ThreadLoop::Instance().Process();
    while (!(*JniAttrList).check)
        std::this_thread::sleep_for(std::chrono::microseconds(1));

    (*JniAttrList).jniLock->unlock();
}

#ifdef __cplusplus
}
#endif

#endif //SMARTTOOLS_SMART_JNI_THREAD_LOOP_HPP
