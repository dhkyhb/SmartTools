#ifndef SMARTTOOLS_SMART_ANDROID_BACKUP_TAMPERED_INFORMATION_HPP
#define SMARTTOOLS_SMART_ANDROID_BACKUP_TAMPERED_INFORMATION_HPP

#include "../kernel.hpp"

namespace smart::android::backup::tampered::information
{

class BackupTamperedInformation
{
public:
    static BackupTamperedInformation &Instance()
    {
        static BackupTamperedInformation *obj = nullptr;
        if (obj == nullptr)
            obj = new BackupTamperedInformation();
        return (*obj);
    }

private:

    BackupTamperedInformation() = default;
};

}

#endif //SMARTTOOLS_SMART_ANDROID_BACKUP_TAMPERED_INFORMATION_HPP
