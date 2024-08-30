#ifndef _frameConfig__h_
#define _frameConfig__h_

#include "comFun.h"
#include <memory>
class frameConfig
{
public:
	frameConfig ();
	int  procCmdArgS (int nArg, char** argS);
	int  dumpConfig (const char* szFile);
	int  loadConfig (const char* szFile);
		  bool  allocDebug ();
  void  setAllocDebug (bool v);
  word  appNetType ();
  void  setAppNetType (word v);
  bool  clearTag ();
  void  setClearTag (bool v);
  udword  delSaveTokenTime ();
  void  setDelSaveTokenTime (udword v);
  ubyte  detachServerS ();
  void  setDetachServerS (ubyte v);
  bool  dumpMsg ();
  void  setDumpMsg (bool v);
  const char*  endPoint ();
  void  setEndPoint (const char* v);
  const char*  frameConfigFile ();
  void  setFrameConfigFile (const char* v);
  const char*  ip ();
  void  setIp (const char* v);
  const char*  level0 ();
  void  setLevel0 (const char* v);
  const char*  logFile ();
  void  setLogFile (const char* v);
  word  logLevel ();
  void  setLogLevel (word v);
  const char*  logicModel ();
  void  setLogicModel (const char* v);
  const char*  modelS ();
  void  setModelS (const char* v);
  const char*  netLib ();
  void  setNetLib (const char* v);
  uword  netNum ();
  void  setNetNum (uword v);
  const char*  runServerNum ();
  void  setRunServerNum (const char* v);
  udword  savePackTag ();
  void  setSavePackTag (udword v);
  const char*  serializePackLib ();
  void  setSerializePackLib (const char* v);
  bool  srand ();
  void  setSrand (bool v);
  uword  startPort ();
  void  setStartPort (uword v);
  udword  testTag ();
  void  setTestTag (udword v);
  const char*  workDir ();
  void  setWorkDir (const char* v);
private:
  bool  m_allocDebug;
  word  m_appNetType;
  bool  m_clearTag;
  udword  m_delSaveTokenTime;
  ubyte  m_detachServerS;
  bool  m_dumpMsg;
  std::unique_ptr <char[]>  m_endPoint;
  std::unique_ptr <char[]>  m_frameConfigFile;
  std::unique_ptr <char[]>  m_ip;
  std::unique_ptr <char[]>  m_level0;
  std::unique_ptr <char[]>  m_logFile;
  word  m_logLevel;
  std::unique_ptr <char[]>  m_logicModel;
  std::unique_ptr <char[]>  m_modelS;
  std::unique_ptr <char[]>  m_netLib;
  uword  m_netNum;
  std::unique_ptr <char[]>  m_runServerNum;
  udword  m_savePackTag;
  std::unique_ptr <char[]>  m_serializePackLib;
  bool  m_srand;
  uword  m_startPort;
  udword  m_testTag;
  std::unique_ptr <char[]>  m_workDir;
};
#endif
