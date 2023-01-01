#ifndef _com_fum_h__
#define _com_fum_h__
#include<string.h>
#include"typeDef.h"

template<class T>
class LessComp
{
public:
    bool operator ()(const T& a, const T& b)const
    {
        return a < b;
    }
};

typedef const char* cpchar;
class cpcharCmp
{
public:
	bool operator ()(const cpchar& a, const cpchar& b)
	{
		return strcmp(a,b) < 0;
	}
};

typedef char* pchar;
class pcharCmp
{
public:
	bool operator ()(const pchar& a, const pchar& b)
	{
		return strcmp(a,b) < 0;
	}
};

struct iStringKey
{
	virtual const char* name() const= 0;
};

typedef iStringKey* pIStringKey;
class pIStringKeyCmp
{
public:
	bool operator ()(const pIStringKey& a, const pIStringKey& b)const
	{
		return strcmp(a->name(),b->name()) < 0;
	}
};

template<class T>
class HashCode
{
public:
    uqword operator ()(const T& a)const
    {
        return *((udword*)&a) % 99991;
    }
};

template<class Key, class Value>
struct KV 
{
    Key k;
    Value v;
};

template<class Key, class Value>
struct keyValue
{
    Key key;
    Value value;
};
struct myEndPoint
{
	static const int c_ipSize = 16;
	char ip[c_ipSize];
	uword port;
};

typedef  bool (*ComTimerFun)(void*);

loopHandleType  toHandle (loopHandleType  p, loopHandleType l);
bool            fromHandle (loopHandleType handle, loopHandleType &p, loopHandleType& l);
#define SD(p) {delete p;p=nullptr;}
#define SDA(p) {delete[]p;p=nullptr;}

#endif
