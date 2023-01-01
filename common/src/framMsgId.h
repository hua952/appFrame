#ifndef framMsgId_h__
#define framMsgId_h__
enum enFramMsgId
{
	enFramMsgId_regMyHandleAsk,
	enFramMsgId_regMyHandleRet
};

#define framMsgIdBegin 60000
#define toFramMsgId(x) (framMsgIdBegin+x)

struct regMyHandleAskPack
{
	ubyte myHandle;
};
struct regMyHandleRetPack
{
	udword result;
	ubyte myHandle;
};
#endif
