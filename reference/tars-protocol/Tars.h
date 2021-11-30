#ifndef __TARS_H__
#define __TARS_H__

#include <string.h>
#include <stdint.h>

#include <vector>

#include "TarsType.h"

struct TarsProtoException : public std::runtime_error
{
    TarsProtoException(const std::string &s) : std::runtime_error(s) {}
};

struct TarsEncodeException : public TarsProtoException
{
    TarsEncodeException(const std::string &s) : TarsProtoException(s) {}
};

struct TarsDecodeException : public TarsProtoException
{
    TarsDecodeException(const std::string &s) : TarsProtoException(s) {}
};

struct TarsDecodeMismatch : public TarsDecodeException
{
    TarsDecodeMismatch(const std::string &s) : TarsDecodeException(s) {}
};

struct TarsDecodeRequireNotExist : public TarsDecodeException
{
    TarsDecodeRequireNotExist(const std::string &s) : TarsDecodeException(s) {}
};

struct TarsDecodeInvalidValue : public TarsDecodeException
{
    TarsDecodeInvalidValue(const std::string &s) : TarsDecodeException(s) {}
};

struct TarsNotEnoughBuff : public TarsProtoException
{
    TarsNotEnoughBuff(const std::string &s) : TarsProtoException(s) {}
};

//解码相应的宏

//获取第一个字节以类型type放入buf
#define TarsPeekTypeBuf(is, buf, offset, type)                                                       \
    if ((is)._cur + (offset) + sizeof(type) > (is)._buf_len)                                         \
    {                                                                                                \
        char s[64];                                                                                  \
        snprintf(s, sizeof(s), "buffer overflow when peekBuf, over %u.", (uint32_t)((is)._buf_len)); \
        throw TarsDecodeException(s);                                                                \
    }                                                                                                \
    (buf) = (*((type *)((is)._buf + (is)._cur + (offset))));

//获取tag以及type分别存入
#define TarsPeekFromHead(is, type, tag, n)    \
    (n) = 1;                                  \
    uint8_t typeTag, tmpTag;                  \
    TarsPeekTypeBuf(is, typeTag, 0, uint8_t); \
    tmpTag = typeTag >> 4;                    \
    (type) = (typeTag & 0x0F);                \
    if (tmpTag == 15)                         \
    {                                         \
        TarsPeekTypeBuf(is, tag, 1, uint8_t); \
        (n) += 1;                             \
    }                                         \
    else                                      \
    {                                         \
        (tag) = tmpTag;                       \
    }

//跳过长度为len的head
#define TarsReadHeadSkip(is, len) \
    (is)._cur += (len);

//获取type存入
#define TarsPeekFromHeadNoTag(is, type, n)        \
    (n) = 1;                                      \
    uint8_t typeTag, tmpTag;                      \
    TarsPeekTypeBuf(is, typeTag, 0, uint8_t);     \
    tmpTag = typeTag >> 4;                        \
    (type) = (typeTag & 0x0F);                    \
    if (tmpTag == 15)                             \
    {                                             \
        TarsPeekTypeBuf(is, typeTag, 1, uint8_t); \
        (n) += 1;                                 \
    }

//获取type存入,跳过长度为len的head
#define readFromHeadNoTag(is, type)     \
    size_t n = 0;                       \
    TarsPeekFromHeadNoTag(is, type, n); \
    TarsReadHeadSkip(is, n);

//获取头部信息，存入，并跳过相应的长度
#define TarsReadTypeBuf(is, buf, type) \
    TarsPeekTypeBuf(is, buf, 0, type); \
    (is)._cur += sizeof(type);

#define TarsSkipToTag(flag, tag, retHeadType, retHeadTag)                \
    try                                                                  \
    {                                                                    \
        uint8_t nextHeadType, nextHeadTag;                               \
        while (!hasEnd())                                                \
        {                                                                \
            size_t len = 0;                                              \
            TarsPeekFromHead(*this, nextHeadType, nextHeadTag, len);     \
            if (nextHeadType == TarsHeadeStructEnd || tag < nextHeadTag) \
            {                                                            \
                break;                                                   \
            }                                                            \
            if (tag == nextHeadTag)                                      \
            {                                                            \
                (retHeadType) = nextHeadType;                            \
                (retHeadTag) = nextHeadTag;                              \
                TarsReadHeadSkip(*this, len);                            \
                (flag) = true;                                           \
                break;                                                   \
            }                                                            \
            TarsReadHeadSkip(*this, len);                                \
            skipField(nextHeadType);                                     \
        }                                                                \
    }                                                                    \
    catch (TarsDecodeException & e)                                      \
    {                                                                    \
    }

#define TarsReadStringBuf(is, str, len)                                                              \
    if ((is)._cur + (len) > (is)._buf_len)                                                           \
    {                                                                                                \
        char s[64];                                                                                  \
        snprintf(s, sizeof(s), "buffer overflow when peekBuf, over %u.", (uint32_t)((is)._buf_len)); \
        throw TarsDecodeException(s);                                                                \
    }                                                                                                \
    str.assign((is)._buf + (is)._cur, (is)._buf + (is)._cur + (len));                                \
    (is)._cur += len;

//编码相应的宏

//扩充缓冲区
#define TarsReserveBuf(os, len)              \
    if ((os)._buf_len < (len))               \
    {                                        \
        size_t len1 = (len) << 1;            \
        if (len1 < 128)                      \
            len1 = 128;                      \
        (os)._buf = (os)._reserve(os, len1); \
        (os)._buf_len = (len1);              \
    }

//写入一个字节的数据，并修改长度
#define TarsWriteUInt8TTypeBuf(os, val, osLen)     \
    TarsReserveBuf(os, (osLen) + sizeof(uint8_t)); \
    (*(uint8_t *)((os)._buf + (osLen))) = (val);   \
    (osLen) += sizeof(uint8_t);

//写入四个字节的数据，并修改长度
#define TarsWriteUInt32TTypeBuf(os, val, osLen)     \
    TarsReserveBuf(os, (osLen) + sizeof(uint32_t)); \
    (*(uint32_t *)((os)._buf + (osLen))) = (val);   \
    (osLen) += sizeof(uint32_t);

//写入type以及tag信息
#define TarsWriteToHead(os, type, tag)                                \
    if ((tag) < 15)                                                   \
    {                                                                 \
        TarsWriteUInt8TTypeBuf(os, (type) + ((tag) << 4), (os)._len); \
    }                                                                 \
    else                                                              \
    {                                                                 \
        TarsWriteUInt8TTypeBuf(os, (type) + (240), (os)._len);        \
        TarsWriteUInt8TTypeBuf(os, (tag), (os)._len);                 \
    }

#define TarsWriteToBody(os, val, osLen, type)   \
    TarsReserveBuf(os, (osLen) + sizeof(type)); \
    (*(type *)((os)._buf + (osLen))) = (val);   \
    (osLen) += sizeof(type);

#define TarsWriteTypeBuf(os, buf, len)                         \
    TarsReserveBuf(os, (os)._len + (len));                     \
    memcpy((os)._buf + (os)._len, (const void *)(buf), (len)); \
    (os)._len += (len);

namespace tars
{
    /// 缓冲区读取器封装
    class BufferReader
    {
    public:
        const char *_buf; ///< 缓冲区
        size_t _buf_len;  ///< 缓冲区长度
        size_t _cur;      ///< 当前位置

    public:
        BufferReader() : _buf(NULL), _buf_len(0), _cur(0) {}
        ~BufferReader()
        {
            delete[] _buf;
        }

        void reset() { _cur = 0; }

        /// 读取缓存
        void readBuf(void *buf, size_t len)
        {
            if (len <= _buf_len && (_cur + len) <= _buf_len)
            {
                peekBuf(buf, len);
                _cur += len;
            }
            else
            {
                char s[64];
                snprintf(s, sizeof(s), "buffer overflow when skip, over %u.", (uint32_t)_buf_len);
                throw TarsDecodeException(s);
            }
        }

        /// 读取缓存，但不改变偏移量
        void peekBuf(void *buf, size_t len, size_t offset = 0)
        {
            if (_cur + offset + len > _buf_len)
            {
                char s[64];
                snprintf(s, sizeof(s), "buffer overflow when peekBuf, over %u.", (uint32_t)_buf_len);
                throw TarsDecodeException(s);
            }
            memcpy(buf, _buf + _cur + offset, len);
        }

        /// 读取缓存 for vector<char>
        void readBuf(std::vector<char> &v, size_t len)
        {
            if (len <= _buf_len && (_cur + len) <= _buf_len)
            {
                peekBuf(v, len);
                _cur += len;
            }
            else
            {
                char s[64];
                snprintf(s, sizeof(s), "buffer overflow when skip, over %u.", (uint32_t)_buf_len);
                throw TarsDecodeException(s);
            }
        }

        /// 读取缓存，但不改变偏移量 for vector<char>
        void peekBuf(std::vector<Char> &v, size_t len, size_t offset = 0)
        {
            if (_cur + offset + len > _buf_len)
            {
                char s[64];
                snprintf(s, sizeof(s), "buffer overflow when peekBuf, over %u.", (uint32_t)_buf_len);
                throw TarsDecodeException(s);
            }

            const char *begin = _buf + _cur + offset;
            v.assign(begin, begin + len);
        }

        /// 跳过len个字节
        void skip(size_t len)
        {
            if (len <= _buf_len && (_cur + len) <= _buf_len)
            {
                _cur += len;
            }
            else
            {
                char s[64];
                snprintf(s, sizeof(s), "buffer overflow when skip, over %u.", (uint32_t)_buf_len);
                throw TarsDecodeException(s);
            }
        }

        /// 设置缓存
        void setBuffer(const char *buf, size_t len)
        {
            _buf = buf;
            _buf_len = len;
            _cur = 0;
        }

        /// 设置缓存
        void setBuffer(const std::vector<char> &buf)
        {
            _buf = buf.data();
            _buf_len = buf.size();
            _cur = 0;
        }

        /// 判断是否已经到buf的末尾
        bool hasEnd()
        {
            if (_cur > _buf_len)
            {
                char s[64];
                snprintf(s, sizeof(s), "buffer overflow when skip, over %u.", (uint32_t)_buf_len);
                throw TarsDecodeException(s);
            }
            return _cur >= _buf_len;
        }

        size_t len() const
        {
            return _cur;
        }
        const char *base() const
        {
            return _buf;
        }
        size_t size() const
        {
            return _buf_len;
        }
    };

    /// 缓冲区写入器封装
    class BufferWriter
    {
    public:
        char *_buf;
        size_t _len;
        size_t _buf_len;

        static char *_reserve(BufferWriter &os, size_t len)
        {
            char *p = new char[(len)];
            memcpy(p, (os)._buf, (os)._len);
            delete[](os)._buf;
            return p;
        }

    public:
        BufferWriter() : _buf(NULL), _len(0), _buf_len(0) {}
        ~BufferWriter()
        {
            delete[] _buf;
        }

        void reset() { _len = 0; }

        /// 添加缓存内容
        void writeBuf(const void *buf, size_t len)
        {
            TarsReserveBuf(*this, _len + len);
            memcpy(_buf + _len, buf, len);
            _len += len;
        }

        /// 获取缓存内容
        std::vector<char> getByteBuffer() const { return std::vector<char>(_buf, _buf + _len); }

        size_t len() const
        {
            return _len;
        }
        const char *base() const
        {
            return _buf;
        }
        size_t size() const
        {
            return _buf_len;
        }
    };

    class TarsInputStream : public BufferReader
    {
    public:
        /// 跳到指定标签的元素前
        bool skipToTag(uint8_t tag)
        {
            try
            {
                uint8_t headType = 0, headTag = 0;
                while (!hasEnd())
                {
                    size_t len = 0;
                    TarsPeekFromHead(*this, headType, headTag, len);
                    if (tag <= headTag || headType == TarsHeadeStructEnd)
                        return headType == TarsHeadeStructEnd ? false : (tag == headTag);
                    TarsReadHeadSkip(*this, len);
                    skipField(headType);
                }
            }
            catch (TarsDecodeException &e)
            {
            }
            return false;
        }

        /// 跳过一个字段
        void skipField()
        {
            uint8_t headType = 0;
            readFromHeadNoTag(*this, headType);
            skipField(headType);
        }

        /// 跳过一个字段，不包含头信息
        void skipField(uint8_t type)
        {
            switch (type)
            {
            case TarsHeadeChar:
                TarsReadHeadSkip(*this, sizeof(Char));
                break;
            case TarsHeadeInt16:
                TarsReadHeadSkip(*this, sizeof(Int16));
                break;
            case TarsHeadeInt32:
                TarsReadHeadSkip(*this, sizeof(Int32));
                break;
            case TarsHeadeInt64:
                TarsReadHeadSkip(*this, sizeof(Int64));
                break;
            case TarsHeadeFloat:
                TarsReadHeadSkip(*this, sizeof(Float));
                break;
            case TarsHeadeDouble:
                TarsReadHeadSkip(*this, sizeof(Double));
                break;
            case TarsHeadeString1:
            {
                uint8_t len = 0;
                TarsReadTypeBuf(*this, len, uint8_t);
                TarsReadHeadSkip(*this, len);
            }
            break;
            case TarsHeadeString4:
            {
                uint32_t len = 0;
                TarsReadTypeBuf(*this, len, uint32_t);
                TarsReadHeadSkip(*this, len);
            }
            break;
            case TarsHeadeMap:
                break;
            case TarsHeadeList:
                break;
            case TarsHeadeSimpleList:
                break;
            case TarsHeadeStructBegin:
                break;
            case TarsHeadeStructEnd:
                break;
            case TarsHeadeZeroTag:
                break;
            default:
            {
                char s[64];
                snprintf(s, sizeof(s), "skipField with invalid type, type value:%d.", type);
                throw TarsDecodeMismatch(s);
            }
            }
        }

        void read(Char &c, uint8_t tag)
        {
            uint8_t headType = 0, headTag = 0;
            bool skipFlag = false;
            TarsSkipToTag(skipFlag, tag, headType, headTag);
            if (skipFlag)
            {
                switch (headType)
                {
                case TarsHeadeChar:
                    TarsReadTypeBuf(*this, c, Char);
                    break;
                default:
                {
                    char s[64];
                    snprintf(s, sizeof(s), "read 'Char' type mismatch, tag: %d, get type: %d.", tag, headType);
                    throw TarsDecodeMismatch(s);
                }
                }
            }
            else
            {
                char s[64];
                snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d.", tag, headTag);
                throw TarsDecodeRequireNotExist(s);
            }
        }

        void read(Bool &b, uint8_t tag)
        {
            Char c;
            read(c, tag);
            b = c ? true : false;
        }

        /* void read(Int8 &i, uint8_t tag)
        {
            Char c;
            read(c, tag);
            i = (Int8)c;
        } */

        void read(UInt8 &u, uint8_t tag)
        {
            Char c;
            read(c, tag);
            u = (UInt8)c;
        }

        void read(Int16 &i, uint8_t tag)
        {
            uint8_t headType = 0, headTag = 0;
            bool skipFlag = false;
            TarsSkipToTag(skipFlag, tag, headType, headTag);
            if (skipFlag)
            {
                switch (headType)
                {
                case TarsHeadeInt16:
                    TarsReadTypeBuf(*this, i, Int16);
                    break;
                default:
                {
                    char s[64];
                    snprintf(s, sizeof(s), "read 'Char' type mismatch, tag: %d, get type: %d.", tag, headType);
                    throw TarsDecodeMismatch(s);
                }
                }
            }
            else
            {
                char s[64];
                snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d.", tag, headTag);
                throw TarsDecodeRequireNotExist(s);
            }
        }

        void read(UInt16 &u, uint8_t tag)
        {
            Int16 i;
            read(i, tag);
            u = (UInt16)i;
        }

        void read(Int32 &i, uint8_t tag)
        {
            uint8_t headType = 0, headTag = 0;
            bool skipFlag = false;
            TarsSkipToTag(skipFlag, tag, headType, headTag);
            if (skipFlag)
            {
                switch (headType)
                {
                case TarsHeadeInt32:
                    TarsReadTypeBuf(*this, i, Int32);
                    break;
                default:
                {
                    char s[64];
                    snprintf(s, sizeof(s), "read 'Char' type mismatch, tag: %d, get type: %d.", tag, headType);
                    throw TarsDecodeMismatch(s);
                }
                }
            }
            else
            {
                char s[64];
                snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d.", tag, headTag);
                throw TarsDecodeRequireNotExist(s);
            }
        }

        void read(UInt32 &u, uint8_t tag)
        {
            Int32 i;
            read(i, tag);
            u = (UInt32)i;
        }

        void read(Int64 &i, uint8_t tag)
        {
            uint8_t headType = 0, headTag = 0;
            bool skipFlag = false;
            TarsSkipToTag(skipFlag, tag, headType, headTag);
            if (skipFlag)
            {
                switch (headType)
                {
                case TarsHeadeInt64:
                    TarsReadTypeBuf(*this, i, Int64);
                    break;
                default:
                {
                    char s[64];
                    snprintf(s, sizeof(s), "read 'Char' type mismatch, tag: %d, get type: %d.", tag, headType);
                    throw TarsDecodeMismatch(s);
                }
                }
            }
            else
            {
                char s[64];
                snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d.", tag, headTag);
                throw TarsDecodeRequireNotExist(s);
            }
        }

        void read(UInt64 &u, uint8_t tag)
        {
            Int64 i;
            read(i, tag);
            u = (UInt64)i;
        }

        void read(String &s, uint8_t tag)
        {
            uint8_t headType = 0, headTag = 0;
            bool skipFlag = false;
            TarsSkipToTag(skipFlag, tag, headType, headTag);
            if (skipFlag)
            {
                uint32_t strLength = 0;
                switch (headType)
                {
                case TarsHeadeString1:
                {
                    TarsReadTypeBuf(*this, strLength, uint8_t);
                }
                break;
                case TarsHeadeString4:
                {
                    TarsReadTypeBuf(*this, strLength, uint32_t);
                }
                break;
                default:
                {
                    char s[64];
                    snprintf(s, sizeof(s), "read 'string' type mismatch, tag: %d, get type: %d, tag: %d.", tag, headType, headTag);
                    throw TarsDecodeMismatch(s);
                }
                }
                TarsReadStringBuf(*this, s, strLength);
            }
            else
            {
                char s[64];
                snprintf(s, sizeof(s), "require field not exist, tag: %d", tag);
                throw TarsDecodeRequireNotExist(s);
            }
        }

        void read(Float &f, uint8_t tag)
        {
            uint8_t headType = 0, headTag = 0;
            bool skipFlag = false;
            TarsSkipToTag(skipFlag, tag, headType, headTag);
            if (skipFlag)
            {
                switch (headType)
                {
                case TarsHeadeFloat:
                    TarsReadTypeBuf(*this, f, Float);
                    break;
                default:
                {
                    char s[64];
                    snprintf(s, sizeof(s), "read 'Char' type mismatch, tag: %d, get type: %d.", tag, headType);
                    throw TarsDecodeMismatch(s);
                }
                }
            }
            else
            {
                char s[64];
                snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d.", tag, headTag);
                throw TarsDecodeRequireNotExist(s);
            }
        }

        void read(Double &d, uint8_t tag)
        {
            uint8_t headType = 0, headTag = 0;
            bool skipFlag = false;
            TarsSkipToTag(skipFlag, tag, headType, headTag);
            if (skipFlag)
            {
                switch (headType)
                {
                case TarsHeadeDouble:
                    TarsReadTypeBuf(*this, d, Double);
                    break;
                default:
                {
                    char s[64];
                    snprintf(s, sizeof(s), "read 'Char' type mismatch, tag: %d, get type: %d.", tag, headType);
                    throw TarsDecodeMismatch(s);
                }
                }
            }
            else
            {
                char s[64];
                snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d.", tag, headTag);
                throw TarsDecodeRequireNotExist(s);
            }
        }
    };

    class TarsOutputStream : public BufferWriter
    {
    public:
        void write(Char c, uint8_t tag)
        {
            TarsWriteToHead(*this, TarsHeadeChar, tag);
            TarsWriteToBody(*this, c, (*this)._len, Char);
        }

        void write(Bool b, uint8_t tag)
        {
            TarsWriteToHead(*this, TarsHeadeChar, tag);
            TarsWriteToBody(*this, b, (*this)._len, Bool);
        }

        /* void write(Int8 i, uint8_t tag)
        {
            TarsWriteToHead(*this, TarsHeadeChar, tag);
            writeBuf((char *)i, sizeof(Char));
        } */

        void write(UInt8 u, uint8_t tag)
        {
            TarsWriteToHead(*this, TarsHeadeChar, tag);
            TarsWriteToBody(*this, u, (*this)._len, UInt8);
        }

        void write(Int16 i, uint8_t tag)
        {
            TarsWriteToHead(*this, TarsHeadeInt16, tag);
            TarsWriteToBody(*this, i, (*this)._len, Int16);
        }

        void write(UInt16 u, uint8_t tag)
        {
            TarsWriteToHead(*this, TarsHeadeInt16, tag);
            TarsWriteToBody(*this, u, (*this)._len, UInt16);
        }

        void write(Int32 i, uint8_t tag)
        {
            TarsWriteToHead(*this, TarsHeadeInt32, tag);
            TarsWriteToBody(*this, i, (*this)._len, Int32);
        }

        void write(UInt32 u, uint8_t tag)
        {
            TarsWriteToHead(*this, TarsHeadeInt32, tag);
            TarsWriteToBody(*this, u, (*this)._len, UInt32);
        }

        void write(Int64 i, uint8_t tag)
        {
            TarsWriteToHead(*this, TarsHeadeInt64, tag);
            TarsWriteToBody(*this, i, (*this)._len, Int64);
        }

        void write(UInt64 u, uint8_t tag)
        {
            TarsWriteToHead(*this, TarsHeadeInt64, tag);
            TarsWriteToBody(*this, u, (*this)._len, UInt64);
        }

        void write(Float f, uint8_t tag)
        {
            TarsWriteToHead(*this, TarsHeadeFloat, tag);
            TarsWriteToBody(*this, f, (*this)._len, Float);
        }

        void write(Double d, uint8_t tag)
        {
            TarsWriteToHead(*this, TarsHeadeDouble, tag);
            TarsWriteToBody(*this, d, (*this)._len, Double);
        }

        void write(String s, uint8_t tag)
        {
            if (s.size() <= 255)
            {
                TarsWriteToHead(*this, TarsHeadeString1, tag);
                uint8_t n = (uint8_t)s.size();
                TarsWriteUInt8TTypeBuf(*this, n, (*this)._len);

                TarsWriteTypeBuf(*this, s.data(), s.size());
            }
            else
            {
                TarsWriteToHead(*this, TarsHeadeString4, tag);
                uint32_t n = (uint32_t)s.size();
                TarsWriteUInt32TTypeBuf(*this, n, (*this)._len);

                TarsWriteTypeBuf(*this, s.data(), s.size());
            }
        }
    };
}

#endif