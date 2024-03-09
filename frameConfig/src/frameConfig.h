#ifndef _frameConfig__h_
#define _frameConfig__h_

#include "comFun.h"
#include <memory>
class frameConfig
{
public:
	frameConfig ();
	int  procCmdArgS (int nArg, char** argS);
		  bool  clearTag ();
  void  setClearTag (bool v);
  const char*  depInclude ();
  void  setDepInclude (const char* v);
  word  sendCount ();
  void  setSendCount (word v);
  ubyte  sendMember ();
  void  setSendMember (ubyte v);
  int  sendNum ();
  void  setSendNum (int v);
  udword  sendSetp ();
  void  setSendSetp (udword v);
private:
  bool  m_clearTag;
  std::unique_ptr <char[]>  m_depInclude;
  word  m_sendCount;
  ubyte  m_sendMember;
  int  m_sendNum;
  udword  m_sendSetp;
};
#endif
