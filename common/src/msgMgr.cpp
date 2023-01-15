#include "msgMgr.h"
#include "myAssert.h"
/*
bool msgMgr::msgInfo::operator () (const pmsgInfo& pa, const pmsgInfo& pb) const
{
	return true;
}

bool  msgMgr::msgInfoCmp::operator () (const msgInfo& ra, const msgInfo& rb) const
{
	return ra.msgId < rb.msgId;
}
*/
msgMgr::msgMgr ()
{
}

msgMgr::~msgMgr ()
{
}

int  msgMgr::  regRpc (msgIdType askId, msgIdType retId, loopHandleType askDefProcSer, loopHandleType retDefProcSer)
{
	int nRet = 0;
	auto& rMsgInfoS = m_msgInfoS;
	auto& rAskMap = m_askMap;
	auto& rRetMap = m_retMap;
	msgInfo  askInfo;
	askInfo.defProcServer = askDefProcSer;
	msgInfo  retInfo;
	retInfo.defProcServer = retDefProcSer;
	bool  bR = 	rMsgInfoS.insert (askId, askInfo);
	myAssert (bR);
	bR = rMsgInfoS.insert (retId, retInfo);
	myAssert (bR);
	bR = rAskMap.insert (askId, retId);
	myAssert (bR);
	bR = rRetMap.insert (retId, askId);
	myAssert (bR);
	return nRet;
}

int   msgMgr:: regAskOnly (msgIdType askId, loopHandleType askDefProcSer)
{
	int nRet = 0;
	auto& rMsgInfoS = m_msgInfoS;
	auto& rAskMap = m_askMap;
	msgInfo  askInfo;
	askInfo.defProcServer = askDefProcSer;
	bool  bR = 	rMsgInfoS.insert (askId, askInfo);
	myAssert (bR);
	bR = rAskMap.insert (askId, c_null_msgID);
	myAssert (bR);
	return nRet;
}
