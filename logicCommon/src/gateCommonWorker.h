#ifndef _gateCommonWorker_h__
#define _gateCommonWorker_h__
#include <memory>
#include "logicWorker.h"

class gateCommonWorker:public logicWorker
{
public:
    gateCommonWorker ();
    ~gateCommonWorker ();
	int  sendPacket (packetHead* pPack, loopHandleType appGroupId, loopHandleType threadGroupId) override;
private:
};
#endif
