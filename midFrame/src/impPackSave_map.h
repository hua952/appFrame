#ifndef _impPackSave_map_h__
#define _impPackSave_map_h__
#include <memory>
#include "iPackSave.h"
#include <unordered_map>

class impPackSave_map:public iPackSave
{
public:
	using tokenPackMap = std::unordered_map<NetTokenType, packetHead*>;
	using oldTokenMap = std::unordered_map<NetTokenType, tokenInfo>;
    impPackSave_map ();
    ~impPackSave_map ();
	void clean ();
	int netTokenPackInsert (packetHead* pack) override;
	int netTokenPackErase (NetTokenType token) override;
	packetHead* netTokenPackFind (NetTokenType token) override;

	int oldTokenInsert (NetTokenType newToken, tokenInfo& rInfo) override;
	int oldTokenErase (NetTokenType newToken)  override;
	tokenInfo* oldTokenFind (NetTokenType newToken)  override;

	tokenPackMap&  netAskPackS ();
	oldTokenMap&  oldTokenS ();
private:
	oldTokenMap  m_oldTokenS;
	tokenPackMap  m_netAskPackS;
};
#endif
