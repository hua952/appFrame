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
  uword  appGroupId ();
  void  setAppGroupId (uword v);
  uword  appIndex ();
  void  setAppIndex (uword v);
  word  appNetType ();
  void  setAppNetType (word v);
  bool  clearTag ();
  void  setClearTag (bool v);
  uword  dbgSleep ();
  void  setDbgSleep (uword v);
  udword  delSaveTokenTime ();
  void  setDelSaveTokenTime (udword v);
  bool  dumpMsg ();
  void  setDumpMsg (bool v);
  const char*  endPoint ();
  void  setEndPoint (const char* v);
  const char*  frameConfigFile ();
  void  setFrameConfigFile (const char* v);
  const char*  frameHome ();
  void  setFrameHome (const char* v);
  const char*  gateInfo ();
  void  setGateInfo (const char* v);
  const char*  homeDir ();
  void  setHomeDir (const char* v);
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
  const char*  modelName ();
  void  setModelName (const char* v);
  const char*  modelS ();
  void  setModelS (const char* v);
  const char*  netLib ();
  void  setNetLib (const char* v);
  uword  netNum ();
  void  setNetNum (uword v);
  const char*  runWorkNum ();
  void  setRunWorkNum (const char* v);
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
private:
  bool  m_allocDebug;
  uword  m_appGroupId;
  uword  m_appIndex;
  word  m_appNetType;
  bool  m_clearTag;
  uword  m_dbgSleep;
  udword  m_delSaveTokenTime;
  bool  m_dumpMsg;
  std::unique_ptr <char[]>  m_endPoint;
  std::unique_ptr <char[]>  m_frameConfigFile;
  std::unique_ptr <char[]>  m_frameHome;
  std::unique_ptr <char[]>  m_gateInfo;
  std::unique_ptr <char[]>  m_homeDir;
  std::unique_ptr <char[]>  m_ip;
  std::unique_ptr <char[]>  m_level0;
  std::unique_ptr <char[]>  m_logFile;
  word  m_logLevel;
  std::unique_ptr <char[]>  m_logicModel;
  std::unique_ptr <char[]>  m_modelName;
  std::unique_ptr <char[]>  m_modelS;
  std::unique_ptr <char[]>  m_netLib;
  uword  m_netNum;
  std::unique_ptr <char[]>  m_runWorkNum;
  udword  m_savePackTag;
  std::unique_ptr <char[]>  m_serializePackLib;
  bool  m_srand;
  uword  m_startPort;
  udword  m_testTag;
};
#endif
