#ifndef _globalGen_h__
#define _globalGen_h__
#include <memory>

class globalGen
{
public:
    globalGen ();
    ~globalGen ();
	int  startGen ();
private:
	int  makePath ();
};
#endif
