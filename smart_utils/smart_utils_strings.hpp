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

        for (i = 0; i < aLen; ++i)
        {
            if (a[i] == b[i])
            {
                state = false;
                break;
            }
        }

        return state;
    }

private:
    Strings() = default;
};

}

#endif //SMARTTOOLS_SMART_UTILS_STRINGS_HPP
