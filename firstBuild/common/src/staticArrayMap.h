#ifndef _staticArrayMap_h_
#define _staticArrayMap_h_
#include "myAssert.h"
#include "TAlgorithm.h"
#include "comFun.h"
#include <memory>

template<class T, class CompF = LessComp<T>>
class staticArraySet
{
public:
	typedef dword arrayMapIndexType;
	staticArraySet(const T* pA, arrayMapIndexType udwNum)
	{
		m_udwMaxTNum = udwNum;
		m_pTS = std::make_unique<T[]>(udwNum);
		auto pT = m_pTS.get();
		memcpy(pT, pA, udwNum * sizeof(T));
		m_udwTNum = udwNum;
		TQuickSort<T*, T, CompF> qs(pT, m_udwTNum);
		qs.sort();
	}
    ~staticArraySet()
    {
    }
    bool find(const T& node)
	{
		return GetNode(node) != NULL;
	}
    
    T* GetNode(const T& node)
    {
		if(0 == m_udwTNum)
		{
			return NULL;
		}
        arrayMapIndexType nI = Find(node);
        if(nI < m_udwTNum)
        {
            return &m_pTS[nI];
        }
        return NULL;
    }
	T* getNodeByIndex(arrayMapIndexType nIndex)
	{
		return nIndex < m_udwTNum ? &m_pTS[nIndex] : NULL;
	}
	arrayMapIndexType  size()
	{
		return m_udwTNum;
	}
private:
    arrayMapIndexType Find(const T& node)
    {
		auto pT = m_pTS.get();
        TBinarySearch<T, CompF> bs(pT, m_udwTNum);
        uqword uqwRet = bs.Search(node);
        CompF cmp;
        if(cmp(pT[uqwRet], node))
        {
            return m_udwTNum;
        }
        if(cmp(node, pT[uqwRet]))
        {
            return  m_udwTNum;
        }
        return (arrayMapIndexType)uqwRet;
    }

	std::unique_ptr<T[]>	m_pTS;
    arrayMapIndexType m_udwTNum;
    arrayMapIndexType m_udwMaxTNum;
};


template<class K, class V, class KCompF = LessComp<K>>
class staticArrayMap
{
public:
    typedef keyValue<K,V> NodeType;
	class CmpFun
    {
    public:
        bool operator ()(const NodeType& a, const NodeType& b)const
        {
            KCompF cmp;
            return cmp(a.key, b.key);
        }
    };
    typedef staticArraySet<NodeType, CmpFun>  SetType;
	using arrayMapIndexType = typename SetType::arrayMapIndexType;
	staticArrayMap(NodeType* pA, arrayMapIndexType udwNum):m_Set(pA, udwNum)
	{
	}
	typedef bool (*visitFun)(void* pU, K& k, V& v);
	void visit(void* pU, visitFun pF)
	{
		arrayMapIndexType  udwS = m_Set.size();
		for(arrayMapIndexType i = 0; i < udwS; i++)
		{
			NodeType* pN = m_Set.getNodeByIndex(i);
			bool bC = pF(pU, pN->key, pN->value);
			if(!bC)
			{
				return;
			}
		}
	}
    V* find(const K& k)
    {
        NodeType node;
        node.key = k;
        NodeType* pF = m_Set.GetNode(node);
        return NULL == pF ? NULL : &pF->value;
    }
	arrayMapIndexType size()
	{
		return m_Set.size();
	}
	V* valueByIndex(arrayMapIndexType nIndex)
	{
		NodeType* pN = m_Set.getNodeByIndex(nIndex);
		return NULL == pN ? NULL:&pN->value;
	}
private:
    
    SetType     m_Set;
};

#endif
