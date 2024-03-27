#ifndef __T_Algorithm___h_
#define __T_Algorithm___h_

#include "typeDef.h"
#include "comFun.h"

template<class ValueVectorType, class ValueType, class LessFunType=LessComp<ValueType>>
class TQuickSort
{
public:
	TQuickSort(ValueVectorType& pBuff, qword	nBuffNum):m_pBuff(pBuff), m_nBuffNum(nBuffNum){}
	void sort(){
		QuickSort_Node(0, m_nBuffNum - 1);
	}
private:
	qword QuickSortSub(qword nB, qword nE)
	{
		qword nRet = nB;
		LessFunType leFun;
		do{
			ValueType TempV;
			// qword nC = (nB + nE) >> 1;
			TempV = m_pBuff[nB];
			// m_pBuff[nC] = m_pBuff[nB];
			while(nB < nE){
				while(nB < nE && !leFun(m_pBuff[nE], TempV))nE--;
				m_pBuff[nB] = m_pBuff[nE];
				while(nB < nE && !leFun(TempV, m_pBuff[nB]))nB++;
				m_pBuff[nE] = m_pBuff[nB];
			}
			m_pBuff[nB] = TempV;
			nRet = nB;
		}while(0);
		return nRet;
	}
	void QuickSort_Node(qword nB, qword nE){
		if(nB < nE){
			qword nC = QuickSortSub(nB, nE);
			if(nC > 0){
				QuickSort_Node(nB, nC - 1);
			}
			QuickSort_Node(nC + 1, nE);
		}
	}
private:
	ValueVectorType&		m_pBuff;
	qword			m_nBuffNum;
};

template<class ValueType, class LessFunType=LessComp<ValueType>>
class TBinarySearch
{
public:
	TBinarySearch(ValueType* pBuff, qword	nBuffNum):m_pBuff(pBuff), m_nBuffNum(nBuffNum){}
	qword	Search(const ValueType& fv)
	{
		qword		qwRet = 0;
		qword		qwBegin = 0;
		qword		qwEnd = m_nBuffNum - 1;

		LessFunType	SLessFun;
		while (qwEnd >= qwBegin)
		{
			qwRet = qwBegin + ((qwEnd - qwBegin) >> 1);
			ValueType& vMid = m_pBuff[qwRet];
			if (SLessFun(fv, vMid))
			{
				qwEnd = qwRet - 1;
			}
			else if (SLessFun(vMid, fv))
			{
				qwBegin = qwRet + 1;
			}
			else
			{
				break;
			}
		}
		return qwRet;
	}
	ValueType*  getBuff()
	{
		return m_pBuff;
	}
private:
	ValueType*		m_pBuff;
	qword			m_nBuffNum;
};

template<class ValueType, class LessFunType=LessComp<ValueType>>
class TGetLine
{
public:
	using binarySearchT =  TBinarySearch<ValueType, LessFunType>;
	TGetLine(ValueType* pointS, qword pointNum):m_binary(pointS, pointNum)
	{
	}
	qword	search(const ValueType& fv)
	{
		auto nR = m_binary.Search (fv);
		auto pBuf = m_binary.getBuff ();
		LessFunType fun;
		return fun (fv, pBuf[nR]) ? nR : nR + 1;
	}
private:
	binarySearchT   m_binary;
};


#endif//__T_Algorithm___h_
