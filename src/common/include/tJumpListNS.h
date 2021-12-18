#ifndef _tJumpListNS_h_
#define _tJumpListNS_h_
#include"typeDef.h"
#include<stdlib.h>
#include"comFun.h"

template<class ValueT, int c_LevelNum = 10, class comFun = LessComp<ValueT>, int c_LevelLen = 4>
class tJumpListNS
{
 public:
	struct NSJumpNode
	{
		ValueT          Data;
		NSJumpNode* Level[c_LevelNum];
	};
	
	typedef NSJumpNode* PNSJumpNode;  
	typedef bool (*NSVisitFunType)(void* pUserData, ValueT& rV);

    tJumpListNS()
	{
		for(int i = 0; i < c_LevelNum; i++)
		{
			m_Head.Level[i] = NULL;
		}
	}
    ~tJumpListNS()
	{
		clear();
	}
    void clear()
	{
		NSJumpNode* pNode = m_Head.Level[0];
		while(NULL != pNode)
		{
			NSJumpNode* pD = pNode;
			pNode = pNode->Level[0];
			FreeNode(pD);
		}
		for(int i = 0; i < c_LevelNum; i++)
		{
			m_Head.Level[i] = NULL;
		}
	}
    bool insert(const ValueT& rV)
	{
		PNSJumpNode pAr[c_LevelNum];
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
		int nMaxIndex = 0;
		PNSJumpNode pInsert = MallocNode(nMaxIndex);
		pInsert->Data = rV;
		for(int i = 0; i <= nMaxIndex; i++)
		{
			pInsert->Level[i] = pAr[i]->Level[i];
			pAr[i]->Level[i] = pInsert;
		}
		return true;
	}

    bool erase(const ValueT& rV)
	{
		PNSJumpNode  pAr[c_LevelNum];
		int nLv = _Find2(pAr, rV);
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
		PNSJumpNode pAr[c_LevelNum];
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
		levelVisit(0, pU, pFun);
	}

    void levelVisit(int nLev, void* pU, NSVisitFunType pFun)
	{
		if(nLev >= 0 && nLev < c_LevelNum)
		{
			NSJumpNode* pC = m_Head.Level[nLev];
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

    int levelSize(int nLev)
	{
		int nRet = 0;
		if(nLev >= 0 && nLev < c_LevelNum)
		{
			NSJumpNode* pNode = m_Head.Level[nLev];
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
		if(NULL == m_Head.Level[0])
			return;
		if(fun(rEnd,m_Head.Level[0]->Data))
			return;
		PNSJumpNode endInfo[c_LevelNum];
		_LowerIndex(rEnd, endInfo);
		NSJumpNode* pDelBegin = m_Head.Level[0];
		NSJumpNode* pDelEnd = endInfo[0]->Level[0];
		for(int i = c_LevelNum - 1; i>= 0; i--)
		{
			m_Head.Level[i] = endInfo[i]->Level[i];
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
		PNSJumpNode beginInfo[c_LevelNum];
		PNSJumpNode endInfo[c_LevelNum];
		_LowerIndex(rBegin, beginInfo);
		_LowerIndex(rEnd, endInfo);
		NSJumpNode* pDelBegin = beginInfo[0]->Level[0];
		NSJumpNode* pDelEnd = endInfo[0]->Level[0];
		for(int i = c_LevelNum - 1; i>= 0; i--)
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
		PNSJumpNode pAr[c_LevelNum];
		_LowerIndex(rV, pAr);
		return NULL == pAr[0]->Level[0] ? NULL : &pAr[0]->Level[0]->Data ;
	}

    bool  empty()
	{
		return NULL == m_Head.Level[0];
	}

 private:
    void  _Find(PNSJumpNode pAr[c_LevelLen], const ValueT& rV)
	{
		comFun fun;
		pAr[c_LevelNum - 1] = &m_Head;
		for(int i = c_LevelNum - 1; i>= 0; --i)
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

    int  _Find2(PNSJumpNode pAr[c_LevelNum], const ValueT& rV)
	{
		comFun fun;
		int nRet = c_LevelNum - 1;
		pAr[c_LevelNum - 1] = &m_Head;
		for(int i = c_LevelNum - 1; i >= 0; --i)
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
		PNSJumpNode pAr[c_LevelNum];
		_Find(pAr, rV);
		return pAr[0]->Level[0];
	}

    void  _LowerIndex(const ValueT& rV, PNSJumpNode pAr[c_LevelNum])
	{
		pAr[c_LevelNum - 1] = &m_Head;
		for(int i = c_LevelNum - 1; i >= 0; i--)
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
	
    NSJumpNode* MallocNode(int& nLevelIndex)
	{
		nLevelIndex = 0;
		while(0==rand()% c_LevelLen)
		{
			nLevelIndex++;
			if(nLevelIndex >= c_LevelNum - 1)
			{
				break;
			}
		}
		int nUnuse = sizeof(NSJumpNode*) * (c_LevelNum - nLevelIndex - 1);
		int nSize = sizeof(NSJumpNode) - nUnuse;
		NSJumpNode* pR = (NSJumpNode*)(malloc(nSize));//(pIMem->Malloc(nSize));
		for(int i = 0; i <= nLevelIndex; i++)
		{
			pR->Level[i] = NULL;
		}
		return pR;
	}

    void     FreeNode(NSJumpNode* pNode)
	{
		freee(pNode);
	}

    NSJumpNode  m_Head;
};

typedef tJumpListNS<pIStringKey, 6, pIStringKeyCmp, 8> pIStringKeyJumpList;
#endif
