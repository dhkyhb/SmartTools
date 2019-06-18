#pragma once

#ifndef SMARTTOOLS_SMART_RESOUCES_ERRORS_HPP
#define SMARTTOOLS_SMART_RESOUCES_ERRORS_HPP

namespace smart::resouces::errors
{

enum class ErrorsType
{
    SUCCESS = 0,

    CONFIG_INVALID,

    SERVER_REQUEST_TIMEOUTS,
    SERVER_REQUEST_FAIL,

    POS_NEED_SHORT_SMALL_BATTERY_OR_REBOOT,
    POS_INVALID,
};

typedef struct
{
    ErrorsType type;
} ErrorsAttr;

class Errors
{
public:
    static Errors &Instance()
    {
        static Errors *obj = nullptr;
        if (obj == nullptr)
            obj = new Errors();
        return (*obj);
    }

    template<ErrorsType _type>
    Errors &SetErrorType()
    {
        this->mErrorsAttr.type = _type;
        return (*this);
    }

    ErrorsType GetErrorType()
    {
        return this->mErrorsAttr.type;
    }

private:
    ErrorsAttr mErrorsAttr;

    Errors() :
        mErrorsAttr{}
    {}
};

}

#endif //SMARTTOOLS_SMART_RESOUCES_ERRORS_HPP
