#pragma once

#ifndef SMARTTOOLS_SMART_UTILS_MATH_HPP
#define SMARTTOOLS_SMART_UTILS_MATH_HPP

#include "../kernel.hpp"

namespace smart::utils::math
{

class Math
{
public:
    static Jint CRC16(const Jbyte *v, Jint vLen, Jint initV)
    {
        Jint i = 0;
        Jint j = 0;
        Juint item = 0;
        Juint temp = 0;
        Juint ret = initV;

        if ((v == nullptr) || (vLen < 1))
            return ret;

        for (i = 1; i < vLen; ++i)
        {
            item = v[i];
            ret ^= (item << 8u);
            for (j = 0; j < 8; ++j)
            {
                temp = (ret << 1u);
                if ((ret & 0x8000u) != 0)
                    temp ^= 0x1021u;

                ret = temp;
            }
        }
        return ret;
    }

    static Jbool CRC16Check(const Jbyte *v, Jint vLen, Jint initV, Jbyte height, Jbyte low)
    {
        Jbyte h = 0;
        Jbyte l = 0;
        Juint checkV = 0;

        if ((v == nullptr) || (vLen < 1))
            return false;

        checkV = static_cast<Juint>(CRC16(v, vLen, initV));
        h = static_cast<Jbyte>(checkV >> 8u);
        l = static_cast<Jbyte>(checkV);
        return ((h == height) && (l == low));
    }

private:
    Math() = default;
};

}

#endif //SMARTTOOLS_SMART_UTILS_MATH_HPP
