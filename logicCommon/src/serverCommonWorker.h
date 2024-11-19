#ifndef _serverCommonWorker_h__
#define _serverCommonWorker_h__
#include <memory>

#include "logicWorker.h"

class serverCommonWorker: public logicWorker
{
public:
    serverCommonWorker ();
    ~serverCommonWorker ();
	int  sendPacket (packetHead* pPack, loopHandleType appGroupId, loopHandleType threadGroupId) override;
private:
};
#endif
