#ifndef __DEF_H__
#define __DEF_H__

typedef char sbyte;
typedef unsigned char ubyte;
typedef short word;
typedef unsigned short uword;
typedef int dword;
typedef unsigned int udword;
typedef long long qword;
typedef unsigned long long uqword;
typedef float float32;
typedef double float64;
//typedef bool boolean;
typedef char chart;

//typedef bool Boolean;

typedef ubyte* pubyte;
//typedef char*  pChar;
typedef void* PVoid;

typedef uword loopHandleType;

#define MyFalse false
#define MyTrue true
#define Null	NULL
#define MaxUQword 0xffffffffffffffff

#define c_null_ubyte 0xff
#define c_null_uword 0xffff
#define c_null_UDW   0xffffffff

#define ipMaxLen 16
#define comNameSize 32 

#endif/*__DEF_H__*/
