#ifndef _frameConfig__h_
#define _frameConfig__h_

#include "comFun.h"
#include <memory>
class frameConfig
{
public:
	frameConfig ();
	int  procCmdArgS (int nArg, char** argS);
	int  loadConfig (const char* szFile);
		  const char*  addLogic ();
  void  setAddLogic (const char* v);
  bool  clearTag ();
  void  setClearTag (bool v);
  ubyte  detachServerS ();
  void  setDetachServerS (ubyte v);
  const char*  endPoint ();
  void  setEndPoint (const char* v);
  const char*  frameConfigFile ();
  void  setFrameConfigFile (const char* v);
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
  uword  procId ();
  void  setProcId (uword v);
  const char*  serializePackLib ();
  void  setSerializePackLib (const char* v);
  const char*  workDir ();
  void  setWorkDir (const char* v);
private:
  std::unique_ptr <char[]>  m_addLogic;
  bool  m_clearTag;
  ubyte  m_detachServerS;
  std::unique_ptr <char[]>  m_endPoint;
  std::unique_ptr <char[]>  m_frameConfigFile;
  std::unique_ptr <char[]>  m_level0;
  std::unique_ptr <char[]>  m_logFile;
  word  m_logLevel;
  std::unique_ptr <char[]>  m_logicModel;
  std::unique_ptr <char[]>  m_modelS;
  std::unique_ptr <char[]>  m_netLib;
  uword  m_procId;
  std::unique_ptr <char[]>  m_serializePackLib;
  std::unique_ptr <char[]>  m_workDir;
};
#endif
