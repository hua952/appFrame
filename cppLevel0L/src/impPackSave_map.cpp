#include "impPackSave_map.h"
#include "strFun.h"
#include "myAssert.h"
#include "tSingleton.h"
#include "workServerMgr.h"

impPackSave_map:: impPackSave_map ()
{
}

impPackSave_map:: ~impPackSave_map ()
{
}

impPackSave_map::tokenPackMap&  impPackSave_map:: netAskPackS ()
{
    return m_netAskPackS;
}

int  impPackSave_map:: netTokenPackInsert (packetHead* pack)
{
	auto& rAskS = netAskPackS ();
	auto pN = P2NHead (pack);
	auto inRet = rAskS.insert (std::make_pair(pN->dwToKen, pack));
	myAssert (inRet.second);
	int nRet = 0;
	do {
		if (!inRet.second) {
			nRet = 1;
			break;
		}
	} while (0);
	return nRet;
}

int  impPackSave_map:: netTokenPackErase (NetTokenType token)
{
	int nRet = 0;
	auto& rAskS = netAskPackS ();
	do {
		auto it = rAskS.find (token);
		if (rAskS.end () == it) {
			nRet = 1;
			break;
		}
		rAskS.erase (it);
	} while (0);
	return nRet;
}

packetHead*  impPackSave_map:: netTokenPackFind (NetTokenType token)
{
	auto& rAskS = netAskPackS ();
	packetHead* pRet = nullptr;
	do {
		auto it = rAskS.find (token);
		if (it == rAskS.end ()) {
			break;
		}
		pRet = it->second;
	} while (0);
	return pRet;
}

void impPackSave_map::clean ()
{
	// auto fnFreePack = tSingleton<loopMgr>::single().getPhyCallback().fnFreePack;
	auto& rAskS = netAskPackS ();
	for (auto it = rAskS.begin(); rAskS.end() != it; ++it) {
		// fnFreePack (it->second);
		freePack (it->second);
	}
	rAskS.clear();
}
	
int  impPackSave_map:: oldTokenInsert (NetTokenType newToken, tokenInfo& rInfo)
{
    int  nRet = 0;
    do {
		auto& rMap = oldTokenS ();
		auto inRet = rMap.insert (std::make_pair(newToken, rInfo));
		myAssert (inRet.second);
		if (!inRet.second) {
			nRet = 1;
			break;
		}
    } while (0);
    return nRet;
}

int  impPackSave_map:: oldTokenErase (NetTokenType newToken)
{
    int  nRet = 0;
    do {
		auto& rMap = oldTokenS ();
		auto it = rMap.find (newToken);
		if (rMap.end () == it) {
			nRet = 1;
			break;
		}
		rMap.erase (it);
    } while (0);
    return nRet;
}

tokenInfo* impPackSave_map:: oldTokenFind (NetTokenType newToken)
{
	tokenInfo* nRet = nullptr;
    do {
		auto& rMap = oldTokenS ();
		auto it = rMap.find (newToken);
		if (rMap.end () != it) {
			nRet = &it->second;
		}
    } while (0);
    return nRet;
}

impPackSave_map::oldTokenMap&  impPackSave_map:: oldTokenS ()
{
    return m_oldTokenS;
}

