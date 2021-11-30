/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016THL A29 Limited, a Tencent company. All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use this file except 
 * in compliance with the License. You may obtain a copy of the License at
 *
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software distributed 
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR 
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the 
 * specific language governing permissions and limitations under the License.
 */

#ifndef __TARS_TYPE_H__
#define __TARS_TYPE_H__


#include <iostream>

//实现
#define TarsHeadeChar 0
#define TarsHeadeInt16 1
#define TarsHeadeInt32 2
#define TarsHeadeInt64 3
#define TarsHeadeFloat 4
#define TarsHeadeDouble 5
#define TarsHeadeString1 6
#define TarsHeadeString4 7

//不实现
#define TarsHeadeMap 8
#define TarsHeadeList 9
#define TarsHeadeStructBegin 10
#define TarsHeadeStructEnd 11
#define TarsHeadeZeroTag 12
#define TarsHeadeSimpleList 13

#define MAX_TRANS_LEN 1024

namespace tars
{
/////////////////////////////////////////////////////////////////////////////////////
typedef bool    Bool;
typedef char    Char;
typedef float   Float;
typedef double  Double;

typedef char    Int8;
typedef short   Int16;
typedef int     Int32;
typedef long    Int64;

typedef unsigned char   UInt8;
typedef unsigned short  UInt16;
typedef unsigned int    UInt32;
typedef unsigned long   UInt64;

typedef std::string String;

}

#endif
