#ifndef _tJumpListNSAuto_h_
#define _tJumpListNSAuto_h_
#include"typeDef.h"
#include"comFun.h"
#include<stdlib.h>
//#include <memory>

template<class ValueT, class comFun = LessComp<ValueT>, ubyte c_startLevelNum = 2,  int c_LevelLen = 3>
class tJumpListNSAuto
{
 public:
	struct NSJumpNode
	{
		ValueT          Data;
		NSJumpNode* Level[1];
	};
	
	typedef NSJumpNode* PNSJumpNode;  
	typedef bool (*NSVisitFunType)(void* pUserData, ValueT& rV);
	static  const ubyte c_MaxLevelNum = 32;
     tJumpListNSAuto()
	{
		m_pHead = _MallocNode(c_startLevelNum -1);
		*((ubyte*)&m_pHead->Data) = c_startLevelNum;
	}
    ~tJumpListNSAuto()
	{
		clear();
        FreeNode(m_pHead);
	}
    void clear()
	{
		ubyte ubyH = height();
		NSJumpNode* pNode = m_pHead->Level[0];
		while(NULL != pNode)
		{
			NSJumpNode* pD = pNode;
			pNode = pNode->Level[0];
			FreeNode(pD);
		}
		for(int i = 0; i < ubyH; i++)
		{
			m_pHead->Level[i] = NULL;
		}
	}
    bool insert(const ValueT& rV)
	{
		PNSJumpNode pAr[c_MaxLevelNum ];
		_Find(pAr, rV);
		NSJumpNode* pC = pAr[0]->Level[0];
		if(NULL != pC)
		{
			comFun fun;
			if(!fun(rV, pC->Data))
			{
				return false;
			}
		}
		ubyte ubyM = 0;
		PNSJumpNode pInsert = MallocNode(ubyM);
		int nMaxIndex = ubyM;
		pInsert->Data = rV;
		for(int i = 0; i <= nMaxIndex; i++)
		{
			pInsert->Level[i] = pAr[i]->Level[i];
			pAr[i]->Level[i] = pInsert;
		}

		ubyte ubyH = height();
		udword udwS = levelSize(ubyH - 1);
		if(udwS > 	c_LevelLen )
		{
			incHeight();
		}
		return true;
	}

    bool erase(const ValueT& rV)
	{
		PNSJumpNode  pAr[c_MaxLevelNum];
		ubyte nLv = _Find2(pAr, rV);
		NSJumpNode* pC = pAr[nLv]->Level[nLv];
		if(NULL == pC)
		{
			return false; //Not Find
		}
		comFun fun;
		if(fun(rV, pC->Data))
		{
			return false; //Not Find
		}
		for(int i = nLv - 1; i >= 0; --i)
		{
			pAr[i] = pAr[i + 1];
			while(pAr[i]->Level[i] != pC)
			{
				pAr[i] = pAr[i]->Level[i];
			}
		}
		for(int i = nLv; i >= 0; --i)
		{
			pAr[i]->Level[i] = pC->Level[i];
		}
		
		FreeNode(pC);
		return true;
	}

    ValueT* find(const ValueT& rV)
	{
		PNSJumpNode pAr[c_MaxLevelNum];
		_Find(pAr, rV);
		NSJumpNode* pRet = NULL;
		NSJumpNode* pC = pAr[0]->Level[0];
		if(NULL != pC)
		{
			comFun fun;
			if(!fun(rV, pC->Data))
			{
				return &pC->Data;
			}
		}
		return NULL;
	}

    void randVisit(const ValueT& rBegin, const ValueT& rEnd, void* pU, NSVisitFunType pFun)
	{
		NSJumpNode* pC = _Lower(rBegin);
		if(NULL == pC)
		{
			return;
		}
		NSJumpNode* pEnd = _Lower(rEnd);
		while(pC != pEnd)
		{
			bool bC = pFun(pU,pC->Data);
			if(!bC)
			{
				break;
			}
			pC = pC->Level[0];
		}
	}

    void visit(void* pU, NSVisitFunType pFun)
	{
		//ubyte u = 0;
		levelVisit(0, pU, pFun);
	}

    void levelVisit(ubyte nLev, void* pU, NSVisitFunType pFun)
	{
		ubyte nH = height();
		if(nLev >= 0 && nLev < nH)
		{
			NSJumpNode* pC = m_pHead->Level[nLev];
			while(NULL != pC)
			{
				bool bC = pFun(pU,pC->Data);
				if(!bC)
				{
					break;
				}
				pC = pC->Level[nLev];
			}
		}
	}

    int size()
	{
		return levelSize(0);
	}

    int levelSize(ubyte nLev)
	{
		ubyte nH = height();
		int nRet = 0;
		if(nLev >= 0 && nLev < nH)
		{
			NSJumpNode* pNode = m_pHead->Level[nLev];
			while(NULL != pNode)
			{
				nRet++;
				pNode = pNode->Level[nLev];
			}
		}
		return nRet;
	}

    void delAndVisit(const ValueT& rEnd, void* pU, NSVisitFunType pFun)
	{
		comFun fun;
		if(NULL == m_pHead->Level[0])
			return;
		if(fun(rEnd,m_pHead->Level[0]->Data))
			return;
		PNSJumpNode endInfo[c_MaxLevelNum];
		_LowerIndex(rEnd, endInfo);
		NSJumpNode* pDelBegin = m_pHead->Level[0];
		NSJumpNode* pDelEnd = endInfo[0]->Level[0];
		ubyte nH = height();
		for(int i = nH - 1; i>= 0; i--)
		{
			m_pHead->Level[i] = endInfo[i]->Level[i];
		}
		NSJumpNode* pC = pDelBegin;
		bool bC = true;
		while(pDelEnd != pC)
		{
			if(NULL != pFun && bC)
			{
				pFun(pU, pC->Data);
			}
			NSJumpNode* pD = pC;
			pC = pC->Level[0];
			FreeNode(pD);
		}
	}

    void randDelAndVisit(const ValueT& rBegin, const ValueT& rEnd, void* pU, NSVisitFunType pFun)
	{
		comFun fun;
		if(!fun(rBegin, rEnd))
			return;
		PNSJumpNode beginInfo[c_MaxLevelNum];
		PNSJumpNode endInfo[c_MaxLevelNum];
		_LowerIndex(rBegin, beginInfo);
		_LowerIndex(rEnd, endInfo);
		NSJumpNode* pDelBegin = beginInfo[0]->Level[0];
		NSJumpNode* pDelEnd = endInfo[0]->Level[0];
		ubyte nH = height();
		for(int i = nH - 1; i>= 0; i--)
		{
			beginInfo[i]->Level[i] = endInfo[i]->Level[i];
		}
		NSJumpNode* pC = pDelBegin;
		bool bC = true;
		while(pDelEnd != pC)
		{
			if(NULL != pFun && bC)
			{
				bC = pFun(pU, pC->Data);
			}
			NSJumpNode* pD = pC;
			pC = pC->Level[0];
			FreeNode(pD);
		}
	}

    ValueT* LowerIndex(const ValueT& rV)
	{
		PNSJumpNode pAr[c_MaxLevelNum];
		_LowerIndex(rV, pAr);
		return NULL == pAr[0]->Level[0] ? NULL : &pAr[0]->Level[0]->Data ;
	}

    bool  empty()
	{
		return NULL == m_pHead->Level[0];
	}
	ubyte height()
	{
		return *((ubyte*)&m_pHead->Data);
	}
 private:
    void  _Find(PNSJumpNode pAr[c_MaxLevelNum], const ValueT& rV)
	{
		comFun fun;
		ubyte nH = height();
		pAr[nH - 1] = m_pHead;
		for(int i = nH - 1; i>= 0; --i)
		{
			while(NULL != pAr[i]->Level[i])
			{
				NSJumpNode* pC = pAr[i]->Level[i];
				if(!fun(pC->Data, rV))
				{
					break;
				}
				pAr[i] = pC;
			}
			if(i > 0)
			{
				pAr[i - 1] = pAr[i];
			}
		}
	}

    int  _Find2(PNSJumpNode pAr[c_MaxLevelNum], const ValueT& rV)
	{
		comFun fun;
		ubyte nH = height();
		int nRet = nH - 1;
		pAr[nH - 1] = m_pHead;
		for(int i = nH - 1; i >= 0; --i)
		{
			nRet = i;
			while(NULL != pAr[i]->Level[i])
			{
				NSJumpNode* pC = pAr[i]->Level[i];
				if(!fun(pC->Data, rV))
				{
					if(!fun(rV, pC->Data))
					{
						return nRet;
					}
					break;
				}
				pAr[i] = pC;
			}
			if(i > 0)
			{
				pAr[i - 1] = pAr[i];
			}
		}
		return nRet;
	}

    NSJumpNode*  _Lower(const ValueT& rV)
	{
		PNSJumpNode pAr[c_MaxLevelNum];
		_Find(pAr, rV);
		return pAr[0]->Level[0];
	}

    void  _LowerIndex(const ValueT& rV, PNSJumpNode pAr[c_MaxLevelNum])
	{
		ubyte nH = height();
		pAr[nH - 1] = m_pHead;
		for(int i = nH - 1; i >= 0; i--)
		{
			NSJumpNode* pC = pAr[i]->Level[i];
			comFun fun;
			while(NULL != pC)
			{
				if(!fun(pC->Data, rV))
				{
					break;
				}

				pAr[i] = pC;
				pC = pC->Level[i];
			}
			if(i > 0)
			{
				pAr[i - 1] = pAr[i];
			}
		}
	}
		
	void incHeight()
	{
		ubyte ubyH = height();
		NSJumpNode* pN = _MallocNode(ubyH);
		*pN = *m_pHead;
		for(ubyte i = 0; i < ubyH; i++)
		{
			pN->Level[i] = m_pHead->Level[i];
		}
		pN->Level[ubyH] = NULL;
		FreeNode(m_pHead);
		m_pHead = pN;
		*((ubyte*)&m_pHead->Data) = ubyH + 1;
	}
    NSJumpNode* _MallocNode(ubyte nLevelIndex)
	{
		int nSize = sizeof(NSJumpNode);
		if(nLevelIndex > 0)
		{
			nSize += (sizeof(NSJumpNode*) * (nLevelIndex ));
		}
		//IMemMgr* pIMem = GetMemMgr();
		NSJumpNode* pR = (NSJumpNode*)(malloc(nSize));//(pIMem->Malloc(nSize));
		for(int i = 0; i <= nLevelIndex; i++)
		{
			pR->Level[i] = NULL;
		}
		return pR;
	}
    NSJumpNode* MallocNode(ubyte& nLevelIndex)
	{
		ubyte ubyH = height();
		nLevelIndex = 0;
		ubyH--;
		while(nLevelIndex < ubyH && 0==rand()% c_LevelLen)
		{
			nLevelIndex++;
		}
		return _MallocNode(nLevelIndex);
	}

    void          FreeNode(NSJumpNode* pNode)
	{
		//IMemMgr* pIMem = GetMemMgr();
		//pIMem->Free(pNode);
		free(pNode);
	}

    //NSJumpNode  m_Head;
	NSJumpNode*   m_pHead;
};

#endif
