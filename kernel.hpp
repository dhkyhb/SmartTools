#pragma once

#ifndef SMARTTOOLS_KERNEL_HPP
#define SMARTTOOLS_KERNEL_HPP

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <time.h>

#include <stdint.h>

using Jsize = size_t;
using Jbool = bool;
using Jbyte = uint8_t;
using Juint = uint32_t;
using Julong = uint64_t;
using Jchar = char;
using Jint = int32_t;
using Jlong = int64_t;

void *operator new(size_t len)
{
    auto *obj = malloc(len);
    if (obj != nullptr)
        memset(obj, 0, len);
    return obj;
}

void operator delete(void *v) noexcept
{
    if (v != nullptr)
        free(v);
}

void *operator new[](size_t len)
{
    auto *obj = malloc(len);
    if (obj != nullptr)
        memset(obj, 0, len);
    return obj;
}

void operator delete[](void *obj) noexcept
{
    if (obj != nullptr)
        free(obj);
}

namespace kernel
{

template<typename T>
using ItType = T;

}

#endif //SMARTTOOLS_KERNEL_HPP
