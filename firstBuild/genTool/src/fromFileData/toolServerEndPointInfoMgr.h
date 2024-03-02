#ifndef _toolServerEndPointInfoMgr_h__
#define _toolServerEndPointInfoMgr_h__
#include <memory>
#include "serverFile.h"
#include <map>

class toolServerEndPointInfoMgr
{
public:
    toolServerEndPointInfoMgr ();
    ~toolServerEndPointInfoMgr ();
	using endPointMap = std::map<std::string, std::pair<serverFile*, toolServerEndPointInfo*>>;
	endPointMap&  endPointS ();
private:
	endPointMap  m_endPointS;
};
#endif
