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

int  msgMgr::  regRpc (msgIdType askId, msgIdType retId, serverIdType	askDefProcSer,
		serverIdType	 retDefProcSer)
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

msgIdType  msgMgr::  getRetMsg(msgIdType    askMsg)
{
	msgIdType nRet = c_null_msgID;
	auto& rMgr = m_askMap;
	auto it = rMgr.find (askMsg);
	if (it) {
		nRet = *it;
	}
	return nRet;
}

msgIdType  msgMgr::getAskMsg(msgIdType    retMsg)
{
	msgIdType nRet = c_null_msgID;
	auto& rMgr = m_retMap;
	auto it = rMgr.find (retMsg);
	if (it) {
		nRet = *it;
	}
	return nRet;
}

int   msgMgr:: regAskOnly (msgIdType askId, serverIdType	askDefProcSer)
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

bool  msgMgr::isRetMsg (msgIdType msgId)
{
	auto& rMgr = m_retMap;
	auto it = rMgr.find (msgId);
	return nullptr != it;
}
