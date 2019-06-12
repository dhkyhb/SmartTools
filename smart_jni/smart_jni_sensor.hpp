#ifndef SMARTTOOLS_SMART_JNI_SENSOR_HPP
#define SMARTTOOLS_SMART_JNI_SENSOR_HPP

#include "../kernel.hpp"
#include "../smart_android/smart_android_sensor.hpp"

using smart::android::sensor::Sensor;

#ifdef __cplusplus
extern "C"
{
#endif

JNIEXPORT jint JNICALL Java_cn_smartpeak_tools_SmartSensor_GetState(JNIEnv *env, jobject self)
{
    return static_cast<Jint>(Sensor::Instance().GetSensorState());
}

JNIEXPORT jint JNICALL Java_cn_smartpeak_tools_SmartSensor_GetTamperedRoute(JNIEnv *env, jobject self)
{
    return static_cast<Jint>(Sensor::Instance().GetSensorTamperedRoute());
}

#ifdef __cplusplus
}
#endif

#endif //SMARTTOOLS_SMART_JNI_SENSOR_HPP
