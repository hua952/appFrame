#ifndef tStaticJumpListNS_h__
#define tStaticJumpListNS_h__
#include "typeDef.h"
#include <cstdlib>
#include <memory>
#include <stdlib.h>
#include "myAssert.h"

template<class T>
class  tStaticJumpListNSLessComp
{
public:
    bool operator ()(const T& a, const T& b)const
    {
        return a < b;
    }
};

template<class ValueT, int c_LevelNum = 4, class comFun =  tStaticJumpListNSLessComp<ValueT>,
	int c_LevelLen = 4, class indexT = uword>
class tStaticJumpListNS
{
public:
	struct NSJumpNode
	{
		ValueT          Data;
		indexT level[c_LevelNum];
	};
	typedef bool (*NSVisitFunType)(void* pUserData, ValueT& rV);
	bool isNull (indexT i)
	{
		return i == m_itemNum + 2;
	}
	void setNull (indexT& i)
	{
		i = m_itemNum + 2;
	}
    tStaticJumpListNS(indexT itemNum)
	{
		auto bitLen = sizeof (indexT) * 8;
		if (bitLen < 64) {
			uqword t = 1;
			t <<= bitLen;
			uqword uqwS = itemNum;
			uqwS += 2;
			myAssert (uqwS < t);
		}
		m_itemS = std::make_unique<NSJumpNode[]>(itemNum + 2);
		m_itemNum = itemNum;
		clear ();
	}
    ~tStaticJumpListNS()
	{
		clear();
	}
    void clear()
	{
		auto itemNum = m_itemNum;
		auto& rFree = freeHead ();
		auto& rHead = this->head ();

		for(int i = 0; i < c_LevelNum; i++) {
			setNull(rHead.level[i]);
			setNull(rFree.level[i]);
		}
		for (int j = 0; j < itemNum; j++) {
			auto& rJ = m_itemS[j];
			rJ.level[0] = rFree.level[0];
			rFree.level[0] = j;
		}
	}
    int insert(const ValueT& rV)
	{
		indexT  pAr[c_LevelNum];
		_Find(pAr, rV);
		auto& rP0 = m_itemS[pAr[0]];
		auto p0n = rP0.level[0];
		if(!isNull(p0n))
		{
			comFun fun;
			auto& rC = m_itemS[p0n];
			if(!fun(rV, rC.Data))
			{
				return 1; /* equie   相等 */
			}
		}
		ubyte lev = 1;
		auto iC = mallocNode (lev);
		if (isNull (iC)) {
			return 2; /* no space 没空间 */
		}
		auto& rC = m_itemS[iC];
		rC.Data = rV;
		for(ubyte i = 0; i < lev; i++)
		{
			auto& rPi = m_itemS [pAr[i]];
			rC.level[i] = rPi.level[i];
			rPi.level[i] = iC;
		}
		return 0;
	}
    bool erase(const ValueT& rV)
	{
		indexT  pAr[c_LevelNum];
		_Find(pAr, rV);
		auto iC = m_itemS[pAr[0]].level[0];
		if (isNull (iC)) {
			return false; //Not Find
		}
		comFun fun;
		auto& rC = m_itemS[iC];
		if(fun(rV, rC.Data)) {
			return false; //Not Find
		}
		
		for(int i = c_LevelNum - 1; i >= 0; --i) {
			if (m_itemS[pAr[i]].level[i] != iC) {
				continue;
			}
			m_itemS[pAr[i]].level[i] = rC.level[i];
		}
		freeNode(iC);
		return true;
	}

    ValueT* find(const ValueT& rV)
	{
		indexT  pAr[c_LevelNum];
		_Find(pAr, rV);
		auto iC = m_itemS[pAr[0]].level[0];
		if (isNull (iC)) {
			return nullptr;
		}
		comFun fun;
		auto& rC = m_itemS[iC];
		if(!fun(rV, rC.Data)) {
			return &rC.Data;
		}
		return nullptr;
	}

    void randVisit(const ValueT& rBegin, const ValueT& rEnd, void* pU, NSVisitFunType pFun)
	{
		auto iC = _Lower(rBegin);
		if(isNull (iC)) {
			return;
		}
		auto iE = _Lower(rEnd);
		while(iC != iE) {
			bool bC = pFun(pU, m_itemS[iC].Data);
			if(!bC) {
				break;
			}
			iC = m_itemS[iC].level[0];
		}
	}
    void visit(void* pU, NSVisitFunType pFun)
	{
		levelVisit(0, pU, pFun);
	}

    void levelVisit(int nLev, void* pU, NSVisitFunType pFun)
	{
		if(nLev >= 0 && nLev < c_LevelNum) {
			auto& head = this->head ();
			auto iC = head.level[nLev];
			while(!isNull(iC)) {
				bool bC = pFun(pU, m_itemS[iC].Data);
				if(!bC) {
					break;
				}
				iC = m_itemS[iC].level[nLev];
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
			auto& head = this->head ();
			auto iC = head.level[nLev];
			while(!isNull(iC)) {
				nRet++;
				iC = m_itemS[iC].level[nLev];
			}
		}
		return nRet;
	}

    void delAndVisit(const ValueT& rEnd, void* pU, NSVisitFunType pFun)
	{
		comFun fun;
		auto& head = this->head ();
		if(isNull (head.level[0])) {
			return;
		}
		if(fun(rEnd,m_itemS[head.level[0]].Data)) {
			return;
		}
		indexT  pArEnd[c_LevelNum];
		_Find(pArEnd, rEnd);  /*pArEnd里返回的值不可能是 null*/

		auto headIndex = this->headIndex ();
		if (pArEnd[0] == headIndex) {
			return;
		}

		auto delBegin = head.level[0];
		auto delEnd = m_itemS[pArEnd[0]].level[0];
		for(int i = c_LevelNum - 1; i>= 0; i--) {
			head.level[i] = m_itemS[pArEnd[i]].level[i];
		}
		auto iC = delBegin;
		bool bC = true;
		while(iC != delEnd) {
			if(NULL != pFun && bC) {
				bC = pFun(pU, m_itemS[iC].Data);
			}
			iC = m_itemS[iC].level[0];
		}
		freeList(delBegin, pArEnd[0]);
	}

    void randDelAndVisit(const ValueT& rBegin, const ValueT& rEnd, void* pU, NSVisitFunType pFun)
	{
		comFun fun;
		if(!fun(rBegin, rEnd)) {
			return;
		}

		indexT  pArBegin[c_LevelNum];
		_Find(pArBegin, rBegin);
		indexT  pArEnd[c_LevelNum];
		_Find(pArEnd, rEnd);
		if (pArBegin[0] == pArEnd[0]) {
			return;
		}
		auto delBegin = m_itemS[pArBegin[0]].level[0];
		auto delEnd = m_itemS[pArEnd[0]].level[0];
		for(int i = c_LevelNum - 1; i>= 0; i--)
		{
			m_itemS[pArBegin[i]].level[i] = m_itemS[pArEnd[i]].level[i];
		}
		auto iC = delBegin;
		bool bC = true;
		while(iC != delEnd) {
			if(NULL != pFun && bC) {
				bC = pFun(pU, m_itemS[iC].Data);
			}
			iC = m_itemS[iC].level[0];
		}
		freeList(delBegin, pArEnd[0]);
	}

    ValueT* LowerIndex(const ValueT& rV)
	{
		indexT  pAr[c_LevelNum];
		_Find(pAr, rV);
		return isNull (m_itemS[pAr[0]].level[0]) ? NULL : &m_itemS[m_itemS[pAr[0]].level[0]].Data;
	}

    bool  empty()
	{
		auto& head = this->head ();
		return isNull (head.level[0]);
	}
 private:
	void  _Find (indexT pAr[c_LevelLen], const ValueT& rV)
	{
		comFun fun;
		pAr[c_LevelNum - 1] = headIndex ();
		for(int i = c_LevelNum - 1; i>= 0; --i) {
			auto  cI = m_itemS[pAr[i]].level[i];
			while(!isNull (cI)) {
				auto& rC = m_itemS[cI];
				if(!fun(rC.Data, rV)) {
					break;
				}
				pAr[i] = cI;
				cI = m_itemS[pAr[i]].level[i];
			}
			if(i > 0) {
				pAr[i - 1] = pAr[i];
			}
		}
	}
    indexT  _Lower(const ValueT& rV)
	{
		indexT  pAr[c_LevelNum];
		_Find(pAr, rV);
		return m_itemS[pAr[0]].level[0];
	}
	indexT mallocNode (ubyte& lev)
	{
		auto& rFree = freeHead ();
		auto nRet = rFree.level[0];
		if (!isNull (nRet)) {
			auto& rRet = m_itemS[nRet];
			rFree.level[0] = rRet.level[0];

			double dMark = 0xff;
			dMark /= c_LevelLen;
			for (lev = 1; lev < c_LevelNum; lev++) {
				if ((double)((ubyte)rand()) >= dMark) {
					break;
				}
			}
		}
		return nRet;
	}
	void freeNode (indexT index)
	{
		if (!isNull (index)) {
			auto& rFree = freeHead ();
			auto& rI = m_itemS[index];
			rI.level[0] = rFree.level[0];
			rFree.level[0] = index;
		}
	}
	void freeList(indexT iBegin, indexT iEnd)
	{
		auto& rFree = freeHead ();
		m_itemS[iEnd].level[0] = rFree.level[0];
		rFree.level[0] = iBegin;
	}
	NSJumpNode& freeHead ()
	{
		return m_itemS[m_itemNum];
	}
	NSJumpNode& head ()
	{
		return m_itemS[m_itemNum + 1];
	}
	indexT headIndex ()
	{
		return m_itemNum + 1;
	}
	std::unique_ptr<NSJumpNode[]> m_itemS;
	indexT						  m_itemNum;
};


#endif
