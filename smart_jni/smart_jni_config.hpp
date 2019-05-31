#ifndef SMARTTOOLS_SMART_JNI_CONFIG_HPP
#define SMARTTOOLS_SMART_JNI_CONFIG_HPP

#include "../kernel.hpp"
#include "../smart_utils/smart_utils_log.hpp"
#include "../smart_android/smart_android_config.hpp"

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

using smart::utils::log::Log;
using smart::utils::log::LogType;
using smart::android::config::Config;

#ifdef __cplusplus
extern "C"
{
#endif

constexpr Jint SMART_JNI_CONFIG_TMP_BUF_SIZE = 200;
constexpr Jchar SMART_JNI_CONFIG_SYSTEM_DEFAULT_PATH[] = "/system/etc/app/cn.basewin.security.defense.system.xml";
constexpr Jchar SMART_JNI_CONFIG_APP_DEFAULT_PATH[] = "config";

JNIEXPORT void JNICALL Java_cn_smartpeak_tools_Config_Init(JNIEnv *env, jobject self, jobject assetContext)
{
    Jint ret = 0;
    Jchar tmpBuf[SMART_JNI_CONFIG_TMP_BUF_SIZE];

    FILE *systemConfigFile = nullptr;
    AAssetManager *aAssetManager = nullptr;
    AAsset *aAsset = nullptr;

    if ((env == nullptr) || (assetContext == nullptr))
        return;

    Config::Instance().Reset();

    do
    {
        if (systemConfigFile = fopen(SMART_JNI_CONFIG_SYSTEM_DEFAULT_PATH, "rb");systemConfigFile == nullptr)
            break;

        while (true)
        {
            ret = fread(tmpBuf, 1, sizeof(tmpBuf), systemConfigFile);
            Config::Instance().WriteBuffer(tmpBuf, ret);
            if (ret < sizeof(tmpBuf))
                break;
        }
    } while (false);

    do
    {
        if (systemConfigFile != nullptr)
            break;
        if (aAssetManager = AAssetManager_fromJava(env, assetContext);aAssetManager == nullptr)
            break;

        if (aAsset = AAssetManager_open(
                    aAssetManager,
                    SMART_JNI_CONFIG_APP_DEFAULT_PATH,
                    AASSET_MODE_UNKNOWN
            );aAsset == nullptr)
            break;

        Config::Instance().WriteBuffer(
                reinterpret_cast<Jchar *>(const_cast<void *>(AAsset_getBuffer(aAsset))),
                AAsset_getLength(aAsset)
        );
    } while (false);

    if (systemConfigFile != nullptr)
        fclose(systemConfigFile);
    if (aAsset != nullptr)
        AAsset_close(aAsset);

    if (Config::Instance().Process())
        Log::Instance().Print<LogType::INFO>("configure is ready");
    else
        Log::Instance().Print<LogType::INFO>("configure not is ready");
}

#ifdef __cplusplus
}
#endif

#endif //SMARTTOOLS_SMART_JNI_CONFIG_HPP
