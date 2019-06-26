#pragma once

#ifndef SMARTTOOLS_SMART_ANDROID_SENSOR_HPP
#define SMARTTOOLS_SMART_ANDROID_SENSOR_HPP

#include "../kernel.hpp"
#include "../smart_resouces/smart_resouces_errors.hpp"

#include <APosSecurityManager.h>

namespace smart::android::sensor
{

using smart::resouces::errors::Errors;
using smart::resouces::errors::ErrorsType;

constexpr Jint SENSOR_SRC_BUF_SIZE = 64;

enum class SensorRoute
{
    ONE = 2,
    TWO = 4,
    THREE = 8,
    FOUR = 16,
};

enum class SensorState
{
    ACTIVE,
    ENABLED,
    ROUTE_IS_DISCONNECTED,
    COMMUNICATION_DISCONNECTED,
    LOCKED,
    NOT_RESPONSE,
    TAMPERED,
    UNKNOWN_ERROR,
};

typedef struct
{
    Jint srcSensorValueLen;
    Jbyte srcSensorValue[SENSOR_SRC_BUF_SIZE];

    SensorState state;
    SensorRoute route;
} SensorAttr;

typedef struct
{
    Jint sensorCheckState;
    Jint sensorActLen;
    Jchar *sensorAct;
    Jint sensorArrayLen;
    Jchar *sensorArray;
    APosSecurityManager *context;

    std::mutex posMutex;
} POSSDKSupport;

constexpr Jbyte SENSOR_ROUTE_CODE_ONE_VALUE = 0x02;
constexpr Jbyte SENSOR_ROUTE_CODE_TWO_VALUE = 0x04;
constexpr Jbyte SENSOR_ROUTE_CODE_TREE_VALUE = 0x08;
constexpr Jbyte SENSOR_ROUTE_CODE_FOUR_VALUE = 0x10;

constexpr Jchar SENSOR_POS_PLATFORM_PROTOCOL_IDENTIFIER_K21 = 0x22;
constexpr Jchar SENSOR_POS_PLATFORM_PROTOCOL_IDENTIFIER_3255 = 0x0C;
constexpr Jchar SENSOR_POS_PLATFORM_PROTOCOL_IDENTIFIER_M1902 = 0xCC;

constexpr Jint SENSOR_POS_SP_STATE_CODE_IS_TAMPERED = 39322;

class Sensor
{
public:
    static Sensor &Instance()
    {
        static Sensor *obj = nullptr;
        if (obj == nullptr)
            obj = new Sensor();
        return (*obj);
    }

    Jbool Active()
    {
        Jint activeCode = 0;
        Jbool state = false;

        do
        {
            if (!this->POSContextInit())
                break;

            this->mPOSSDKSupport.sensorCheckState = APosSecurityManager_SysSensorCheck(
                this->mPOSSDKSupport.context,
                nullptr,
                0,
                &this->mPOSSDKSupport.sensorArray,
                &this->mPOSSDKSupport.sensorArrayLen
            );

            if (this->mPOSSDKSupport.sensorCheckState == -1)
            {
                Errors::Instance().SetErrorType<ErrorsType::POS_INVALID>();
                break;
            }

            if ((this->mPOSSDKSupport.sensorArray == nullptr) || (this->mPOSSDKSupport.sensorArrayLen < 1))
                break;

            this->mSensorAttr.srcSensorValueLen = this->mPOSSDKSupport.sensorArrayLen;
            memcpy(
                this->mSensorAttr.srcSensorValue,
                this->mPOSSDKSupport.sensorArray,
                this->mSensorAttr.srcSensorValueLen
            );

            activeCode = APosSecurityManager_SysActSecurityFun(
                this->mPOSSDKSupport.context,
                this->mPOSSDKSupport.sensorArray,
                this->mPOSSDKSupport.sensorArrayLen,
                &this->mPOSSDKSupport.sensorAct,
                &this->mPOSSDKSupport.sensorActLen
            );

            state = (activeCode == 0 ? true : (activeCode == 0xFFFF));
        } while (false);

        this->PosContextRelease();
        return state;
    }

    Jbool Check()
    {
        Jbool state = false;

        do
        {
            if (!this->POSContextInit())
                break;

            this->mPOSSDKSupport.sensorCheckState = APosSecurityManager_SysSensorCheck(
                this->mPOSSDKSupport.context,
                nullptr,
                0,
                &this->mPOSSDKSupport.sensorArray,
                &this->mPOSSDKSupport.sensorArrayLen
            );

            if(this->mPOSSDKSupport.sensorCheckState == SENSOR_POS_SP_STATE_CODE_IS_TAMPERED)
            {
                Errors::Instance().SetErrorType<ErrorsType::POS_NEED_SHORT_SMALL_BATTERY_OR_REBOOT>();
                return false;
            }

            if ((this->mPOSSDKSupport.sensorArray == nullptr) || (this->mPOSSDKSupport.sensorArrayLen < 1))
                return false;

            this->mSensorAttr.srcSensorValueLen = this->mPOSSDKSupport.sensorArrayLen;
            memcpy(
                this->mSensorAttr.srcSensorValue,
                this->mPOSSDKSupport.sensorArray,
                this->mSensorAttr.srcSensorValueLen
            );

            if (!this->DistinguishRoute())
                break;
            if (!this->DistinguishResults())
                break;

            state = true;
        } while (false);

        this->PosContextRelease();
        return state;
    }

    SensorState GetSensorState()
    {
        return this->mSensorAttr.state;
    }

    SensorRoute GetSensorTamperedRoute()
    {
        return this->mSensorAttr.route;
    }

    const Jbyte *GetSourceSensorValue()
    {
        return this->mSensorAttr.srcSensorValue;
    }

    Jint GetSourceSensorValueLength()
    {
        return this->mSensorAttr.srcSensorValueLen;
    }

private:
    SensorAttr mSensorAttr;
    POSSDKSupport mPOSSDKSupport;

    Sensor() :
        mSensorAttr{},
        mPOSSDKSupport{}
    {}

    Jbool POSContextInit()
    {
        this->mPOSSDKSupport.posMutex.lock();
        memset(&this->mPOSSDKSupport, 0, sizeof(this->mPOSSDKSupport));
        this->mPOSSDKSupport.context = APosSecurityManager_new();
        return (this->mPOSSDKSupport.context != nullptr);
    }

    void PosContextRelease()
    {
        if (this->mPOSSDKSupport.sensorAct != nullptr)
            free(this->mPOSSDKSupport.sensorAct);
        if (this->mPOSSDKSupport.sensorArray != nullptr)
            free(this->mPOSSDKSupport.sensorArray);
        if (this->mPOSSDKSupport.context != nullptr)
            APosSecurityManager_delete(this->mPOSSDKSupport.context);

        this->mPOSSDKSupport.posMutex.unlock();
    }

    Jbool DistinguishRoute()
    {
        Jint step = 0;
        Juint routeCode = 0;
        Juint routeEnum = 0;

        routeCode = this->mPOSSDKSupport.sensorArray[step];
        routeCode += static_cast<Juint>(this->mPOSSDKSupport.sensorArray[++step]) << 8u;
        routeCode += static_cast<Juint>(this->mPOSSDKSupport.sensorArray[++step]) << 16u;
        routeCode += static_cast<Juint>(this->mPOSSDKSupport.sensorArray[++step]) << 24u;

        if ((routeCode & SENSOR_ROUTE_CODE_ONE_VALUE) == SENSOR_ROUTE_CODE_ONE_VALUE)
            routeEnum |= static_cast<Juint>(SensorRoute::ONE);
        if ((routeCode & SENSOR_ROUTE_CODE_TWO_VALUE) == SENSOR_ROUTE_CODE_TWO_VALUE)
            routeEnum |= static_cast<Juint>(SensorRoute::TWO);
        if ((routeCode & SENSOR_ROUTE_CODE_TREE_VALUE) == SENSOR_ROUTE_CODE_TREE_VALUE)
            routeEnum |= static_cast<Juint>(SensorRoute::THREE);
        if ((routeCode & SENSOR_ROUTE_CODE_FOUR_VALUE) == SENSOR_ROUTE_CODE_FOUR_VALUE)
            routeEnum |= static_cast<Juint>(SensorRoute::FOUR);

        this->mSensorAttr.route = static_cast<SensorRoute>(routeEnum);
        ++step;
        if (this->mPOSSDKSupport.sensorArrayLen > 5)
        {
            if (this->mPOSSDKSupport.sensorArray[step] == SENSOR_POS_PLATFORM_PROTOCOL_IDENTIFIER_K21)
            {
                step += 2;
                Log::Instance().Print<LogType::DEBUG>("the K21 platform, the tampered time:");
                Log::Instance().PrintHex(
                    reinterpret_cast<Jbyte *>(&this->mPOSSDKSupport.sensorArray[step]),
                    (this->mPOSSDKSupport.sensorArrayLen - step)
                );
            } else if (this->mPOSSDKSupport.sensorArray[step] == SENSOR_POS_PLATFORM_PROTOCOL_IDENTIFIER_3255)
            {
                Log::Instance().Print<LogType::DEBUG>("the 3255 platform");
                Log::Instance().Print<LogType::DEBUG>(
                    "register 0CH: %02X and 04H: %02X",
                    this->mPOSSDKSupport.sensorArray[step],
                    this->mPOSSDKSupport.sensorArray[step + 1]
                );

                step += 2;
                if ((this->mPOSSDKSupport.sensorArrayLen - step - 4) < 0)
                    return true;

                Log::Instance().Print<LogType::DEBUG>("SECDIAG:");
                Log::Instance().PrintHex(reinterpret_cast<Jbyte *>(&this->mPOSSDKSupport.sensorArray[step]), 4);
                step += 4;
                if ((this->mPOSSDKSupport.sensorArrayLen - step - 2) < 0)
                    return true;

                Log::Instance().Print<LogType::DEBUG>(
                    "register 08H: %02X and 04H: %02X",
                    this->mPOSSDKSupport.sensorArray[step],
                    this->mPOSSDKSupport.sensorArray[step + 1]
                );

                step += 2;
                if ((this->mPOSSDKSupport.sensorArrayLen - step - 4) < 0)
                    return true;

                Log::Instance().Print<LogType::DEBUG>("SECALM:");
                Log::Instance().PrintHex(reinterpret_cast<Jbyte *>(&this->mPOSSDKSupport.sensorArray[step]), 4);
            } else if (this->mPOSSDKSupport.sensorArray[step] == SENSOR_POS_PLATFORM_PROTOCOL_IDENTIFIER_M1902)
            {
                Log::Instance().Print<LogType::DEBUG>("the M1902 platform");
                Log::Instance().Print<LogType::DEBUG>(
                    "register CCH: %02X and 04H",
                    this->mPOSSDKSupport.sensorArray[step],
                    this->mPOSSDKSupport.sensorArray[step + 1]
                );

                step += 2;
                if ((this->mPOSSDKSupport.sensorArrayLen - step - 4) < 0)
                    return true;

                Log::Instance().Print<LogType::DEBUG>("SEN_STATE:");
                Log::Instance().PrintHex(reinterpret_cast<Jbyte *>(&this->mPOSSDKSupport.sensorArray[step]), 4);
            }
        }
        return true;
    }

    Jbool DistinguishResults()
    {
        if (this->mPOSSDKSupport.sensorCheckState == -1)
            return false;

        if (this->mPOSSDKSupport.sensorCheckState == 0)
            this->mSensorAttr.state = SensorState::ACTIVE;
        else if (this->mPOSSDKSupport.sensorCheckState == 0xFFFF)
            this->mSensorAttr.state = SensorState::ENABLED;
        else if ((this->mPOSSDKSupport.sensorCheckState == -2) || (this->mPOSSDKSupport.sensorCheckState == 0xFFFE))
            this->mSensorAttr.state = SensorState::ROUTE_IS_DISCONNECTED;
        else if ((this->mPOSSDKSupport.sensorCheckState == -5) || (this->mPOSSDKSupport.sensorCheckState == 0xFFFB))
            this->mSensorAttr.state = SensorState::COMMUNICATION_DISCONNECTED;
        else if (this->mPOSSDKSupport.sensorCheckState == 0x999A)
            this->mSensorAttr.state = SensorState::LOCKED;
        else if ((this->mPOSSDKSupport.sensorCheckState == -6) && (this->mPOSSDKSupport.sensorCheckState == 0xFFFA))
            this->mSensorAttr.state = SensorState::NOT_RESPONSE;
        else if (this->mPOSSDKSupport.sensorCheckState == 0xEEEE)
            this->mSensorAttr.state = SensorState::TAMPERED;
        else
            this->mSensorAttr.state = SensorState::UNKNOWN_ERROR;

        return true;
    }
};

}

#endif //SMARTTOOLS_SMART_ANDROID_SENSOR_HPP
