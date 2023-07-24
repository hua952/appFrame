#include "tokenSave_map.h"
#include "strFun.h"

tokenSave_map:: tokenSave_map ()
{
}

tokenSave_map:: ~tokenSave_map ()
{
}

int  tokenSave_map:: netTokenPackInsert (NetTokenType token, const tokenInfo& info)
{
    int  nRet = 0;
    do {
		auto& rMap = tokenS ();
		auto ret = rMap.insert (std::make_pair (token, info));
		if (ret.second) {
			nRet = 1;
		}
    } while (0);
    return nRet;
}

int  tokenSave_map:: netTokenPackErase (NetTokenType token)
{
    int  nRet = 0;
    do {
		auto& rMap = tokenS ();
		rMap.erase (token);
    } while (0);
    return nRet;
}

tokenInfo*  tokenSave_map:: netTokenPackFind (NetTokenType token)
{
    tokenInfo*  nRet = 0;
    do {
		auto& rMap = tokenS ();
		auto it = rMap.find (token);
		nRet = &(it->second);
    } while (0);
    return nRet;
}

tokenSave_map::tokenMap&  tokenSave_map:: tokenS ()
{
    return m_tokenS;
}

