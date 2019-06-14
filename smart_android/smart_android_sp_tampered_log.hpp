#ifndef SMARTTOOLS_SMART_ANDROID_SP_TAMPERED_LOG_HPP
#define SMARTTOOLS_SMART_ANDROID_SP_TAMPERED_LOG_HPP

#include "../kernel.hpp"
#include "../smart_utils/smart_utils_strings.hpp"
#include "../smart_resouces/smart_resouces_errors.hpp"

#include <APosAccessoryManager.h>

namespace smart::android::sp::tampered::log
{

using smart::utils::strings::Strings;
using smart::resouces::errors::Errors;
using smart::resouces::errors::ErrorsType;

constexpr Jint SP_TAMPERED_LOG_COMMAND_SIZE = 1024;
constexpr Jint SP_TAMPERED_LOG_RECORD_TIME_SIZE = 32;
constexpr Jint SP_TAMPERED_LOG_RECORD_MAX_SIZE = 32;

enum class SPTamperedLogType
{
    SENSOR_ONE = 2,
    SENSOR_TWO = 4,
    SENSOR_THREE = 8,
    SENSOR_FOUR = 16,

    OTHER_FORMAT_PED = 0,
    OTHER_UNLOCK_PED,
    OTHER_READ_PED_IMAGE_ERROR,
    OTHER_SEK_INVALID,
    OTHER_SEK_CRC_ERROR,
    OTHER_SEK_ZERO,
    OTHER_PED_FILE_NOT_EXIST,
    OTHER_MKSK_INVALID,
    OTHER_MKSK_ATTR_ERROR,
    OTHER_MKSK_CRC_ERROR,
    OTHER_SSAK_ATTR_ERROR,
    OTHER_SSAK_AUTH_ERR_OVERFLOW,
    OTHER_DUKPT_ATTR_ERROR,
    OTHER_DUKPT_CRC_ERROR,
    OTHER_DUKPT_FUTURE_KEY_CRC_ERROR,
    OTHER_DES_KEY_ATTR_ERROR,
    OTHER_DES_KEY_CRC_ERROR,
    OTHER_FIRMWARE_SIG_ERR,
    OTHER_PED_CFG_SIG_ERROR,
    OTHER_AUTH_FAIL_EXCEED,
    OTHER_RSA_KEY_ATTR_ERROR,
    OTHER_RSA_KEY_CRC_ERROR,
    OTHER_ADMIN_PWD_CHECK_FAIL,
    OTHER_USPUK_SIG_ERR,
    OTHER_APP_SIG_ERR,
};

enum class SPTamperedLogCategory
{
    SENSOR = 0,
    KEY_VERIFY_ERR,
    UNLOCK,
    SOFTWARE,
};

typedef struct
{
    Jchar time[SP_TAMPERED_LOG_RECORD_TIME_SIZE];
    SPTamperedLogCategory category;
    Juint type;
} SPTamperedLogRecord;

typedef struct
{
    Jint commandStep;
    Jint commandLen;
    Jbyte command[SP_TAMPERED_LOG_COMMAND_SIZE];

    Jint recordLen;
    SPTamperedLogRecord records[SP_TAMPERED_LOG_RECORD_MAX_SIZE];
    Jchar tmpCategoryStr[SP_TAMPERED_LOG_RECORD_TIME_SIZE];
    Jchar tmpTypeStr[SP_TAMPERED_LOG_RECORD_TIME_SIZE];
} SPTamperedLogAttr;

typedef struct
{
    Jint logLen;
    Jchar *log;
    APosAccessoryManager *context;

    std::mutex posMutex;
} SPTamperedLogPOSSupport;

constexpr Jbyte SP_TAMPERED_LOG_COMMAND_TYPE = 0x87;
constexpr Jint SP_TAMPERED_LOG_COMMAND_VALUE_LENGTH = 4;

constexpr Jbyte SENSOR_ROUTE_CODE_ONE_VALUE = 0x02;
constexpr Jbyte SENSOR_ROUTE_CODE_TWO_VALUE = 0x04;
constexpr Jbyte SENSOR_ROUTE_CODE_TREE_VALUE = 0x08;
constexpr Jbyte SENSOR_ROUTE_CODE_FOUR_VALUE = 0x10;

class SPTamperedLog
{
public:
    static SPTamperedLog &Instance()
    {
        static SPTamperedLog *obj = nullptr;
        if (obj == nullptr)
            obj = new SPTamperedLog();
        return (*obj);
    }

    Jbool Process()
    {
        Jbool state = false;

        do
        {
            if (!this->POSInit())
                break;
            if (!this->TamperedLogReader())
                break;
            if (!this->TamperedLogParse())
                break;

            state = true;
        } while (false);

        if ((!state) && (Errors::Instance().GetErrorType() != ErrorsType::POS_INVALID))
            Errors::Instance().SetErrorType<ErrorsType::POS_NEED_SHORT_SMALL_BATTERY_OR_REBOOT>();

        this->POSRelease();
        return state;
    }

    const SPTamperedLogRecord *GetRecords()
    {
        return this->mSPTamperedLogAttr.records;
    }

    Jint GetRecordLength()
    {
        return this->mSPTamperedLogAttr.recordLen;
    }

private:
    SPTamperedLogAttr mSPTamperedLogAttr;
    SPTamperedLogPOSSupport mSPTamperedLogPOSSupport;

    SPTamperedLog() :
        mSPTamperedLogAttr{},
        mSPTamperedLogPOSSupport{}
    {}

    Jbool POSInit()
    {
        this->mSPTamperedLogPOSSupport.posMutex.lock();
        memset(&this->mSPTamperedLogPOSSupport, 0, sizeof(this->mSPTamperedLogPOSSupport));
        this->mSPTamperedLogPOSSupport.context = APosAccessoryManager_new();
        return (this->mSPTamperedLogPOSSupport.context != nullptr);
    }

    void POSRelease()
    {
        if (this->mSPTamperedLogPOSSupport.log != nullptr)
            free(this->mSPTamperedLogPOSSupport.log);
        if (this->mSPTamperedLogPOSSupport.context != nullptr)
            APosAccessoryManager_delete(this->mSPTamperedLogPOSSupport.context);

        this->mSPTamperedLogPOSSupport.posMutex.unlock();
    }

    Jbool TamperedLogReader()
    {
        auto &&ret = APosAccessoryManager_getAttackedLogs(
            this->mSPTamperedLogPOSSupport.context,
            nullptr,
            0,
            &this->mSPTamperedLogPOSSupport.log,
            &this->mSPTamperedLogPOSSupport.logLen
        );

        if (ret == -1)
        {
            Errors::Instance().SetErrorType<ErrorsType::POS_INVALID>();
            return false;
        }

        Log::Instance().Print<LogType::DEBUG>("APosAccessoryManager_getAttackedLogs ret: %d", ret);
        if ((this->mSPTamperedLogPOSSupport.log == nullptr) || (this->mSPTamperedLogPOSSupport.logLen < 1))
            return false;

        Log::Instance().Print<LogType::DEBUG>("obtained binary tampered data:");
        memset(this->mSPTamperedLogAttr.command, 0, sizeof(this->mSPTamperedLogAttr.command));
        memcpy(
            this->mSPTamperedLogAttr.command,
            this->mSPTamperedLogPOSSupport.log,
            this->mSPTamperedLogPOSSupport.logLen
        );
        Log::Instance().PrintHex(
            reinterpret_cast<Jbyte *>(this->mSPTamperedLogPOSSupport.log),
            this->mSPTamperedLogPOSSupport.logLen
        );
        return (ret == 0);
    }

    Jbool TamperedLogParse()
    {
        Jint i = 0;
        Jint j = 0;

        this->mSPTamperedLogAttr.recordLen = 0;
        this->mSPTamperedLogAttr.commandStep = 0;
        if (this->mSPTamperedLogAttr.command[this->mSPTamperedLogAttr.commandStep] != SP_TAMPERED_LOG_COMMAND_TYPE)
            return false;

        if (this->mSPTamperedLogAttr.commandLen = Strings::LittleHex2Int32(
                &this->mSPTamperedLogAttr.command[++this->mSPTamperedLogAttr.commandStep],
                SP_TAMPERED_LOG_COMMAND_VALUE_LENGTH
            );this->mSPTamperedLogAttr.commandLen < 1)
            return false;

        this->mSPTamperedLogAttr.commandStep += SP_TAMPERED_LOG_COMMAND_VALUE_LENGTH;
        Log::Instance().Print<LogType::DEBUG>("command length: %d", this->mSPTamperedLogAttr.commandLen);
        Log::Instance().Print<LogType::DEBUG>("  Time | Category | Content");
        for (i = 0; i < this->mSPTamperedLogAttr.commandLen; i += 16)
        {
            this->HexTimeConvToFormatStringTimeAndCategoryAndType();

            Strings::Bytes2String(
                &this->mSPTamperedLogAttr.command[this->mSPTamperedLogAttr.commandStep - 8],
                4,
                this->mSPTamperedLogAttr.tmpCategoryStr,
                sizeof(this->mSPTamperedLogAttr.tmpCategoryStr)
            );
            Strings::Bytes2String(
                &this->mSPTamperedLogAttr.command[this->mSPTamperedLogAttr.commandStep - 4],
                4,
                this->mSPTamperedLogAttr.tmpTypeStr,
                sizeof(this->mSPTamperedLogAttr.tmpTypeStr)
            );
            Log::Instance().Print<LogType::DEBUG>(
                "%s | %s | %s",
                this->mSPTamperedLogAttr.records[j].time,
                this->mSPTamperedLogAttr.tmpCategoryStr,
                this->mSPTamperedLogAttr.tmpTypeStr
            );

            ++j;
        }
        return true;
    }

    void HexTimeConvToFormatStringTimeAndCategoryAndType()
    {
        Jint i = 0;
        Jint mark = 0;

        if ((this->mSPTamperedLogAttr.recordLen + 1) > SP_TAMPERED_LOG_RECORD_MAX_SIZE)
            return;

        for (i = 0; i < 6; ++i)
        {
            Strings::Bytes2String(
                &this->mSPTamperedLogAttr.command[this->mSPTamperedLogAttr.commandStep],
                1,
                &this->mSPTamperedLogAttr.records[this->mSPTamperedLogAttr.recordLen].time[mark],
                (sizeof(this->mSPTamperedLogAttr.records[this->mSPTamperedLogAttr.recordLen].time) - mark)
            );

            mark += 2;
            if ((mark == 2) || (mark == 5))
                this->mSPTamperedLogAttr.records[this->mSPTamperedLogAttr.recordLen].time[mark] = '-';
            else if (mark == 8)
                this->mSPTamperedLogAttr.records[this->mSPTamperedLogAttr.recordLen].time[mark] = ' ';
            else if ((mark == 11) || (mark == 14))
                this->mSPTamperedLogAttr.records[this->mSPTamperedLogAttr.recordLen].time[mark] = ':';
            ++mark;
            ++this->mSPTamperedLogAttr.commandStep;
        }

        this->mSPTamperedLogAttr.commandStep += 2;
        i = this->mSPTamperedLogAttr.command[this->mSPTamperedLogAttr.commandStep] << 24u;
        i += this->mSPTamperedLogAttr.command[++this->mSPTamperedLogAttr.commandStep] << 16u;
        i += this->mSPTamperedLogAttr.command[++this->mSPTamperedLogAttr.commandStep] << 8u;
        i += this->mSPTamperedLogAttr.command[++this->mSPTamperedLogAttr.commandStep];
        ++this->mSPTamperedLogAttr.commandStep;

        this->mSPTamperedLogAttr.records[this->mSPTamperedLogAttr.recordLen].category =
            static_cast<SPTamperedLogCategory>(i);

        if (static_cast<SPTamperedLogCategory>(i) != SPTamperedLogCategory::SENSOR)
        {
            i = this->mSPTamperedLogAttr.command[this->mSPTamperedLogAttr.commandStep] << 24u;
            i += this->mSPTamperedLogAttr.command[++this->mSPTamperedLogAttr.commandStep] << 16u;
            i += this->mSPTamperedLogAttr.command[++this->mSPTamperedLogAttr.commandStep] << 8u;
            i += this->mSPTamperedLogAttr.command[++this->mSPTamperedLogAttr.commandStep];
            ++this->mSPTamperedLogAttr.commandStep;
            this->mSPTamperedLogAttr.records[this->mSPTamperedLogAttr.recordLen].type = i;
        } else
        {
            i = this->mSPTamperedLogAttr.command[this->mSPTamperedLogAttr.commandStep] << 24u;
            i += this->mSPTamperedLogAttr.command[++this->mSPTamperedLogAttr.commandStep] << 16u;
            i += this->mSPTamperedLogAttr.command[++this->mSPTamperedLogAttr.commandStep] << 8u;
            i += this->mSPTamperedLogAttr.command[++this->mSPTamperedLogAttr.commandStep];
            ++this->mSPTamperedLogAttr.commandStep;
            this->mSPTamperedLogAttr.records[this->mSPTamperedLogAttr.recordLen].type = 0;
            if ((i & SENSOR_ROUTE_CODE_ONE_VALUE) == SENSOR_ROUTE_CODE_ONE_VALUE)
                this->mSPTamperedLogAttr.records[this->mSPTamperedLogAttr.recordLen].type |=
                    static_cast<Juint>(SPTamperedLogType::SENSOR_ONE);
            if ((i & SENSOR_ROUTE_CODE_TWO_VALUE) == SENSOR_ROUTE_CODE_TWO_VALUE)
                this->mSPTamperedLogAttr.records[this->mSPTamperedLogAttr.recordLen].type |=
                    static_cast<Juint>(SPTamperedLogType::SENSOR_TWO);
            if ((i & SENSOR_ROUTE_CODE_TREE_VALUE) == SENSOR_ROUTE_CODE_TREE_VALUE)
                this->mSPTamperedLogAttr.records[this->mSPTamperedLogAttr.recordLen].type |=
                    static_cast<Juint>(SPTamperedLogType::SENSOR_THREE);
            if ((i & SENSOR_ROUTE_CODE_FOUR_VALUE) == SENSOR_ROUTE_CODE_FOUR_VALUE)
                this->mSPTamperedLogAttr.records[this->mSPTamperedLogAttr.recordLen].type |=
                    static_cast<Juint>(SPTamperedLogType::SENSOR_FOUR);
        }

        ++this->mSPTamperedLogAttr.recordLen;
    }
};

}

#endif //SMARTTOOLS_SMART_ANDROID_SP_TAMPERED_LOG_HPP
