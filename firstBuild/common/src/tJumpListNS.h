#ifndef _tJumpListNS_h_
#define _tJumpListNS_h_
#include "typeDef.h"
#include <stdlib.h>

template<class T>
class  tJumpListNSLessComp
{
public:
    bool operator ()(const T& a, const T& b)const
    {
        return a < b;
    }
};

template<class ValueT, int c_LevelNum = 10, class comFun =   tJumpListNSLessComp<ValueT>, int c_LevelLen = 4>
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
		for(int i = 0; i < c_LevelNum; i++) {
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
		while(NULL != pNode) {
			NSJumpNode* pD = pNode;
			pNode = pNode->Level[0];
			FreeNode(pD);
		}
		for(int i = 0; i < c_LevelNum; i++) {
			m_Head.Level[i] = NULL;
		}
	}
    bool insert(const ValueT& rV)
	{
		PNSJumpNode pAr[c_LevelNum];
		_Find(pAr, rV);
		NSJumpNode* pC = pAr[0]->Level[0];
		if(NULL != pC) {
			comFun fun;
			if(!fun(rV, pC->Data)) {
				return false;
			}
		}
		int nMaxIndex = 0;
		PNSJumpNode pInsert = MallocNode(nMaxIndex);
		pInsert->Data = rV;
		for(int i = 0; i <= nMaxIndex; i++) {
			pInsert->Level[i] = pAr[i]->Level[i];
			pAr[i]->Level[i] = pInsert;
		}
		return true;
	}

    bool erase(const ValueT& rV)
	{
		PNSJumpNode  pAr[c_LevelNum];
		_Find(pAr, rV);
		NSJumpNode* pC = pAr[0]->Level[0];
		if(NULL == pC) {
			return false; //Not Find
		}
		comFun fun;
		if(fun(rV, pC->Data)) {
			return false; //Not Find
		}

		for(int i = c_LevelNum - 1; i >= 0; --i) {
			if (pAr[i]->Level[i] != pC) {
				continue;
			}
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
		if(NULL != pC) {
			comFun fun;
			if(!fun(rV, pC->Data)) {
				return &pC->Data;
			}
		}
		return NULL;
	}

    void randVisit(const ValueT& rBegin, const ValueT& rEnd, void* pU, NSVisitFunType pFun)
	{
		NSJumpNode* pC = _Lower(rBegin);
		if(NULL == pC) {
			return;
		}
		NSJumpNode* pEnd = _Lower(rEnd);
		while(pC != pEnd) {
			bool bC = pFun(pU,pC->Data);
			if(!bC) {
				break;
			}
			pC = pC->Level[0];
		}
	}

    void visit(void* pU, NSVisitFunType pFun) {
		levelVisit(0, pU, pFun);
	}

    void levelVisit(int nLev, void* pU, NSVisitFunType pFun)
	{
		if(nLev >= 0 && nLev < c_LevelNum) {
			NSJumpNode* pC = m_Head.Level[nLev];
			while(NULL != pC) {
				bool bC = pFun(pU,pC->Data);
				if(!bC) {
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
		if(nLev >= 0 && nLev < c_LevelNum) {
			NSJumpNode* pNode = m_Head.Level[nLev];
			while(NULL != pNode) {
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
		_Find(endInfo, rEnd);
		NSJumpNode* pDelBegin = m_Head.Level[0];
		NSJumpNode* pDelEnd = endInfo[0]->Level[0];
		for(int i = c_LevelNum - 1; i>= 0; i--) {
			m_Head.Level[i] = endInfo[i]->Level[i];
		}
		NSJumpNode* pC = pDelBegin;
		bool bC = true;
		while(pDelEnd != pC) {
			if(NULL != pFun && bC) {
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
		_Find (beginInfo, rBegin);
		_Find (endInfo, rEnd);
		NSJumpNode* pDelBegin = beginInfo[0]->Level[0];
		NSJumpNode* pDelEnd = endInfo[0]->Level[0];
		for(int i = c_LevelNum - 1; i>= 0; i--) {
			beginInfo[i]->Level[i] = endInfo[i]->Level[i];
		}
		NSJumpNode* pC = pDelBegin;
		bool bC = true;
		while(pDelEnd != pC) {
			if(NULL != pFun && bC) {
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
		_Find(pAr, rV);
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
		for(int i = c_LevelNum - 1; i>= 0; --i) {
			while(NULL != pAr[i]->Level[i]) {
				NSJumpNode* pC = pAr[i]->Level[i];
				if(!fun(pC->Data, rV)) {
					break;
				}
				pAr[i] = pC;
			}
			if(i > 0) {
				pAr[i - 1] = pAr[i];
			}
		}
	}
	
    NSJumpNode*  _Lower(const ValueT& rV)
	{
		PNSJumpNode pAr[c_LevelNum];
		_Find(pAr, rV);
		return pAr[0]->Level[0];
	}
	
    NSJumpNode* MallocNode(int& nLevelIndex)
	{
		nLevelIndex = 0;
		double dMark = 0xff;
		dMark /= c_LevelLen;
		while((double)((ubyte)rand()) < dMark) {
			nLevelIndex++;
			if(nLevelIndex >= c_LevelNum - 1) {
				break;
			}
		}
		int nUnuse = sizeof(NSJumpNode*) * (c_LevelNum - nLevelIndex - 1);
		int nSize = sizeof(NSJumpNode) - nUnuse;
		NSJumpNode* pR = (NSJumpNode*)(malloc(nSize));
		for(int i = 0; i <= nLevelIndex; i++) {
			pR->Level[i] = NULL;
		}
		return pR;
	}
    void     FreeNode(NSJumpNode* pNode)
	{
		free(pNode);
	}
    NSJumpNode  m_Head;
};

#endif
