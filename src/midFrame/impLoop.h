#ifndef _impLoog_h_
#define _impLoog_h_
#include "loop.h"
#include "arrayMap.h"
#include "mainLoop.h"
#include <memory>
class impLoop
{
public:
	impLoop();
	~impLoop();
	int init(const char* szName);
	void clean();

	bool regMsg(uword uwMsgId, procPacketFunType pFun);
	bool	removeMsg(uword uwMsgId);
	procPacketFunType findMsg(uword uwMsgId);
	const char* name();
	int OnLoopFrame();
	int processOncePack(packetHead* pPack);
	ModelIDType		id();
	void			setId(ModelIDType id);
	typedef arrayMap<uword, procPacketFunType> MsgMap;

private:
	MsgMap	m_MsgMap;
	frameFunType	m_funOnFrame;
	void*			m_pArg;
	ModelIDType		m_id;
	std::unique_ptr<char[]>	 m_name;
};

#endif
