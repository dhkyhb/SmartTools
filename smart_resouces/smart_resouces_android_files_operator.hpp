#pragma once

#ifndef SMARTTOOLS_SMART_RESOUCES_ANDROID_FILES_OPERATOR_HPP
#define SMARTTOOLS_SMART_RESOUCES_ANDROID_FILES_OPERATOR_HPP

#include "../kernel.hpp"

namespace smart::resouces::android::files
{

enum class AndroidFilesType
{
    CUSTOMER, SUB_CUSTOMER,
};

enum class AndroidFilesOperation
{
    DELETE,
};

typedef struct
{
    AndroidFilesType type;
    AndroidFilesOperation operation;

    std::mutex attrLock;
} AndroidFilesAttr;

typedef struct
{
    AndroidFilesType type;
    const char *path;
} AndroidFilesManager;

constexpr AndroidFilesManager Manager[] = {
    {AndroidFilesType::CUSTOMER,     "/persist/customer/customer_name.txt"},
    {AndroidFilesType::SUB_CUSTOMER, "/persist/customer/customer_sub_name.txt"},
};

template<AndroidFilesType _type>
constexpr const char *GetPathOnAndroidFilesManager()
{
    for (auto manager : Manager)
    {
        if (manager.type == _type)
            return manager.path;
    }

    return nullptr;
}

class AndroidFiles
{
public:
    static AndroidFiles &Instance()
    {
        static AndroidFiles *obj = nullptr;
        if (obj == nullptr)
            obj = new AndroidFiles();
        return (*obj);
    }

    AndroidFiles &Ready()
    {
        this->mAndroidFilesAttr.attrLock.lock();
        return (*this);
    }

    void Execute()
    {
        this->SelectFile();
        this->mAndroidFilesAttr.attrLock.unlock();
    }

    template<AndroidFilesType _type>
    AndroidFiles &SetType()
    {
        this->mAndroidFilesAttr.type = _type;
        return (*this);
    }

    template<AndroidFilesOperation _operation>
    AndroidFiles &SetOperation()
    {
        this->mAndroidFilesAttr.operation = _operation;
        return (*this);
    }

private:
    AndroidFilesAttr mAndroidFilesAttr;

    AndroidFiles() :
        mAndroidFilesAttr{}
    {}

    void SelectFile()
    {
        if (this->mAndroidFilesAttr.type == AndroidFilesType::CUSTOMER)
            this->OperationCustomer();
        else if (this->mAndroidFilesAttr.type == AndroidFilesType::SUB_CUSTOMER)
            this->OperationSubCustomer();
    }

    void OperationCustomer()
    {
        auto &&path = GetPathOnAndroidFilesManager<AndroidFilesType::CUSTOMER>();
        if (path == nullptr)
            return;

        if (this->mAndroidFilesAttr.operation == AndroidFilesOperation::DELETE)
            Log::Instance().Print<LogType::DEBUG>("remove customer, ret: %d", remove(path));
    }

    void OperationSubCustomer()
    {
        auto &&path = GetPathOnAndroidFilesManager<AndroidFilesType::SUB_CUSTOMER>();
        if (path == nullptr)
            return;

        if (this->mAndroidFilesAttr.operation == AndroidFilesOperation::DELETE)
            Log::Instance().Print<LogType::DEBUG>("remove sub customer, ret: %d", remove(path));
    }
};

}

#endif //SMARTTOOLS_SMART_RESOUCES_ANDROID_FILES_OPERATOR_HPP
