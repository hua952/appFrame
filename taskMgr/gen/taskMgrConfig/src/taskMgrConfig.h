#ifndef _taskMgrConfig__h_
#define _taskMgrConfig__h_

#include "comFun.h"
#include <memory>
class taskMgrConfig
{
public:
	taskMgrConfig ();
	int  procCmdArgS (int nArg, char** argS);
	int  dumpConfig (const char* szFile);
	int  loadConfig (const char* szFile);
		  const char*  modelS ();
  void  setModelS (const char* v);
private:
  std::unique_ptr <char[]>  m_modelS;
};
#endif
