#include "moduleWinDefGen.h"
#include "strFun.h"
#include "fromFileData/moduleFile.h"
#include "moduleGen.h"
#include "rLog.h"
#include <string>
#include <fstream>

moduleWinDefGen:: moduleWinDefGen ()
{
}

moduleWinDefGen:: ~moduleWinDefGen ()
{
}

int   moduleWinDefGen:: startGen (moduleGen&  rMod)
{
    int   nRet = 0;
    do {
		
		std::string win = rMod.genPath ();
		win += "/win";
		auto defFun = win;
		defFun += "/defFun.def";
		std::ofstream os(defFun.c_str ());
		if (!os) {
			nRet = 1;
			rError (" open file for write error fileName = "<<defFun.c_str ());
			break;
		}
		const char* szCon = R"(LIBRARY
EXPORTS
	afterLoad	@1
	logicOnAccept	@2
	logicOnConnect  @3
	onLoopBegin		@4
	onLoopEnd		@5
	beforeUnload	@6
	onFrameLogic	@7
	)";
		os<<szCon;
    } while (0);
    return nRet;
}

