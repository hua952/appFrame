#ifndef _impPackSave_map_h__
#define _impPackSave_map_h__
#include <memory>
#include "iPackSave.h"
#include <unordered_map>

class impPackSave_map:public iPackSave
{
public:
    impPackSave_map ();
    ~impPackSave_map ();

	int threadTokenPackInsert (packetHead* pack) override;
	int threadTokenPackErase (NetTokenType token) override;
	packetHead* threadTokenPackFind (NetTokenType token) override;

	int netTokenPackInsert (packetHead* pack) override;
	int netTokenPackErase (NetTokenType token) override;
	packetHead* netTokenPackFind (NetTokenType token) override;
	using tokenPackMap = std::unordered_map <NetTokenType, packetHead*>;
	tokenPackMap&  tokenPackS ();
private:
	int tokenPackInsert (packetHead* pack, tokenPackMap& rMap);
	int tokenPackErase (NetTokenType token, tokenPackMap& rMap);
	packetHead* tokenPackFind (NetTokenType token, tokenPackMap& rMap);

	tokenPackMap  m_tokenPackS;
	tokenPackMap  m_netAskPackS;
};
#endif
