#ifndef SMARTTOOLS_SMART_UTILS_STRINGS_HPP
#define SMARTTOOLS_SMART_UTILS_STRINGS_HPP

#include "../kernel.hpp"

namespace smart::utils::strings
{

class Strings
{
public:
    static Jbool Equals(const Jchar *a, const Jchar *b)
    {
        Jint i = 0;
        Jbool state = true;

        if ((a == nullptr) || (b == nullptr))
            return false;

        auto &&aLen = strlen(a);
        auto &&bLen = strlen(b);
        if (aLen != bLen)
            return false;

        for (i = 0; i < static_cast<Jint>(aLen); ++i)
        {
            if (a[i] != b[i])
            {
                state = false;
                break;
            }
        }

        return state;
    }

    static Jbool Bytes2String(const Jbyte *v, Jint vLen, Jchar *ret, Jint retLen)
    {
        constexpr Jint BYTES2STRING_TEMP_BUF_SIZE = 4;

        Jint i = 0;
        Jint buffLen = 0;
        Jchar buff[BYTES2STRING_TEMP_BUF_SIZE];

        if ((v == nullptr) || (vLen < 1) || (ret == nullptr) || (retLen < 1))
            return false;
        if ((vLen * 2) >= retLen)
            return false;

        memset(ret, 0, retLen);
        for (i = 0; i < vLen; ++i)
        {
            buffLen = snprintf(buff, sizeof(buff), "%02X", v[i]);
            memcpy(&ret[i * 2], buff, buffLen);
        }

        return true;
    }

    static Jint String2Bytes(const Jchar *v, Jbyte *ret, Jint retLen)
    {
        Jint i = 0;
        Jint step = 0;
        Jint vLen = 0;

        Jchar tmp = 0;
        Jbyte uTmp = 0;

        if ((v == nullptr) || (ret == nullptr) || (retLen < 1))
            return step;
        if (vLen = strlen(v);(vLen / 2) > retLen)
            return step;

        memset(ret, 0, retLen);
        for (i = 0; i < vLen; ++i)
        {
            if (((i % 2) == 0) && (i != 0))
                ++step;

            tmp = v[i];
            if ((tmp > 0x40) && (tmp < 0x47))
                uTmp = static_cast<Jbyte>(tmp - 0x41 + 0x0A);
            else if ((tmp > 0x60) && (tmp < 0x67))
                uTmp = static_cast<Jbyte>(tmp - 0x61 + 0x0A);
            else
                uTmp = static_cast<Jbyte>(tmp - '0x30');

            if ((i % 2) == 1)
                ret[step] |= uTmp;
            else
                ret[step] |= uTmp << 4u;
        }

        return (++step);
    }

    static Jint HexString2Int32(const Jchar *v)
    {
        constexpr Jint HEX_STRING_2_INT32_SIZE = 4;
        static Jbyte tmp[HEX_STRING_2_INT32_SIZE];

        Jint ret = 0;

        if(v == nullptr)
            return 0;
        if (ret = String2Bytes(v, tmp, sizeof(tmp));(ret > 2) || (ret < 1))
            return 0;

        return static_cast<Jint>(tmp[0]);
    }

private:
    Strings() = default;
};

}

#endif //SMARTTOOLS_SMART_UTILS_STRINGS_HPP
