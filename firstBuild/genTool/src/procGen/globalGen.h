#ifndef _globalGen_h__
#define _globalGen_h__
#include <memory>

class globalGen
{
public:
    globalGen ();
    ~globalGen ();
	int  startGen ();
	int  secondGen ();
	int  configLibGen ();
private:
	int  makePath ();
};
#endif
