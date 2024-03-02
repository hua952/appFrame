#ifndef tDThrStaticJumpListNS_h__
#define tDThrStaticJumpListNS_h__
#include "typeDef.h"
#include <cstdlib>
#include <memory>
#include <stdlib.h>
#include "myAssert.h"
#include<cmath>

template<class T>
class  tDThrStaticJumpListNSLessComp
{
public:
    bool operator ()(const T& a, const T& b)const
    {
        return a < b;
    }
};

template<class ValueT, uqword m_itemNum, class comFun =  tDThrStaticJumpListNSLessComp<ValueT>, int c_levelLenBit = 1>
class tDThrStaticJumpListNS
{
public:
	typedef udword indexT;
	static const int c_blNum = 3;
	static const int c_levelLen = 1 << c_levelLenBit;
	typedef bool (*NSVisitFunType)(void* pUserData, ValueT& rV);
	
	static constexpr int cLevelNum()
	{
		uqword curNum = m_itemNum;
		int i = 0;
		for (; i < 64; i++) {
			curNum >>= 1;
			if (0 == curNum) {
				break;
			}
		}
		i /= c_levelLenBit;
		i++;
		return i;
	}
	static const int c_levelNum = cLevelNum ();
	static constexpr int c_blLevelS [c_blNum] = {2, 6, c_levelNum};
	static constexpr uqword cLargeNum ()
	{
		uqword ret = 0;
		double dF = 1;
		for (int i = 0; i < c_blLevelS[1]; i++) {
			dF /= c_levelLen;
		}
		dF *= m_itemNum;
		ret = (udword)dF;
		return ret;
	}
	static const uqword c_largeNum = cLargeNum ();
	static constexpr uqword cMidNum ()
	{
		uqword ret = 0;
		double dF = 1;
		for (int i = 0; i < c_blLevelS[0]; i++) {
			dF /= c_levelLen;
		}
		auto leafNum = m_itemNum - c_largeNum;
		dF *= leafNum;
		ret = (udword)dF;
		return ret;
	}
	static const uqword c_midNum = cMidNum ();
	static const uqword c_allItemNum = m_itemNum + 1;
	static constexpr int c_blItemNum [c_blNum] = {m_itemNum - c_midNum - c_largeNum, c_midNum,  c_largeNum + 1};
	static constexpr uqword cAllIndexNum ()
	{
		uqword ret = 0;
		for (int i = 0; i < c_blNum; i++) {
			ret += (c_blItemNum[i] * c_blLevelS [i]);
		}
		return ret;
	}
	static const uqword c_allIndexNum = cAllIndexNum ();

	bool isNull (indexT i)  // i in all item 
	{
		return i >= c_allItemNum;
	}
	void setNull (indexT& i) // i in all index
	{
		i = c_allItemNum;
	}
    tDThrStaticJumpListNS()
	{
		auto bitLen = sizeof (indexT) * 8 - 1;
		if (bitLen < 64) {
			uqword t = 1;
			t <<= bitLen;
			uqword uqwS = m_itemNum;
			uqwS += 3;
			myAssert (uqwS < t);
			int indBit = sizeof (indexT) * 8;
			uqword uqwT = 1;
			uqwT <<= indBit;
			myAssert (c_allIndexNum < uqwT);
		}
		clear ();
	}
    ~tDThrStaticJumpListNS()
	{
	}
	indexT  getIndexS (indexT nItem)
	{
		auto temp = nItem;
		indexT nRet = 0;
		for (int i = 0; i < c_blNum; i++) {
			auto allI = c_blItemNum [i] * c_blLevelS [i];
			if (temp < c_blItemNum [i]) {
				return nRet + temp * c_blLevelS[i];
			}
			temp -= c_blItemNum [i];
			nRet += allI;
		}
		return c_allIndexNum;
	}
	indexT head () // in item
	{
		return (m_itemNum);
	}
    void clear()
	{
		indexT nBase = 0;
		for (int i = 0; i < c_blNum; i++) {
			setNull(m_freeH[i]);
			auto itemNum = c_blItemNum [i];
			if (i == c_blNum - 1) {
				itemNum--;
			}
			for (indexT j = 0; j < itemNum; j++) {
				auto rI = nBase + j;
				auto curA = getIndexS (rI);
				m_indexS[curA] = m_freeH[i];
				m_freeH[i] = rI;
			}
			nBase += itemNum;
		}
		auto nH = head ();
		auto nHa = getIndexS (nH);
		for (indexT i = 0; i < c_levelNum; i++) {
			setNull(m_indexS[nHa + i]);
		}
	}
	int insert(const ValueT& rV)
	{
		indexT  pAr[c_levelNum];
		_Find(pAr, rV);
		auto p0 = pAr[0];
		auto p0a = getIndexS (pAr[0]);
		auto p0n = m_indexS[p0a];
		if(!isNull(p0n)) {
			comFun fun;
			auto& rC = getData(p0n);
			if(!fun(rV, rC)) {
				return 1; /* equie   相等 */
			}
		}
		ubyte lev = 1;
		auto iC = mallocNode (lev);
		if (isNull (iC)) {
			return 2; /* no space 没空间 */
		}
		auto& rC = getData (iC);
		rC = rV;
		auto iCa = getIndexS (iC);
		for(ubyte i = 0; i < lev; i++) {
			auto pi = pAr[i];
			auto piA = getIndexS (pi);
			m_indexS[iCa + i] = m_indexS[piA + i];
			m_indexS[piA + i] = iC;
		}
		return 0;
	}
    bool erase(const ValueT& rV)
	{
		indexT  pAr[c_levelNum];
		_Find(pAr, rV);
		auto p0a = getIndexS (pAr[0]);
		auto iC = m_indexS[p0a];
		if (isNull (iC)) {
			return false;
		}
		comFun fun;
		auto& rC = getData (iC);
		if(fun(rV, rC)) {
			return false;
		}
		auto iCa = getIndexS (iC);
		for(int i = c_levelNum - 1; i >= 0; --i) {
			auto pi = pAr[i];
			auto pia = getIndexS(pAr[i]);
			if (m_indexS[pia + i] != iC) {
				continue;
			}
			m_indexS[pia + i] = m_indexS[iCa + i];
		}
		freeNode(iC);
		return true;
	}

    ValueT* find(const ValueT& rV)
	{
		indexT  pAr[c_levelNum];
		_Find(pAr, rV);
		auto p0 = pAr[0];
		auto p0a = getIndexS (p0);
		auto iC = m_indexS[p0a];
		if (isNull (iC)) {
			return nullptr;
		}
		comFun fun;
		auto& rC = getData(iC);
		if(!fun(rV, rC)) {
			return &rC;
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
			auto& rData = getData (iC);
			bool bC = pFun(pU, rData);
			if(!bC) {
				break;
			}
			auto iCa = getIndexS (iC);
			iC = m_indexS[iCa];
		}
	}
    void visit(void* pU, NSVisitFunType pFun)
	{
		levelVisit(0, pU, pFun);
	}

    void levelVisit(int nLev, void* pU, NSVisitFunType pFun)
	{
		if(nLev >= 0 && nLev < c_levelNum) {
			auto  nH = head ();
			auto  nHa = getIndexS(nH);
			auto iC = m_indexS[nHa + nLev];
			while(!isNull(iC)) {
				auto& rData = getData (iC);
				bool bC = pFun(pU, rData);
				if(!bC) {
					break;
				}
				auto iCa = getIndexS (iC);
				iC = m_indexS[iCa + nLev];
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
		if(nLev >= 0 && nLev < c_levelNum) {
			auto nH = head ();
			auto nHa = getIndexS (nH);
			auto iC = m_indexS[nHa + nLev];
			while(!isNull(iC)) {
				nRet++;
				auto iCa = getIndexS (iC);
				iC = m_indexS[iCa + nLev];
			}
		}
		return nRet;
	}

    void delAndVisit(const ValueT& rEnd, void* pU, NSVisitFunType pFun)
	{
		comFun fun;
		auto nH = head ();
		auto nHa = getIndexS (nH);
		auto nC = m_indexS[nHa];
		if(isNull (nC)) {
			return;
		}
		auto& rData = getData (nC);
		if(fun(rEnd, rData)) {
			return;
		}
		indexT  pArEnd[c_levelNum];
		_Find(pArEnd, rEnd);  /*pArEnd里返回的值不可能是 null*/

		if (pArEnd[0] == nH) {
			return;
		}

		auto delBegin = nC;
		auto endA = getIndexS(pArEnd[0]);
		auto delEnd = m_indexS[endA];
		for(int i = c_levelNum - 1; i>= 0; i--) {
			auto endA = getIndexS (pArEnd[i]);
			m_indexS [nHa + i] = m_indexS[endA + i];
		}
		auto iC = delBegin;
		bool bC = true;
		while(iC != delEnd) {
			if(NULL != pFun && bC) {
				auto& rData = getData (iC);
				bC = pFun(pU, rData);
			}
			auto iD = iC;
			auto iCa = getIndexS (iC);
			iC = m_indexS[iCa];
			freeNode (iD);
		}
	}

    void randDelAndVisit(const ValueT& rBegin, const ValueT& rEnd, void* pU, NSVisitFunType pFun)
	{
		comFun fun;
		if(!fun(rBegin, rEnd)) {
			return;
		}
		indexT  pArBegin[c_levelNum];
		_Find(pArBegin, rBegin);
		indexT  pArEnd[c_levelNum];
		_Find(pArEnd, rEnd);
		if (pArBegin[0] == pArEnd[0]) {
			return;
		}
		auto beginA = getIndexS (pArBegin[0]);
		auto endA = getIndexS (pArEnd[0]);
		auto delBegin = m_indexS [beginA];
		auto delEnd = m_indexS [endA];
		for(int i = c_levelNum - 1; i>= 0; i--) {
			auto endA = getIndexS (pArEnd[i]);
			auto nextA = m_indexS[endA + i];
			m_indexS[beginA + i] = nextA;
		}
		auto iC = delBegin;
		bool bC = true;
		while(iC != delEnd) {
			if(NULL != pFun && bC) {
				auto& rData = getData (iC);
				bC = pFun(pU, rData);
			}
			auto iD = iC;
			auto iCa = getIndexS (iC);
			iC = m_indexS [iCa];
			freeNode(iD);
		}
	}

    ValueT* LowerIndex(const ValueT& rV)
	{
		indexT  pAr[c_levelNum];
		_Find(pAr, rV);
		auto iArA = getIndexS (pAr[0]);
		auto nN = m_indexS [iArA];
		return isNull (nN) ? nullptr : &getData (nN);
	}

    bool  empty()
	{
		auto headI = head ();
		auto headIa = getIndexS();
		return isNull (m_indexS[headIa]);
	}
 private:
	void  _Find (indexT pAr[c_levelLen], const ValueT& rV)
	{
		comFun fun;
		pAr[c_levelNum - 1] = head ();
		for(int i = c_levelNum - 1; i>= 0; --i) {
			auto  cur = pAr[i];
			auto  curA = getIndexS (cur);
			auto  cI = m_indexS[curA + i];
			while(!isNull (cI)) {
				auto& cDataI = getData (cI);
				if(!fun(cDataI, rV)) {
					break;
				}
				pAr[i] = cI;
				auto  cIA = getIndexS(cI);
				cI = m_indexS[cIA + i];
			}
			if(i > 0) {
				pAr[i - 1] = pAr[i];
			}
		}
	}

    indexT  _Lower(const ValueT& rV)
	{
		indexT  pAr[c_levelNum];
		_Find(pAr, rV);
		auto p0a = getIndexS(pAr[0]);
		return m_indexS [p0a];
	}

	indexT mallocNode (ubyte& lev)
	{
		double dMark = 0xff;
		dMark /= c_levelLen;
		for (lev = 1; lev < c_levelNum; lev++) {
			if ((double)((ubyte)rand()) >= dMark) {
				break;
			}
		}
		auto nRet = _mallocNode (lev);
		if (isNull (nRet)) {
			auto nMaxLev = c_blLevelS [0];
			if (lev > nMaxLev) {
				lev = nMaxLev;
			} else {
				lev = nMaxLev + 1;
			}
			nRet = _mallocNode (lev);
		}
		return nRet;
	}

	indexT _mallocNode (ubyte lev)
	{
		auto nH = 0;
		for (; lev > c_blLevelS [nH]; nH++);
		auto nRet = m_freeH[nH];
		if (!isNull (nRet)) {
			auto nRetA = getIndexS (nRet);
			m_freeH [nH] = m_indexS [nRetA];
		}
		return nRet;
	}

	void freeNode (indexT index)
	{
		if (index < m_itemNum) {
			auto nH = 0;
			auto temp = index;
			for (; temp >= c_blItemNum [nH]; nH++) {
				temp -= c_blItemNum [nH];
			}
			auto indexA = getIndexS (index);
			m_indexS [indexA] = m_freeH[nH];
			m_freeH[nH] = index;
		}
	}

	ValueT&  getData(indexT nItem)
	{
		return m_dataS[nItem];
	}
	ValueT	m_dataS[m_itemNum];
	indexT  m_freeH[c_blNum];
	indexT  m_indexS[c_allIndexNum];
};
#endif
