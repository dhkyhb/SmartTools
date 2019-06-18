#pragma once

#ifndef SMARTTOOLS_SMART_UTILS_SIMPLE_HTTP_HPP
#define SMARTTOOLS_SMART_UTILS_SIMPLE_HTTP_HPP

#include "../kernel.hpp"

#include "smart_utils_strings.hpp"

namespace smart::utils::simple::http
{

using smart::utils::strings::Strings;

constexpr Jint HTTP_CACHE_SIZE = 4096;

enum class HttpMethod
{
    GET, POST,
};

typedef struct
{
    Jint code;
    char *bodyMark;

    const Jchar *host;
    const Jchar *connection;
    const Jchar *transaferEncoding;
    const Jchar *target;
    const Jchar *body;

    Jint cacheLen;
    Jchar cache[HTTP_CACHE_SIZE];

    HttpMethod method;
} HttpAttr;

constexpr Jchar HTTP_METHOD_GET[] = "GET";
constexpr Jchar HTTP_METHOD_POST[] = "POST";
constexpr Jchar HTTP_PROTOCOL_LEVEL[] = "HTTP/1.1";

constexpr Jchar HTTP_PROTOCOL_HEAD_TOP[] = "%s %s %s\r\n";
constexpr Jchar HTTP_PROTOCOL_HEAD_ARR[] = "%s: %s\r\n";

constexpr Jchar HTTP_PROTOCOL_HEAD_HOST[] = "Host";
constexpr Jchar HTTP_PROTOCOL_HEAD_CONTENT_TYPE[] = "Content-Type";
constexpr Jchar HTTP_PROTOCOL_HEAD_CONNECTION[] = "Connection";
constexpr Jchar HTTP_PROTOCOL_HEAD_TRANSFER_ENCODING[] = "Transfer-Encoding";

constexpr Jchar HTTP_PROTOCOL_HEAD_CONTENT_TYPE_DEFAULT[] = "application/x-www-form-urlencoded";
constexpr Jchar HTTP_PROTOCOL_HEAD_TRANSFER_ENCODEING_CHUNKED[] = "chunked";
constexpr Jchar HTTP_PROTOCOL_HEAD_CONNECTION_DEFAULT[] = "close";

constexpr Jchar HTTP_LINK_BODY_SYMBOL[] = "\r\n\r\n";
constexpr Jchar HTTP_LINK_SEGMENT_SYMBOL[] = "\r\n";
constexpr Jchar HTTP_LINK_ROW_SYMBOL[] = " ";
constexpr Jchar HTTP_LINK_HEAD_SYMBOL[] = ": ";

class Http
{
public:
    Http() :
        mHttpAttr{}
    {}

    ~Http() = default;

    Http &SetMethod(HttpMethod method)
    {
        this->mHttpAttr.method = method;
        return (*this);
    }

    Http &SetHost(const Jchar *host)
    {
        this->mHttpAttr.host = host;
        return (*this);
    }

    const Jchar *GetHost()
    {
        return this->mHttpAttr.host;
    }

    const Jchar *GetBody()
    {
        return this->mHttpAttr.body;
    }

    Http &SetTarget(const Jchar *target)
    {
        this->mHttpAttr.target = target;
        return (*this);
    }

    Http &Init()
    {
        memset(&this->mHttpAttr, 0, sizeof(this->mHttpAttr));
        return (*this);
    }

    const Jchar *Marshal()
    {
        auto &&segmentLinkSymbolLen = strlen(HTTP_LINK_SEGMENT_SYMBOL);

        if (!this->HeadProctocol())
            return nullptr;
        if (!this->HeadArray())
            return nullptr;
        if ((this->mHttpAttr.cacheLen + segmentLinkSymbolLen) > (sizeof(this->mHttpAttr.cache) - 1))
            return nullptr;

        memcpy(&this->mHttpAttr.cache[this->mHttpAttr.cacheLen], HTTP_LINK_SEGMENT_SYMBOL, segmentLinkSymbolLen);
        return this->mHttpAttr.cache;
    }

    Jbool Parse(const Jchar *v, Jint vLen)
    {
        if (vLen > static_cast<Jint>(sizeof(this->mHttpAttr.cache) - 1))
            return false;

        this->Init();
        memcpy(this->mHttpAttr.cache, v, vLen);
        this->mHttpAttr.cacheLen = vLen;

        if (!this->ParseHead())
            return false;
        if (!this->ParseBody())
            return false;
        return this->ParseProtocol();
    }

private:
    HttpAttr mHttpAttr;

    Jbool HeadArray()
    {
        Jint ret = 0;

        if (this->mHttpAttr.host != nullptr)
        {
            ret = snprintf(
                &this->mHttpAttr.cache[this->mHttpAttr.cacheLen],
                (sizeof(this->mHttpAttr.cache) - this->mHttpAttr.cacheLen),
                HTTP_PROTOCOL_HEAD_ARR,
                HTTP_PROTOCOL_HEAD_HOST,
                this->mHttpAttr.host
            );

            this->mHttpAttr.cacheLen += ret;
            if ((ret < 1) && (ret > static_cast<Jint>(sizeof(this->mHttpAttr.cache) - 1)))
                return false;
        }

        if (this->mHttpAttr.connection == nullptr)
            ret = snprintf(
                &this->mHttpAttr.cache[this->mHttpAttr.cacheLen],
                (sizeof(this->mHttpAttr.cache) - this->mHttpAttr.cacheLen),
                HTTP_PROTOCOL_HEAD_ARR,
                HTTP_PROTOCOL_HEAD_CONNECTION,
                HTTP_PROTOCOL_HEAD_CONNECTION_DEFAULT
            );
        else
            ret = snprintf(
                &this->mHttpAttr.cache[this->mHttpAttr.cacheLen],
                (sizeof(this->mHttpAttr.cache) - this->mHttpAttr.cacheLen),
                HTTP_PROTOCOL_HEAD_ARR,
                HTTP_PROTOCOL_HEAD_CONNECTION,
                this->mHttpAttr.connection
            );

        this->mHttpAttr.cacheLen += ret;
        if ((ret < 1) && (ret > static_cast<Jint>(sizeof(this->mHttpAttr.cache) - this->mHttpAttr.cacheLen - 1)))
            return false;

        ret = snprintf(
            &this->mHttpAttr.cache[this->mHttpAttr.cacheLen],
            (sizeof(this->mHttpAttr.cache) - this->mHttpAttr.cacheLen),
            HTTP_PROTOCOL_HEAD_ARR,
            HTTP_PROTOCOL_HEAD_CONTENT_TYPE,
            HTTP_PROTOCOL_HEAD_CONTENT_TYPE_DEFAULT
        );
        this->mHttpAttr.cacheLen += ret;
        return ((ret > 0) && (ret < static_cast<Jint>(sizeof(this->mHttpAttr.cache) - this->mHttpAttr.cacheLen - 1)));
    }

    Jbool HeadProctocol()
    {
        Jint ret = 0;

        if (this->mHttpAttr.method == HttpMethod::POST)
            ret = snprintf(
                &this->mHttpAttr.cache[this->mHttpAttr.cacheLen],
                (sizeof(this->mHttpAttr.cache) - this->mHttpAttr.cacheLen),
                HTTP_PROTOCOL_HEAD_TOP,
                HTTP_METHOD_POST,
                this->mHttpAttr.target,
                HTTP_PROTOCOL_LEVEL
            );
        else if (this->mHttpAttr.method == HttpMethod::GET)
            ret = snprintf(
                &this->mHttpAttr.cache[this->mHttpAttr.cacheLen],
                (sizeof(this->mHttpAttr.cache) - this->mHttpAttr.cacheLen),
                HTTP_PROTOCOL_HEAD_TOP,
                HTTP_METHOD_GET,
                this->mHttpAttr.target,
                HTTP_PROTOCOL_LEVEL
            );
        else
            return false;

        this->mHttpAttr.cacheLen += ret;
        return ((ret > 0) && (ret < static_cast<Jint>(sizeof(this->mHttpAttr.cache) - 1)));
    }

    Jbool ParseBody()
    {
        Jint i = 0;
        Jint backupI = 0;
        Jint bodyLen = 0;

        auto &&httpLinkSegmentSymbolLen = strlen(HTTP_LINK_SEGMENT_SYMBOL);
        auto &&httpLinkBodySymbolLen = strlen(HTTP_LINK_BODY_SYMBOL);
        i += (this->mHttpAttr.bodyMark - this->mHttpAttr.cache + httpLinkBodySymbolLen);

        if ((this->mHttpAttr.transaferEncoding != nullptr)
            && (Strings::Equals(this->mHttpAttr.transaferEncoding, HTTP_PROTOCOL_HEAD_TRANSFER_ENCODEING_CHUNKED)))
        {
            backupI = i;
            auto &&body = strstr(&this->mHttpAttr.cache[i], HTTP_LINK_SEGMENT_SYMBOL);
            if (body == nullptr)
                return false;

            i = body - this->mHttpAttr.cache;
            this->mHttpAttr.cache[i] = 0x00;
            i += httpLinkSegmentSymbolLen;

            auto &&checkValueLen = strlen(&this->mHttpAttr.cache[backupI]);
            if (checkValueLen > 2)
                bodyLen = Strings::HexString2Int32(&this->mHttpAttr.cache[backupI]);
            else
                bodyLen = Strings::HexString2Int16(&this->mHttpAttr.cache[backupI]);

            this->mHttpAttr.body = &this->mHttpAttr.cache[i];
            i += bodyLen + 1;
            this->mHttpAttr.cache[i] = 0x00;
        } else
        {
            this->mHttpAttr.body = &this->mHttpAttr.cache[i];
        }
        return true;
    }

    Jbool ParseHead()
    {
        Jint i = 0;
        Jint backupI = 0;
        Jint headKV = 0;

        auto &&httpLinkSegmentSymbolLen = strlen(HTTP_LINK_SEGMENT_SYMBOL);
        auto &&httpLinkHeadSymbolLen = strlen(HTTP_LINK_HEAD_SYMBOL);
        auto &&protocol = strstr(this->mHttpAttr.cache, HTTP_LINK_SEGMENT_SYMBOL);
        if (protocol == nullptr)
            return false;

        this->mHttpAttr.bodyMark = strstr(this->mHttpAttr.cache, HTTP_LINK_BODY_SYMBOL);
        if (this->mHttpAttr.bodyMark == nullptr)
            return false;

        i = protocol - this->mHttpAttr.cache + httpLinkSegmentSymbolLen;
        while (i < static_cast<Jint>(this->mHttpAttr.bodyMark - this->mHttpAttr.cache))
        {
            backupI = i;
            auto &&segment = strstr(&this->mHttpAttr.cache[i], HTTP_LINK_SEGMENT_SYMBOL);
            if (segment == nullptr)
                break;

            i = segment - this->mHttpAttr.cache;
            this->mHttpAttr.cache[i] = 0x00;
            i += httpLinkSegmentSymbolLen;

            auto &&kv = strstr(&this->mHttpAttr.cache[backupI], HTTP_LINK_HEAD_SYMBOL);
            if (kv == nullptr)
                continue;

            headKV = kv - this->mHttpAttr.cache;
            this->mHttpAttr.cache[headKV] = 0x0;
            if (Strings::Equals(&this->mHttpAttr.cache[backupI], HTTP_PROTOCOL_HEAD_CONNECTION))
                this->mHttpAttr.connection = &this->mHttpAttr.cache[headKV + httpLinkHeadSymbolLen];
            else if (Strings::Equals(&this->mHttpAttr.cache[backupI], HTTP_PROTOCOL_HEAD_HOST))
                this->mHttpAttr.host = &this->mHttpAttr.cache[headKV + httpLinkHeadSymbolLen];
            else if (Strings::Equals(&this->mHttpAttr.cache[backupI], HTTP_PROTOCOL_HEAD_TRANSFER_ENCODING))
                this->mHttpAttr.transaferEncoding = &this->mHttpAttr.cache[headKV + httpLinkHeadSymbolLen];
        }

        return true;
    }

    Jbool ParseProtocol()
    {
        Jint i = 0;
        Jint step = 0;
        Jint point = 0;
        Jint codePoint = 0;
        Jbool state = false;

        auto &&httpLinkRowSymbol = strlen(HTTP_LINK_ROW_SYMBOL);
        auto &&protocol = strstr(this->mHttpAttr.cache, HTTP_LINK_SEGMENT_SYMBOL);
        if (protocol == nullptr)
            return false;

        for (i = 0; i < 2; ++i)
        {
            auto segment = strstr(&this->mHttpAttr.cache[step], HTTP_LINK_ROW_SYMBOL);
            if (segment == nullptr)
                break;

            point = segment - this->mHttpAttr.cache;
            if (i == 0)
                codePoint = point;

            this->mHttpAttr.cache[point] = 0x00;
            step += (point + httpLinkRowSymbol);
        }

        if (codePoint != 0)
        {
            this->mHttpAttr.code = atoi(&this->mHttpAttr.cache[codePoint + httpLinkRowSymbol]);
            state = true;
        }

        return state;
    }

};

}

#endif //SMARTTOOLS_SMART_UTILS_SIMPLE_HTTP_HPP
