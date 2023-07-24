#ifndef _tokenSave_map_h__
#define _tokenSave_map_h__
#include <memory>
#include <unordered_map>
#include "iTokenSave.h"

class tokenSave_map:public iTokenSave
{
public:
    tokenSave_map ();
    ~tokenSave_map ();
	int netTokenPackInsert (NetTokenType token, const tokenInfo& info) override;
	int netTokenPackErase (NetTokenType token) override;
	tokenInfo* netTokenPackFind (NetTokenType token) override;

	typedef std::unordered_map<NetTokenType, tokenInfo>  tokenMap;
	tokenMap&  tokenS ();
private:
	tokenMap  m_tokenS;
};
#endif
