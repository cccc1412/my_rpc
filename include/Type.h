#ifndef TYPE_H_
#define TYPE_H_

#include <iostream>

//实现
#define HeadeChar 0
#define HeadeInt16 1
#define HeadeInt32 2
#define HeadeInt64 3
#define HeadeFloat 4
#define HeadeDouble 5
#define HeadeString1 6
#define HeadeString4 7
//不实现
#define HeadeMap 8
#define HeadeList 9
#define HeadeStructBegin 10
#define HeadeStructEnd 11
#define HeadeZeroTag 12
#define HeadeSimpleList 13

#define MAX_TRANS_LEN 1024

typedef bool Bool;
typedef char Char;
typedef float Float;
typedef double Double;

typedef char Int8;
typedef short Int16;
typedef int Int32;
typedef long Int64;

typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef unsigned int UInt32;
typedef unsigned long UInt64;

typedef std::string String;

#endif
