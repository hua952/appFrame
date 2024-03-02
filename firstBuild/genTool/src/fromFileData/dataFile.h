#ifndef _dataFile_h__
#define _dataFile_h__
#include <memory>

class dataFile
{
public:
    dataFile ();
    ~dataFile ();
	const char*  dataName ();
	void  setDataName (const char* v);
	const char*  dataType ();
	void  setDataType (const char* v);
	int  dataLength ();
	void  setDataLength (int v);
	bool  wordSize ();
	void  setWordSize (bool v);

	bool  haveSize ();
	void  setHaveSize (bool v);
	bool  zeroEnd ();
	void  setZeroEnd (bool v);
	const char*  commit ();
	void  setCommit (const char* v);
	const char*  memberName ();
	void  setMemberName (const char* v);
	const char*  lowerName ();
	void  setLowerName (const char* v);
private:
	std::unique_ptr <char[]>  m_lowerName;
	std::unique_ptr <char[]>  m_memberName;
	std::unique_ptr <char[]>  m_dataType;
	std::unique_ptr <char[]>  m_dataName;
	bool  m_wordSize;std::unique_ptr <char[]>  m_commit;
	int  m_dataLength;
	bool  m_zeroEnd;
	bool  m_haveSize;
};
#endif
