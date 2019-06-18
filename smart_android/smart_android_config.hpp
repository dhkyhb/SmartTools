#pragma once

#ifndef SMARTTOOLS_SMART_ANDROID_CONFIG_HPP
#define SMARTTOOLS_SMART_ANDROID_CONFIG_HPP

#include "../kernel.hpp"
#include "../smart_utils/smart_utils_log.hpp"

#include <string>
#include <boost/property_tree/xml_parser.hpp>

namespace smart::android::config
{

using smart::utils::log::Log;
using smart::utils::log::LogType;

constexpr Jint CONFIG_ELEMENT_BUF_VERSION_SIZE = 32;
constexpr Jint CONFIG_ELEMENT_BUF_POS_VERIFY_ADDRESS_SIZE = 1024;
constexpr Jint CONFIG_ELEMENT_BUF_POS_CHECK_CUSTOMER_ADDRESS = 1024;

constexpr Jint CONFIG_BUFFER_CACHE_SIZE = 4096;

typedef struct
{
    Jint checkCustomerTimeouts;
    Jint checkCustomerPort;
    Jchar checkCustomerAddress[CONFIG_ELEMENT_BUF_POS_CHECK_CUSTOMER_ADDRESS];

    Jint verifyTimeouts;
    Jint verifyPort;
    Jchar verifyAddress[CONFIG_ELEMENT_BUF_POS_VERIFY_ADDRESS_SIZE];

    Jint model;
    Jbool audio;
    Jbool signVerify;
    Jbool posVerify;
    Jbool launchSensorCheck;
    Jbool launchCheckCustomer;
    Jchar version[CONFIG_ELEMENT_BUF_VERSION_SIZE];
} ConfigFileElement;

typedef struct
{
    Jbool isReady;
    Jint cacheSet;
    Jchar cache[CONFIG_BUFFER_CACHE_SIZE];
} ConfigAttr;

typedef struct BoostXMLSupport
{
    boost::property_tree::ptree root;
    std::stringstream localXML;

    BoostXMLSupport() :
        root{},
        localXML{}
    {}
} BoostXMLSupport;

constexpr Jchar CONFIG_ELEMENT_ROOT[] = "root";

constexpr Jchar CONFIG_ELEMENT_POS_VERIFY_ADDRESS_TAG[] = "root.pos_verify_address";
constexpr Jchar CONFIG_ELEMENT_POS_VERIFY_PORT_TAG[] = "root.pos_verify_port";
constexpr Jchar CONFIG_ELEMENT_POS_VERIFY_TIMEOUTS_TAG[] = "root.pos_verify_timeout";

constexpr Jchar CONFIG_ELEMENT_POS_CHECK_CUSTOMER_ADDRESS_TAG[] = "root.pos_check_customer_address";
constexpr Jchar CONFIG_ELEMENT_POS_CHECK_CUSTOMER_PORT_TAG[] = "root.pos_check_customer_port";
constexpr Jchar CONFIG_ELEMENT_POS_CHECK_CUSTOMER_TIMEOUTS_TAG[] = "root.pos_check_customer_timeout";

constexpr Jchar CONFIG_ELEMENT_MODEL_TAG[] = "root.model";
constexpr Jchar CONFIG_ELEMENT_AUDIO_TAG[] = "root.audio";
constexpr Jchar CONFIG_ELEMENT_SIGN_VERIFY[] = "root.sign_verify";
constexpr Jchar CONFIG_ELEMENT_POS_VERIFY[] = "root.pos_verify";
constexpr Jchar CONFIG_ELEMENT_LAUNCH_SENSOR_CHECK_TAG[] = "root.launch_sensor_check";
constexpr Jchar CONFIG_ELEMENT_LAUNCH_CHECK_CUSTOMER_TAG[] = "root.launch_check_customer";
constexpr Jchar CONFIG_ELEMENT_VERSION_TAG[] = "root.version";

constexpr Jint CONFIG_DEFAULT_TIMEOUTS = 5000;

class Config
{
public:
    static Config &Instance()
    {
        static Config *obj = nullptr;
        if (obj == nullptr)
            obj = new Config();
        return (*obj);
    }

    Jbool Process()
    {
        if (!this->Check())
            return false;
        return (this->mConfigAttr.isReady = this->Parse());
    }

    Config &Reset()
    {
        memset(&this->mConfigAttr, 0, sizeof(this->mConfigAttr));
        memset(&this->mConfigFileElement, 0, sizeof(this->mConfigFileElement));
        return (*this);
    }

    Config &WriteBuffer(const Jchar *v, Jint set)
    {
        if ((v == nullptr) || (this->mConfigAttr.cacheSet > static_cast<Jint>(sizeof(this->mConfigAttr.cache))))
            return (*this);

        memcpy(&this->mConfigAttr.cache[this->mConfigAttr.cacheSet], v, set);
        this->mConfigAttr.cacheSet += set;
        return (*this);
    }

    Config &Test()
    {
        Log::Instance().Print<LogType::INFO>(
            "-------------------------------- Config --------------------------------"
        );

        Log::Instance().Print<LogType::INFO>("%s: %s", "VerifyAddress", this->mConfigFileElement.verifyAddress);
        Log::Instance().Print<LogType::INFO>("%s: %d", "VerifyPort", this->mConfigFileElement.verifyPort);
        Log::Instance().Print<LogType::INFO>("%s: %d", "VerifyTimeouts", this->mConfigFileElement.verifyTimeouts);

        Log::Instance().Print<LogType::INFO>(
            "%s: %s",
            "CheckCustomerAddress",
            this->mConfigFileElement.checkCustomerAddress
        );
        Log::Instance().Print<LogType::INFO>(
            "%s: %d",
            "CheckCustomerPort",
            this->mConfigFileElement.checkCustomerPort
        );
        Log::Instance().Print<LogType::INFO>(
            "%s: %d",
            "CheckCustomerTimeouts",
            this->mConfigFileElement.checkCustomerTimeouts
        );

        Log::Instance().Print<LogType::INFO>("%s: %d", "Model", this->mConfigFileElement.model);
        Log::Instance().Print<LogType::INFO>("%s: %s", "Audio", (this->mConfigFileElement.audio ? "true" : "false"));

        Log::Instance().Print<LogType::INFO>(
            "%s: %s",
            "SignVerify",
            (this->mConfigFileElement.signVerify ? "true" : "false")
        );
        Log::Instance().Print<LogType::INFO>(
            "%s: %s",
            "PosVerify",
            (this->mConfigFileElement.posVerify ? "true" : "false")
        );
        Log::Instance().Print<LogType::INFO>(
            "%s: %s",
            "LaunchSensorCheck",
            (this->mConfigFileElement.launchSensorCheck ? "true" : "false")
        );
        Log::Instance().Print<LogType::INFO>(
            "%s: %s",
            "LaunchCheckCustomer",
            (this->mConfigFileElement.launchCheckCustomer ? "true" : "false")
        );

        Log::Instance().Print<LogType::INFO>("%s: %s", "Version", this->mConfigFileElement.version);
        return (*this);
    }

    const Jchar *GetRemoteActivationAddress()
    {
        return (this->mConfigAttr.isReady ? this->mConfigFileElement.verifyAddress : nullptr);
    }

    Jint GetRemoteActivationPort()
    {
        return (this->mConfigAttr.isReady ? this->mConfigFileElement.verifyPort : 0);
    }

    Jint GetRemoteActivationTimeouts()
    {
        return (this->mConfigAttr.isReady ? this->mConfigFileElement.verifyTimeouts : CONFIG_DEFAULT_TIMEOUTS);
    }

    const Jchar *GetCustomerCheckAddress()
    {
        return (this->mConfigAttr.isReady ? this->mConfigFileElement.checkCustomerAddress : nullptr);
    }

    Jint GetCustomerCheckPort()
    {
        return (this->mConfigAttr.isReady ? this->mConfigFileElement.checkCustomerPort : 0);
    }

    Jint GetCustomerCheckTimeouts()
    {
        return (this->mConfigAttr.isReady ? this->mConfigFileElement.checkCustomerTimeouts : CONFIG_DEFAULT_TIMEOUTS);
    }

    Jint GetModel()
    {
        return (this->mConfigAttr.isReady ? this->mConfigFileElement.model : 0);
    }

    Jbool GetWhetherToOpenAudio()
    {
        return (this->mConfigAttr.isReady ? this->mConfigFileElement.audio : false);
    }

    Jbool GetWhetherToOpenSignVerify()
    {
        return (this->mConfigAttr.isReady ? this->mConfigFileElement.signVerify : false);
    }

    Jbool GetWhetherToOpenDeviceTampered()
    {
        return (this->mConfigAttr.isReady ? this->mConfigFileElement.posVerify : false);
    }

    Jbool GetWhetherTheDeviceOnStartUpToBeCheckSensor()
    {
        return (this->mConfigAttr.isReady ? this->mConfigFileElement.launchSensorCheck : false);
    }

    Jbool GetWhetherToOpenCustomerCheck()
    {
        return (this->mConfigAttr.isReady ? this->mConfigFileElement.launchCheckCustomer : false);
    }

    const Jchar *GetVersion()
    {
        return (this->mConfigAttr.isReady ? this->mConfigFileElement.version : nullptr);
    }

private:
    ConfigAttr mConfigAttr;
    ConfigFileElement mConfigFileElement;
    BoostXMLSupport mBoostXMLSupport;

    Config() :
        mConfigAttr{},
        mConfigFileElement{},
        mBoostXMLSupport{}
    {}

    Jbool Check()
    {
        return (this->mConfigAttr.cacheSet > 0);
    }

    Jbool Parse()
    {
        this->mBoostXMLSupport.localXML.write(this->mConfigAttr.cache, this->mConfigAttr.cacheSet);
        boost::property_tree::read_xml(this->mBoostXMLSupport.localXML, this->mBoostXMLSupport.root);
        if (this->mBoostXMLSupport.root.get_child_optional(CONFIG_ELEMENT_ROOT).get_ptr() == nullptr)
            return false;

        if ((this->mBoostXMLSupport.root.get_optional<Jint>(
            CONFIG_ELEMENT_POS_VERIFY_TIMEOUTS_TAG
        ).get_ptr() == nullptr)
            || (this->mBoostXMLSupport.root.get_optional<Jint>(
            CONFIG_ELEMENT_POS_VERIFY_PORT_TAG
        ).get_ptr() == nullptr)
            || (this->mBoostXMLSupport.root.get_optional<std::string>(
            CONFIG_ELEMENT_POS_VERIFY_ADDRESS_TAG
        ).get_ptr() == nullptr))
            return false;

        auto &&verifyTimeouts = this->mBoostXMLSupport.root.get<Jint>(
            CONFIG_ELEMENT_POS_VERIFY_TIMEOUTS_TAG
        );
        auto &&verifyPort = this->mBoostXMLSupport.root.get<Jint>(
            CONFIG_ELEMENT_POS_VERIFY_PORT_TAG
        );
        auto &&verifyAddress = this->mBoostXMLSupport.root.get<std::string>(
            CONFIG_ELEMENT_POS_VERIFY_ADDRESS_TAG
        );

        if ((this->mBoostXMLSupport.root.get_optional<Jint>(
            CONFIG_ELEMENT_POS_CHECK_CUSTOMER_TIMEOUTS_TAG
        ).get_ptr() == nullptr)
            || (this->mBoostXMLSupport.root.get_optional<Jint>(
            CONFIG_ELEMENT_POS_CHECK_CUSTOMER_PORT_TAG
        ).get_ptr() == nullptr)
            || (this->mBoostXMLSupport.root.get_optional<std::string>(
            CONFIG_ELEMENT_POS_CHECK_CUSTOMER_ADDRESS_TAG
        ).get_ptr() == nullptr))
            return false;

        auto &&customerTimeout = this->mBoostXMLSupport.root.get<Jint>(
            CONFIG_ELEMENT_POS_CHECK_CUSTOMER_TIMEOUTS_TAG
        );
        auto &&customerPort = this->mBoostXMLSupport.root.get<Jint>(
            CONFIG_ELEMENT_POS_CHECK_CUSTOMER_PORT_TAG
        );
        auto &&customerAddress = this->mBoostXMLSupport.root.get<std::string>(
            CONFIG_ELEMENT_POS_CHECK_CUSTOMER_ADDRESS_TAG
        );

        if ((this->mBoostXMLSupport.root.get_optional<Jint>(CONFIG_ELEMENT_MODEL_TAG).get_ptr() == nullptr)
            || (this->mBoostXMLSupport.root.get_optional<Jbool>(CONFIG_ELEMENT_AUDIO_TAG).get_ptr() == nullptr)
            || (this->mBoostXMLSupport.root.get_optional<Jbool>(CONFIG_ELEMENT_SIGN_VERIFY).get_ptr() == nullptr)
            || (this->mBoostXMLSupport.root.get_optional<Jbool>(CONFIG_ELEMENT_POS_VERIFY).get_ptr() == nullptr))
            return false;

        auto &&model = this->mBoostXMLSupport.root.get<Jint>(CONFIG_ELEMENT_MODEL_TAG);
        auto &&audio = this->mBoostXMLSupport.root.get<Jbool>(CONFIG_ELEMENT_AUDIO_TAG);
        auto &&signVerify = this->mBoostXMLSupport.root.get<Jbool>(CONFIG_ELEMENT_SIGN_VERIFY);
        auto &&posVerify = this->mBoostXMLSupport.root.get<Jbool>(CONFIG_ELEMENT_POS_VERIFY);

        if ((this->mBoostXMLSupport.root.get_optional<Jbool>(
            CONFIG_ELEMENT_LAUNCH_SENSOR_CHECK_TAG
        ).get_ptr() == nullptr)
            || (this->mBoostXMLSupport.root.get_optional<Jbool>(
            CONFIG_ELEMENT_LAUNCH_CHECK_CUSTOMER_TAG
        ).get_ptr() == nullptr))
            return false;

        auto &&launchSensorCheck = this->mBoostXMLSupport.root.get<Jbool>(
            CONFIG_ELEMENT_LAUNCH_SENSOR_CHECK_TAG
        );
        auto &&launchCheckCustomer = this->mBoostXMLSupport.root.get<Jbool>(
            CONFIG_ELEMENT_LAUNCH_CHECK_CUSTOMER_TAG
        );

        if (this->mBoostXMLSupport.root.get_optional<std::string>(CONFIG_ELEMENT_VERSION_TAG).get_ptr() == nullptr)
            return false;

        auto &&version = this->mBoostXMLSupport.root.get<std::string>(CONFIG_ELEMENT_VERSION_TAG);

        this->mConfigFileElement.verifyTimeouts = verifyTimeouts;
        this->mConfigFileElement.verifyPort = verifyPort;
        memcpy(
            this->mConfigFileElement.verifyAddress,
            verifyAddress.c_str(),
            verifyAddress.size());

        this->mConfigFileElement.checkCustomerTimeouts = customerTimeout;
        this->mConfigFileElement.checkCustomerPort = customerPort;
        memcpy(
            this->mConfigFileElement.checkCustomerAddress,
            customerAddress.c_str(),
            customerAddress.size()
        );

        this->mConfigFileElement.model = model;
        this->mConfigFileElement.audio = audio;
        this->mConfigFileElement.signVerify = signVerify;
        this->mConfigFileElement.posVerify = posVerify;

        this->mConfigFileElement.launchSensorCheck = launchSensorCheck;
        this->mConfigFileElement.launchCheckCustomer = launchCheckCustomer;

        memcpy(this->mConfigFileElement.version, version.c_str(), version.size());
        return true;
    }
};

}

#endif //SMARTTOOLS_SMART_ANDROID_CONFIG_HPP
