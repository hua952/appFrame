#ifndef _myLogger_h__
#define _myLogger_h__
#include <memory>
#include <fstream>
#include <mutex>
#include "typeDef.h"

class myLogger
{
public:
    myLogger (const char* fileName);
    ~myLogger ();
	uword  minLevel ();
	void  setMinLevel (uword v);
	int   logTxt (const char* szTxt, uword uwLevel);
	static uword getLevelIndex (uword uwLevel);
	static const uword c_comLevelNum = 5;
	static uword s_comLevel[c_comLevelNum ];
	static const char* s_comLevelShow[c_comLevelNum + 1];
	bool  logLevelType ();
	void  setLogLevelType (bool v);
	bool  logTime ();
	void  setLogTime (bool v);
	bool  logToCon ();
	void  setLogToCon (bool v);
	bool  logToFile ();
	void  setLogToFile (bool v);
	bool  logThreId ();
	void  setLogThreId (bool v);
private:
	void  writeToCon(const char* txt);
	void  writeToFile(const char* txt);
	bool  m_logThreId;
	bool  m_logToFile;
	bool  m_logToCon;
	bool  m_logTime;
	bool  m_logLevelType;
	uword  m_minLevel;
	std::ofstream m_os;
	std::mutex m_mtxCon;
	std::mutex m_mtxFile;
};
#endif
