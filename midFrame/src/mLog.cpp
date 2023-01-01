#include "mLog.h"
#include "impMainLoop.h"
#include "tSingleton.h"

logMsgFT getLogMsgFun ()
{
	return tSingleton<PhyInfo>::single ().getPhyCallback().fnLogMsg;
}
