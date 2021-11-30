#ifndef SERIALIZATION_H_
#define SERIALIZATION_H_

#include <stdint.h>
#include <string.h>

#include <vector>

#include "Type.h"

struct ProtoException : public std::runtime_error {
  ProtoException(const std::string &s) : std::runtime_error(s) {}
};

struct EncodeException : public ProtoException {
  EncodeException(const std::string &s) : ProtoException(s) {}
};

struct DecodeException : public ProtoException {
  DecodeException(const std::string &s) : ProtoException(s) {}
};

struct DecodeMismatch : public DecodeException {
  DecodeMismatch(const std::string &s) : DecodeException(s) {}
};

struct DecodeRequireNotExist : public DecodeException {
  DecodeRequireNotExist(const std::string &s) : DecodeException(s) {}
};

struct DecodeInvalidValue : public DecodeException {
  DecodeInvalidValue(const std::string &s) : DecodeException(s) {}
};

struct NotEnoughBuff : public ProtoException {
  NotEnoughBuff(const std::string &s) : ProtoException(s) {}
};

//解码相应的宏

//获取第一个字节以类型type放入buf
#define PeekTypeBuf(is, buf, offset, type)                           \
  if ((is)._cur + (offset) + sizeof(type) > (is)._buf_len) {         \
    char s[64];                                                      \
    snprintf(s, sizeof(s), "buffer overflow when peekBuf, over %u.", \
             (uint32_t)((is)._buf_len));                             \
    throw DecodeException(s);                                        \
  }                                                                  \
  (buf) = (*((type *)((is)._buf + (is)._cur + (offset))));

//获取tag以及type分别存入
#define PeekFromHead(is, type, tag, n)  \
  (n) = 1;                              \
  uint8_t typeTag, tmpTag;              \
  PeekTypeBuf(is, typeTag, 0, uint8_t); \
  tmpTag = typeTag >> 4;                \
  (type) = (typeTag & 0x0F);            \
  if (tmpTag == 15) {                   \
    PeekTypeBuf(is, tag, 1, uint8_t);   \
    (n) += 1;                           \
  } else {                              \
    (tag) = tmpTag;                     \
  }

//跳过长度为len的head
#define ReadHeadSkip(is, len) (is)._cur += (len);

//获取type存入
#define PeekFromHeadNoTag(is, type, n)    \
  (n) = 1;                                \
  uint8_t typeTag, tmpTag;                \
  PeekTypeBuf(is, typeTag, 0, uint8_t);   \
  tmpTag = typeTag >> 4;                  \
  (type) = (typeTag & 0x0F);              \
  if (tmpTag == 15) {                     \
    PeekTypeBuf(is, typeTag, 1, uint8_t); \
    (n) += 1;                             \
  }

//获取type存入,跳过长度为len的head
#define readFromHeadNoTag(is, type) \
  size_t n = 0;                     \
  PeekFromHeadNoTag(is, type, n);   \
  ReadHeadSkip(is, n);

//获取头部信息，存入，并跳过相应的长度
#define ReadTypeBuf(is, buf, type) \
  PeekTypeBuf(is, buf, 0, type);   \
  (is)._cur += sizeof(type);

#define SkipToTag(flag, tag, retHeadType, retHeadTag)            \
  try {                                                          \
    uint8_t nextHeadType, nextHeadTag;                           \
    while (!hasEnd()) {                                          \
      size_t len = 0;                                            \
      PeekFromHead(*this, nextHeadType, nextHeadTag, len);       \
      if (nextHeadType == HeadeStructEnd || tag < nextHeadTag) { \
        break;                                                   \
      }                                                          \
      if (tag == nextHeadTag) {                                  \
        (retHeadType) = nextHeadType;                            \
        (retHeadTag) = nextHeadTag;                              \
        ReadHeadSkip(*this, len);                                \
        (flag) = true;                                           \
        break;                                                   \
      }                                                          \
      ReadHeadSkip(*this, len);                                  \
      skipField(nextHeadType);                                   \
    }                                                            \
  } catch (DecodeException & e) {                                \
  }

#define ReadStringBuf(is, str, len)                                  \
  if ((is)._cur + (len) > (is)._buf_len) {                           \
    char s[64];                                                      \
    snprintf(s, sizeof(s), "buffer overflow when peekBuf, over %u.", \
             (uint32_t)((is)._buf_len));                             \
    throw DecodeException(s);                                        \
  }                                                                  \
  str.assign((is)._buf + (is)._cur, (is)._buf + (is)._cur + (len));  \
  (is)._cur += len;

//编码相应的宏

//扩充缓冲区
#define ReserveBuf(os, len)              \
  if ((os)._buf_len < (len)) {           \
    size_t len1 = (len) << 1;            \
    if (len1 < 128) len1 = 128;          \
    (os)._buf = (os)._reserve(os, len1); \
    (os)._buf_len = (len1);              \
  }

//写入一个字节的数据，并修改长度
#define WriteUInt8TTypeBuf(os, val, osLen)     \
  ReserveBuf(os, (osLen) + sizeof(uint8_t));   \
  (*(uint8_t *)((os)._buf + (osLen))) = (val); \
  (osLen) += sizeof(uint8_t);

//写入四个字节的数据，并修改长度
#define WriteUInt32TTypeBuf(os, val, osLen)     \
  ReserveBuf(os, (osLen) + sizeof(uint32_t));   \
  (*(uint32_t *)((os)._buf + (osLen))) = (val); \
  (osLen) += sizeof(uint32_t);

//写入type以及tag信息
#define WriteToHead(os, type, tag)                            \
  if ((tag) < 15) {                                           \
    WriteUInt8TTypeBuf(os, (type) + ((tag) << 4), (os)._len); \
  } else {                                                    \
    WriteUInt8TTypeBuf(os, (type) + (240), (os)._len);        \
    WriteUInt8TTypeBuf(os, (tag), (os)._len);                 \
  }

#define WriteToBody(os, val, osLen, type)   \
  ReserveBuf(os, (osLen) + sizeof(type));   \
  (*(type *)((os)._buf + (osLen))) = (val); \
  (osLen) += sizeof(type);

#define WriteTypeBuf(os, buf, len)                           \
  ReserveBuf(os, (os)._len + (len));                         \
  memcpy((os)._buf + (os)._len, (const void *)(buf), (len)); \
  (os)._len += (len);

/// 缓冲区读取器封装
class BufferReader {
 public:
  const char *_buf;  ///< 缓冲区
  size_t _buf_len;   ///< 缓冲区长度
  size_t _cur;       ///< 当前位置

 public:
  BufferReader() : _buf(NULL), _buf_len(0), _cur(0) {}
  ~BufferReader() { delete[] _buf; }

  void reset() { _cur = 0; }

  /// 读取缓存
  void readBuf(void *buf, size_t len) {
    if (len <= _buf_len && (_cur + len) <= _buf_len) {
      peekBuf(buf, len);
      _cur += len;
    } else {
      char s[64];
      snprintf(s, sizeof(s), "buffer overflow when skip, over %u.",
               (uint32_t)_buf_len);
      throw DecodeException(s);
    }
  }

  /// 读取缓存，但不改变偏移量
  void peekBuf(void *buf, size_t len, size_t offset = 0) {
    if (_cur + offset + len > _buf_len) {
      char s[64];
      snprintf(s, sizeof(s), "buffer overflow when peekBuf, over %u.",
               (uint32_t)_buf_len);
      throw DecodeException(s);
    }
    memcpy(buf, _buf + _cur + offset, len);
  }

  /// 读取缓存 for vector<char>
  void readBuf(std::vector<char> &v, size_t len) {
    if (len <= _buf_len && (_cur + len) <= _buf_len) {
      peekBuf(v, len);
      _cur += len;
    } else {
      char s[64];
      snprintf(s, sizeof(s), "buffer overflow when skip, over %u.",
               (uint32_t)_buf_len);
      throw DecodeException(s);
    }
  }

  /// 读取缓存，但不改变偏移量 for vector<char>
  void peekBuf(std::vector<Char> &v, size_t len, size_t offset = 0) {
    if (_cur + offset + len > _buf_len) {
      char s[64];
      snprintf(s, sizeof(s), "buffer overflow when peekBuf, over %u.",
               (uint32_t)_buf_len);
      throw DecodeException(s);
    }

    const char *begin = _buf + _cur + offset;
    v.assign(begin, begin + len);
  }

  /// 跳过len个字节
  void skip(size_t len) {
    if (len <= _buf_len && (_cur + len) <= _buf_len) {
      _cur += len;
    } else {
      char s[64];
      snprintf(s, sizeof(s), "buffer overflow when skip, over %u.",
               (uint32_t)_buf_len);
      throw DecodeException(s);
    }
  }

  /// 设置缓存
  void setBuffer(const char *buf, size_t len) {
    _buf = buf;
    _buf_len = len;
    _cur = 0;
  }

  /// 设置缓存
  void setBuffer(const std::vector<char> &buf) {
    _buf = buf.data();
    _buf_len = buf.size();
    _cur = 0;
  }

  /// 判断是否已经到buf的末尾
  bool hasEnd() {
    if (_cur > _buf_len) {
      char s[64];
      snprintf(s, sizeof(s), "buffer overflow when skip, over %u.",
               (uint32_t)_buf_len);
      throw DecodeException(s);
    }
    return _cur >= _buf_len;
  }

  size_t len() const { return _cur; }
  const char *base() const { return _buf; }
  size_t size() const { return _buf_len; }
};

/// 缓冲区写入器封装
class BufferWriter {
 public:
  char *_buf;
  size_t _len;
  size_t _buf_len;

  static char *_reserve(BufferWriter &os, size_t len) {
    char *p = new char[(len)];
    memcpy(p, (os)._buf, (os)._len);
    delete[](os)._buf;
    return p;
  }

 public:
  BufferWriter() : _buf(NULL), _len(0), _buf_len(0) {}
  ~BufferWriter() { delete[] _buf; }

  void reset() { _len = 0; }

  /// 添加缓存内容
  void writeBuf(const void *buf, size_t len) {
    ReserveBuf(*this, _len + len);
    memcpy(_buf + _len, buf, len);
    _len += len;
  }

  /// 获取缓存内容
  std::vector<char> getByteBuffer() const {
    return std::vector<char>(_buf, _buf + _len);
  }

  size_t len() const { return _len; }
  const char *base() const { return _buf; }
  size_t size() const { return _buf_len; }
};

class InputStream : public BufferReader {
 public:
  /// 跳到指定标签的元素前
  bool skipToTag(uint8_t tag) {
    try {
      uint8_t headType = 0, headTag = 0;
      while (!hasEnd()) {
        size_t len = 0;
        PeekFromHead(*this, headType, headTag, len);
        if (tag <= headTag || headType == HeadeStructEnd)
          return headType == HeadeStructEnd ? false : (tag == headTag);
        ReadHeadSkip(*this, len);
        skipField(headType);
      }
    } catch (DecodeException &e) {
    }
    return false;
  }

  /// 跳过一个字段
  void skipField() {
    uint8_t headType = 0;
    readFromHeadNoTag(*this, headType);
    skipField(headType);
  }

  /// 跳过一个字段，不包含头信息
  void skipField(uint8_t type) {
    switch (type) {
      case HeadeChar:
        ReadHeadSkip(*this, sizeof(Char));
        break;
      case HeadeInt16:
        ReadHeadSkip(*this, sizeof(Int16));
        break;
      case HeadeInt32:
        ReadHeadSkip(*this, sizeof(Int32));
        break;
      case HeadeInt64:
        ReadHeadSkip(*this, sizeof(Int64));
        break;
      case HeadeFloat:
        ReadHeadSkip(*this, sizeof(Float));
        break;
      case HeadeDouble:
        ReadHeadSkip(*this, sizeof(Double));
        break;
      case HeadeString1: {
        uint8_t len = 0;
        ReadTypeBuf(*this, len, uint8_t);
        ReadHeadSkip(*this, len);
      } break;
      case HeadeString4: {
        uint32_t len = 0;
        ReadTypeBuf(*this, len, uint32_t);
        ReadHeadSkip(*this, len);
      } break;
      case HeadeMap:
        break;
      case HeadeList:
        break;
      case HeadeSimpleList:
        break;
      case HeadeStructBegin:
        break;
      case HeadeStructEnd:
        break;
      case HeadeZeroTag:
        break;
      default: {
        char s[64];
        snprintf(s, sizeof(s), "skipField with invalid type, type value:%d.",
                 type);
        throw DecodeMismatch(s);
      }
    }
    // std::cout<<_cur<<std::endl;
  }

  void read(Char &c, uint8_t tag) {
    uint8_t headType = 0, headTag = 0;
    bool skipFlag = false;
    SkipToTag(skipFlag, tag, headType, headTag);
    if (skipFlag) {
      switch (headType) {
        case HeadeChar:
          ReadTypeBuf(*this, c, Char);
          break;
        default: {
          char s[64];
          snprintf(s, sizeof(s),
                   "read 'Char' type mismatch, tag: %d, get type: %d.", tag,
                   headType);
          throw DecodeMismatch(s);
        }
      }
    } else {
      char s[64];
      snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d.",
               tag, headTag);
      throw DecodeRequireNotExist(s);
    }
  }

  void read(Bool &b, uint8_t tag) {
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

  void read(UInt8 &u, uint8_t tag) {
    Char c;
    read(c, tag);
    u = (UInt8)c;
  }

  void read(Int16 &i, uint8_t tag) {
    uint8_t headType = 0, headTag = 0;
    bool skipFlag = false;
    SkipToTag(skipFlag, tag, headType, headTag);
    if (skipFlag) {
      switch (headType) {
        case HeadeInt16:
          ReadTypeBuf(*this, i, Int16);
          break;
        default: {
          char s[64];
          snprintf(s, sizeof(s),
                   "read 'Char' type mismatch, tag: %d, get type: %d.", tag,
                   headType);
          throw DecodeMismatch(s);
        }
      }
    } else {
      char s[64];
      snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d.",
               tag, headTag);
      throw DecodeRequireNotExist(s);
    }
  }

  void read(UInt16 &u, uint8_t tag) {
    Int16 i;
    read(i, tag);
    u = (UInt16)i;
  }

  void read(Int32 &i, uint8_t tag) {
    uint8_t headType = 0, headTag = 0;
    bool skipFlag = false;
    SkipToTag(skipFlag, tag, headType, headTag);
    if (skipFlag) {
      switch (headType) {
        case HeadeInt32:
          ReadTypeBuf(*this, i, Int32);
          break;
        default: {
          char s[64];
          snprintf(s, sizeof(s),
                   "read 'Char' type mismatch, tag: %d, get type: %d.", tag,
                   headType);
          throw DecodeMismatch(s);
        }
      }
    } else {
      char s[64];
      snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d.",
               tag, headTag);
      throw DecodeRequireNotExist(s);
    }
  }

  void read(UInt32 &u, uint8_t tag) {
    Int32 i;
    read(i, tag);
    u = (UInt32)i;
  }

  void read(Int64 &i, uint8_t tag) {
    uint8_t headType = 0, headTag = 0;
    bool skipFlag = false;
    SkipToTag(skipFlag, tag, headType, headTag);
    if (skipFlag) {
      switch (headType) {
        case HeadeInt64:
          ReadTypeBuf(*this, i, Int64);
          break;
        default: {
          char s[64];
          snprintf(s, sizeof(s),
                   "read 'Char' type mismatch, tag: %d, get type: %d.", tag,
                   headType);
          throw DecodeMismatch(s);
        }
      }
    } else {
      char s[64];
      snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d.",
               tag, headTag);
      throw DecodeRequireNotExist(s);
    }
  }

  void read(UInt64 &u, uint8_t tag) {
    Int64 i;
    read(i, tag);
    u = (UInt64)i;
  }

  void read(String &s, uint8_t tag) {
    uint8_t headType = 0, headTag = 0;
    bool skipFlag = false;
    SkipToTag(skipFlag, tag, headType, headTag);
    if (skipFlag) {
      uint32_t strLength = 0;
      switch (headType) {
        case HeadeString1: {
          ReadTypeBuf(*this, strLength, uint8_t);
        } break;
        case HeadeString4: {
          ReadTypeBuf(*this, strLength, uint32_t);
        } break;
        default: {
          char s[64];
          snprintf(
              s, sizeof(s),
              "read 'string' type mismatch, tag: %d, get type: %d, tag: %d.",
              tag, headType, headTag);
          throw DecodeMismatch(s);
        }
      }
      ReadStringBuf(*this, s, strLength);
    } else {
      char s[64];
      snprintf(s, sizeof(s), "require field not exist, tag: %d", tag);
      throw DecodeRequireNotExist(s);
    }
  }

  void read(Float &f, uint8_t tag) {
    uint8_t headType = 0, headTag = 0;
    bool skipFlag = false;
    SkipToTag(skipFlag, tag, headType, headTag);
    if (skipFlag) {
      switch (headType) {
        case HeadeFloat:
          ReadTypeBuf(*this, f, Float);
          break;
        default: {
          char s[64];
          snprintf(s, sizeof(s),
                   "read 'Char' type mismatch, tag: %d, get type: %d.", tag,
                   headType);
          throw DecodeMismatch(s);
        }
      }
    } else {
      char s[64];
      snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d.",
               tag, headTag);
      throw DecodeRequireNotExist(s);
    }
  }

  void read(Double &d, uint8_t tag) {
    uint8_t headType = 0, headTag = 0;
    bool skipFlag = false;
    SkipToTag(skipFlag, tag, headType, headTag);
    if (skipFlag) {
      switch (headType) {
        case HeadeDouble:
          ReadTypeBuf(*this, d, Double);
          break;
        default: {
          char s[64];
          snprintf(s, sizeof(s),
                   "read 'Char' type mismatch, tag: %d, get type: %d.", tag,
                   headType);
          throw DecodeMismatch(s);
        }
      }
    } else {
      char s[64];
      snprintf(s, sizeof(s), "require field not exist, tag: %d, headTag: %d.",
               tag, headTag);
      throw DecodeRequireNotExist(s);
    }
  }
};

class OutputStream : public BufferWriter {
 public:
  void write(Char c, uint8_t tag) {
    WriteToHead(*this, HeadeChar, tag);
    WriteToBody(*this, c, (*this)._len, Char);
  }

  void write(Bool b, uint8_t tag) {
    WriteToHead(*this, HeadeChar, tag);
    WriteToBody(*this, b, (*this)._len, Bool);
  }

  /* void write(Int8 i, uint8_t tag)
      {
          WriteToHead(*this, HeadeChar, tag);
          writeBuf((char *)i, sizeof(Char));
      } */

  void write(UInt8 u, uint8_t tag) {
    WriteToHead(*this, HeadeChar, tag);
    WriteToBody(*this, u, (*this)._len, UInt8);
  }

  void write(Int16 i, uint8_t tag) {
    WriteToHead(*this, HeadeInt16, tag);
    WriteToBody(*this, i, (*this)._len, Int16);
  }

  void write(UInt16 u, uint8_t tag) {
    WriteToHead(*this, HeadeInt16, tag);
    WriteToBody(*this, u, (*this)._len, UInt16);
  }

  void write(Int32 i, uint8_t tag) {
    WriteToHead(*this, HeadeInt32, tag);
    WriteToBody(*this, i, (*this)._len, Int32);
  }

  void write(UInt32 u, uint8_t tag) {
    WriteToHead(*this, HeadeInt32, tag);
    WriteToBody(*this, u, (*this)._len, UInt32);
  }

  void write(Int64 i, uint8_t tag) {
    WriteToHead(*this, HeadeInt64, tag);
    WriteToBody(*this, i, (*this)._len, Int64);
  }

  void write(UInt64 u, uint8_t tag) {
    WriteToHead(*this, HeadeInt64, tag);
    WriteToBody(*this, u, (*this)._len, UInt64);
  }

  void write(Float f, uint8_t tag) {
    WriteToHead(*this, HeadeFloat, tag);
    WriteToBody(*this, f, (*this)._len, Float);
  }

  void write(Double d, uint8_t tag) {
    WriteToHead(*this, HeadeDouble, tag);
    WriteToBody(*this, d, (*this)._len, Double);
  }

  void write(String s, uint8_t tag) {
    if (s.size() <= 255) {
      WriteToHead(*this, HeadeString1, tag);
      uint8_t n = (uint8_t)s.size();
      WriteUInt8TTypeBuf(*this, n, (*this)._len);

      WriteTypeBuf(*this, s.data(), s.size());
    } else {
      WriteToHead(*this, HeadeString4, tag);
      uint32_t n = (uint32_t)s.size();
      WriteUInt32TTypeBuf(*this, n, (*this)._len);

      WriteTypeBuf(*this, s.data(), s.size());
    }
  }
};

#endif