#ifndef _arrayMap_h_
#define _arrayMap_h_
#include "myAssert.h"
#include "TAlgorithm.h"
#include "comFun.h"
#include <memory>

typedef dword arrayMapIndexType;


template<class T, class CompF = LessComp<T>>
class arraySet
{
public:
    arraySet(arrayMapIndexType MaxNodeNum = 8)
    {
        m_udwMaxTNum = MaxNodeNum;
		m_pTS = std::make_unique<T[]>(MaxNodeNum);
        m_udwTNum = 0;
    }
	arraySet(const T* pA, arrayMapIndexType udwNum)
	{
		m_udwMaxTNum = udwNum;
		m_pTS = std::make_unique<T[]>(udwNum);
		auto pT = m_pTS.get();
		memcpy(pT, pA, udwNum * sizeof(T));
		m_udwTNum = udwNum;
		TQuickSort<T*, T, CompF> qs(pT, m_udwTNum);
		qs.sort();
	}

	int init (const T* pA, arrayMapIndexType udwNum)
	{
		int nRet = 0;
		m_udwMaxTNum = udwNum;
		m_pTS = std::make_unique<T[]>(udwNum);
		auto pT = m_pTS.get();
		memcpy(pT, pA, udwNum * sizeof(T));
		m_udwTNum = udwNum;
		TQuickSort<T*, T, CompF> qs(pT, m_udwTNum);
		qs.sort();
		return nRet;
	}

    ~arraySet()
    {
    }
	void clear()
	{
		m_udwTNum = 0;
	}
    dword RegNode(const T& node)
    {
        if(m_udwTNum >= m_udwMaxTNum)
        {
			auto nNewNum = m_udwMaxTNum <<= 1;
			auto pNewTS = std::make_unique<T[]>(nNewNum);
			auto pT = m_pTS.get();
			auto pNT = pNewTS.get();
			memcpy(pNT, pT, m_udwMaxTNum * sizeof(T));
			m_pTS = std::move(pNewTS);
			m_udwMaxTNum = nNewNum;
        }
		if(0 == m_udwTNum)
		{
			m_pTS[0] = node;
			m_udwTNum = 1;
			return 0;
		}
		auto pT = m_pTS.get();
        TBinarySearch<T, CompF> bs(pT, m_udwTNum);
        uqword uqwRet = bs.Search(node);
        CompF cmp;
		if(cmp(node, pT[uqwRet]))
		{
			memmove(&pT[uqwRet + 1], &pT[uqwRet], sizeof(pT[0]) * (m_udwTNum - (udword)uqwRet));
			pT[uqwRet] = node;
			m_udwTNum++;
		}
		else         
		{
			if(cmp(pT[uqwRet], node))
			{
				if(uqwRet == m_udwTNum - 1)
				{
					pT[m_udwTNum++] = node;
				}
				else
				{
					uqwRet++;
					memmove(&pT[uqwRet + 1], &pT[uqwRet], sizeof(pT[0]) * (m_udwTNum - (udword)uqwRet));
					pT[uqwRet] = node;
					m_udwTNum++;
				}
			}
			else
			{
				return 2;
			}
		}
        return 0;
    }
    bool find(const T& node)
	{
		return GetNode(node) != NULL;
	}
    bool insert(const T& node)
	{
		return 0 == RegNode(node); 
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
    bool erase(const T& node)
    {
        arrayMapIndexType nI = Find(node);
        if(nI < m_udwTNum)
        {
            for(arrayMapIndexType i = nI + 1; i < m_udwTNum; i++)
            {
                m_pTS[i - 1] = m_pTS[i];
            }
            return true;
        }
        return false;
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
class arrayMap
{
public:
    typedef keyValue<K,V> NodeType;
    arrayMap(arrayMapIndexType nMax = 8):m_Set(nMax)
    {
    }
	arrayMap(NodeType* pA, arrayMapIndexType udwNum):m_Set(pA, udwNum)
	{
	}
	int init (NodeType* pA, arrayMapIndexType udwNum)
	{
		return m_Set.init (pA, udwNum);
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
	void clear()
	{
		return m_Set.clear();
	}
    bool insert(const K& k, const V& v)
    {
        NodeType node;
        node.key = k;
        node.value = v;
        return m_Set.RegNode(node) == 0;
    }
    bool erase(const K& k)
    {
        NodeType node;
        node.key = k;
        return m_Set.erase(node);
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
    V& operator [](const K& k)
    {
        V* pV = find(k);
        if(NULL == pV)
        {
            V v;
            bool bI = inser(k,v);
            assert(bI);
            pV = find(k);
            myAssert(pV);
        }
        return *pV;
    }
private:
    class CmpFun
    {
    public:
        bool operator ()(const NodeType& a, const NodeType& b)const
        {
            KCompF cmp;
            return cmp(a.key, b.key);
        }
    };
    typedef arraySet<NodeType, CmpFun>  SetType;
    SetType     m_Set;
};

typedef arraySet<pIStringKey, pIStringKeyCmp> pIStringKeyArraySet;
#endif
