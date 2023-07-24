#include "impPackSave_map.h"
#include "strFun.h"

impPackSave_map:: impPackSave_map ()
{
}

impPackSave_map:: ~impPackSave_map ()
{
}

impPackSave_map::tokenPackMap&  impPackSave_map:: tokenPackS ()
{
    return m_tokenPackS;
}


int impPackSave_map::threadTokenPackInsert (packetHead* pack)
{
    return tokenPackInsert (pack, m_tokenPackS);
}

int impPackSave_map::threadTokenPackErase (NetTokenType token)
{
    return tokenPackErase (token, m_tokenPackS);
}

packetHead* impPackSave_map::threadTokenPackFind (NetTokenType token)
{
    return tokenPackFind (token, m_tokenPackS);
}

int  impPackSave_map:: netTokenPackInsert (packetHead* pack)
{
    return tokenPackInsert (pack, m_netAskPackS);
}

int  impPackSave_map:: netTokenPackErase (NetTokenType token)
{
    return tokenPackErase (token, m_netAskPackS);
}

packetHead*  impPackSave_map:: netTokenPackFind (NetTokenType token)
{
    return tokenPackFind (token, m_netAskPackS);
}

int  impPackSave_map:: tokenPackInsert (packetHead* pack, tokenPackMap& rMap)
{
	int  nRet = 0;
    do {
		auto pN = P2NHead(pack);
		auto paRet = rMap.insert (std::make_pair(pN->dwToKen, pack));
    } while (0);
    return nRet;
}

int  impPackSave_map:: tokenPackErase (NetTokenType token, tokenPackMap& rMap)
{
	int  nRet = 0;
    do {
		rMap.erase (token);
    } while (0);
    return nRet;
}

packetHead*  impPackSave_map:: tokenPackFind (NetTokenType token, tokenPackMap& rMap)
{
	packetHead*  nRet = nullptr;
    do {
		auto it = rMap.find (token);
		if (rMap.end () != it) {
			nRet = it->second;
		}
    } while (0);
    return nRet;
}


