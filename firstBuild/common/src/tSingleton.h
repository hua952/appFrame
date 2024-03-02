#ifndef _tSingleton_h_
#define _tSingleton_h_
#include <memory>
#include "myAssert.h"

template<class T>
class tSingleton
{
public:
	static void createSingleton()
	{
		myAssert(nullptr == s_pMgr);
		s_pMgr = std::make_unique<T>();
	}
	
	static T& single()
	{
		return *s_pMgr;
	}
	static T* psingle()
	{
		return s_pMgr;
	}
protected:
	static std::unique_ptr<T> s_pMgr;
};

template<class T>
std::unique_ptr<T> tSingleton<T>::s_pMgr;
#endif
