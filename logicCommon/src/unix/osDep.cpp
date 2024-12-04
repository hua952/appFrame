#include <unistd.h>
#include <dlfcn.h>
#include <limits.h>
#include "comFun.h"
#include "strFun.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <random>
#include <iomanip>
#include "myAssert.h"
#include <iostream>
#include<filesystem>
#include <uuid/uuid.h>

int   createUuid (char* outPut)
{
/*
	std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 255);

	auto pBuf = (ubyte*)outPut;
    for (int i = 0; i < 16; ++i) {
		pBuf[i] = distrib(gen);
    }

*/
	auto p= (uuid_t*)outPut;
	uuid_generate(*p);
	return 0;
}
