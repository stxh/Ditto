#if !defined(AFX_CARRAYEX_H__BE2C5983_CE26_11D3_BAE6_0000C0D475E2__INCLUDED_)
#define AFX_CARRAYEX_H__BE2C5983_CE26_11D3_BAE6_0000C0D475E2__INCLUDED_

#pragma once

#include <vector>
#include <algorithm>
#include <functional>

template <class TYPE> class CArrayEx
{
public:
    CArrayEx() = default;

    inline const TYPE& operator[](INT_PTR nIndex) const
    {
        ASSERT(0 <= nIndex && nIndex < GetSize());
        return m_data[static_cast<size_t>(nIndex)];
    }

    inline TYPE& operator[](INT_PTR nIndex)
    {
        ASSERT(0 <= nIndex && nIndex < GetSize());
        return m_data[static_cast<size_t>(nIndex)];
    }

    inline const TYPE& GetAt(INT_PTR nIndex) const
    {
        return (*this)[nIndex];
    }

    inline TYPE& ElementAt(INT_PTR nIndex)
    {
        return (*this)[nIndex];
    }

    inline const TYPE& ElementAt(INT_PTR nIndex) const
    {
        return (*this)[nIndex];
    }

    inline TYPE* GetData()
    {
        return m_data.empty() ? nullptr : m_data.data();
    }

    inline const TYPE* GetData() const
    {
        return m_data.empty() ? nullptr : m_data.data();
    }

    inline INT_PTR GetSize() const
    {
        return static_cast<INT_PTR>(m_data.size());
    }

    inline INT_PTR GetCount() const
    {
        return GetSize();
    }

    inline INT_PTR GetUpperBound() const
    {
        return GetSize() - 1;
    }

    inline BOOL IsEmpty() const
    {
        return m_data.empty() ? TRUE : FALSE;
    }

    inline void SetSize(INT_PTR nNewSize)
    {
        ASSERT(nNewSize >= 0);
        m_data.resize(static_cast<size_t>(nNewSize));
    }

    inline INT_PTR Add(const TYPE& element)
    {
        m_data.push_back(element);
        return GetUpperBound();
    }

    inline void RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1)
    {
        ASSERT(nCount >= 0);
        ASSERT(0 <= nIndex && nIndex + nCount <= GetSize());
        m_data.erase(m_data.begin() + static_cast<size_t>(nIndex),
                     m_data.begin() + static_cast<size_t>(nIndex + nCount));
    }

    inline void RemoveAll()
    {
        m_data.clear();
    }

    inline void SetAt(INT_PTR nIndex, const TYPE& element)
    {
        ASSERT(0 <= nIndex && nIndex < GetSize());
        m_data[static_cast<size_t>(nIndex)] = element;
    }

    inline void SetAtGrow(INT_PTR nIndex, const TYPE& element)
    {
        if (nIndex >= GetSize())
        {
            SetSize(nIndex + 1);
        }
        SetAt(nIndex, element);
    }

    inline void InsertAt(INT_PTR nIndex, const TYPE& element)
    {
        ASSERT(0 <= nIndex && nIndex <= GetSize());
        m_data.insert(m_data.begin() + static_cast<size_t>(nIndex), element);
    }

    inline INT_PTR Append(const CArrayEx& src)
    {
        m_data.insert(m_data.end(), src.m_data.begin(), src.m_data.end());
        return GetSize();
    }

    inline void Copy(const CArrayEx& src)
    {
        m_data = src.m_data;
    }

    inline void SortAscending()
    {
        std::sort(m_data.begin(), m_data.end(), std::less<TYPE>());
    }

    inline void SortDescending()
    {
        std::sort(m_data.begin(), m_data.end(), std::greater<TYPE>());
    }

    BOOL Find(const TYPE& value)
    {
        return std::find(m_data.begin(), m_data.end(), value) != m_data.end();
    }

private:
    std::vector<TYPE> m_data;
};

typedef CArrayEx<int> ARRAY;

#endif // !defined(AFX_OCARRAY_H__BE2C5983_CE26_11D3_BAE6_0000C0D475E2__INCLUDED_)
