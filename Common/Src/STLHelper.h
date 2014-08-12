/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 2.3.7
 * Author	: Bruce Liang
 * Website	: http://www.jessma.org
 * Project	: https://github.com/ldcsaa
 * Blog		: http://www.cnblogs.com/ldcsaa
 * Wiki		: http://www.oschina.net/p/hp-socket
 * QQ Group	: 75375912
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#pragma once

#include "GeneralHelper.h"

#include <memory>
#include <string>
#include <functional>
#include <algorithm>
#include <vector>
#include <deque>
#include <queue>
#include <stack>
#include <list>
#include <set>
#include <map>
#include <hash_set>
#include <hash_map>
#include <unordered_set>
#include <unordered_map>

using namespace std;
using namespace stdext;
using namespace std::tr1;

typedef	wstring		CStdStringW;
typedef string		CStdStringA;

#ifdef _UNICODE
	typedef	CStdStringW		CStdString;
#else
	typedef CStdStringA		CStdString;
#endif

typedef list<short>						short_list;
typedef list<int>						int_list;
typedef list<long>						long_list;
typedef list<__int64>					int64_list;
typedef list<unsigned short>			ushort_list;
typedef list<unsigned int>				uint_list;
typedef list<unsigned long>				ulong_list;
typedef list<unsigned __int64>			uint64_list;
typedef list<float>						float_list;
typedef list<double>					double_list;
typedef stack<short>					short_stack;
typedef stack<int>						int_stack;
typedef stack<long>						long_stack;
typedef stack<__int64>					int64_stack;
typedef stack<unsigned short>			ushort_stack;
typedef stack<unsigned int>				uint_stack;
typedef stack<unsigned long>			ulong_stack;
typedef stack<unsigned __int64>			uint64_stack;
typedef stack<float>					float_stack;
typedef stack<double>					double_stack;
typedef queue<short>					short_queue;
typedef queue<int>						int_queue;
typedef queue<long>						long_queue;
typedef queue<__int64>					int64_queue;
typedef queue<unsigned short>			ushort_queue;
typedef queue<unsigned int>				uint_queue;
typedef queue<unsigned long>			ulong_queue;
typedef queue<unsigned __int64>			uint64_queue;
typedef queue<float>					float_queue;
typedef queue<double>					double_queue;
typedef deque<short>					short_deque;
typedef deque<int>						int_deque;
typedef deque<long>						long_deque;
typedef deque<__int64>					int64_deque;
typedef deque<unsigned short>			ushort_deque;
typedef deque<unsigned int>				uint_deque;
typedef deque<unsigned long>			ulong_deque;
typedef deque<unsigned __int64>			uint64_deque;
typedef deque<float>					float_deque;
typedef deque<double>					double_deque;
typedef vector<short>					short_vector;
typedef vector<int>						int_vector;
typedef vector<long>					long_vector;
typedef vector<__int64>					int64_vector;
typedef vector<unsigned short>			ushort_vector;
typedef vector<unsigned int>			uint_vector;
typedef vector<unsigned long>			ulong_vector;
typedef vector<unsigned __int64>		uint64_vector;
typedef vector<float>					float_vector;
typedef vector<double>					double_vector;
typedef set<short>						short_set;
typedef set<int>						int_set;
typedef set<long>						long_set;
typedef set<__int64>					int64_set;
typedef set<unsigned short>				ushort_set;
typedef set<unsigned int>				uint_set;
typedef set<unsigned long>				ulong_set;
typedef set<unsigned __int64>			uint64_set;
typedef set<float>						float_set;
typedef set<double>						double_set;
typedef hash_set<short>					short_hash_set;
typedef hash_set<int>					int_hash_set;
typedef hash_set<long>					long_hash_set;
typedef hash_set<__int64>				int64_hash_set;
typedef hash_set<unsigned short>		ushort_hash_set;
typedef hash_set<unsigned int>			uint_hash_set;
typedef hash_set<unsigned long>			ulong_hash_set;
typedef hash_set<unsigned __int64>		uint64_hash_set;
typedef hash_set<float>					float_hash_set;
typedef hash_set<double>				double_hash_set;
typedef unordered_set<short>			short_unordered_set;
typedef unordered_set<int>				int_unordered_set;
typedef unordered_set<long>				long_unordered_set;
typedef unordered_set<__int64>			int64_unordered_set;
typedef unordered_set<unsigned short>	ushort_unordered_set;
typedef unordered_set<unsigned int>		uint_unordered_set;
typedef unordered_set<unsigned long>	ulong_unordered_set;
typedef unordered_set<unsigned __int64>	uint64_unordered_set;
typedef unordered_set<float>			float_unordered_set;
typedef unordered_set<double>			double_unordered_set;

typedef list<INT_PTR>					int_ptr_list;
typedef list<LONG_PTR>					long_ptr_list;
typedef list<UINT_PTR>					uint_ptr_list;
typedef list<ULONG_PTR>					ulong_ptr_list;
typedef stack<INT_PTR>					int_ptr_stack;
typedef stack<LONG_PTR>					long_ptr_stack;
typedef stack<UINT_PTR>					uint_ptr_stack;
typedef stack<ULONG_PTR>				ulong_ptr_stack;
typedef queue<INT_PTR>					int_ptr_queue;
typedef queue<LONG_PTR>					long_ptr_queue;
typedef queue<UINT_PTR>					uint_ptr_queue;
typedef queue<ULONG_PTR>				ulong_ptr_queue;
typedef deque<INT_PTR>					int_ptr_deque;
typedef deque<LONG_PTR>					long_ptr_deque;
typedef deque<UINT_PTR>					uint_ptr_deque;
typedef deque<ULONG_PTR>				ulong_ptr_deque;
typedef vector<INT_PTR>					int_ptr_vector;
typedef vector<LONG_PTR>				long_ptr_vector;
typedef vector<UINT_PTR>				uint_ptr_vector;
typedef vector<ULONG_PTR>				ulong_ptr_vector;
typedef set<INT_PTR>					int_ptr_set;
typedef set<LONG_PTR>					long_ptr_set;
typedef set<UINT_PTR>					uint_ptr_set;
typedef set<ULONG_PTR>					ulong_ptr_set;
typedef hash_set<INT_PTR>				int_ptr_hash_set;
typedef hash_set<LONG_PTR>				long_ptr_hash_set;
typedef hash_set<UINT_PTR>				uint_ptr_hash_set;
typedef hash_set<ULONG_PTR>				ulong_ptr_hash_set;
typedef unordered_set<INT_PTR>			int_ptr_unordered_set;
typedef unordered_set<LONG_PTR>			long_ptr_unordered_set;
typedef unordered_set<UINT_PTR>			uint_ptr_unordered_set;
typedef unordered_set<ULONG_PTR>		ulong_ptr_unordered_set;

/*****************************************************************************/
/******************************** 容器操作函数 *******************************/

/**********************************
描述: 清除普通集合 , 适用于 vector<Object> / list<Object>
参数: 
	v		: vector / list / set

返回值: 		
**********************************/
template<class Set> void ClearSet(Set& v)
{
	v.clear();
}

template<class Set> struct Set_Cleaner
{
	static void Clear(Set& v) {ClearSet(v);}
};

/**********************************
描述: 清除指针集合 (清除前先释放指针), 适用于 vector<Object*> / list<Object*>
参数: 
		v		: vector / list / set

返回值: 		
**********************************/
template<class PtrSet> void ClearPtrSet(PtrSet& v)
{
	for(PtrSet::iterator	it = v.begin(); 
							it != v.end(); 
							++it)
		delete (*it);

	v.clear();
}

template<class PtrSet> struct PtrSet_Cleaner
{
	static void Clear(PtrSet& v) {ClearPtrSet(v);}
};

/**********************************
描述: 清除指针集合 (指针同时又指向数组), 适用于 vector<Object*[]> / list<Object*[]>
参数: 
		v		: vector / list / set

返回值: 		
**********************************/
template<class PtrArraySet> void ClearPtrArraySet(PtrArraySet& v)
{
	for(PtrArraySet::iterator	it = v.begin(); 
								it != v.end(); 
								++it)
		delete[] (*it);

	v.clear();
}

template<class PtrArraySet> struct PtrArraySet_Cleaner
{
	static void Clear(PtrArraySet& v) {ClearPtrArraySet(v);}
};

/**********************************
描述: 清除普通影射 , 适用于 map<key, value>
参数: 
	v		: map

返回值: 		
**********************************/
template<class Map> void ClearMap(Map& v)
{
	v.clear();
}

template<class Map> struct Map_Cleaner
{
	static void Clear(Map& v) {ClearMap(v);}
};

/**********************************
描述: 清除指针影射 (清除前先释放指针), 适用于 map<key, Object*>
参数: 
		v		: map

返回值: 		
**********************************/
template<class PtrMap> void ClearPtrMap(PtrMap& v)
{
	for(PtrMap::iterator	it = v.begin(); 
							it != v.end(); 
							++it)
		delete it->second;

	v.clear();
}

template<class PtrMap> struct PtrMap_Cleaner
{
	static void Clear(PtrMap& v) {ClearPtrMap(v);}
};

/**********************************
描述: 清除指针影射 (指针同时又指向数组), 适用于 map<key, Object*[]>
参数: 
		v		: map

返回值: 		
**********************************/
template<class PtrArrayMap> void ClearPtrArrayMap(PtrArrayMap& v)
{
	for(PtrArrayMap::iterator	it = v.begin(); 
								it != v.end(); 
								++it)
		delete[] it->second;

	v.clear();
}

template<class PtrArrayMap> struct PtrArrayMap_Cleaner
{
	static void Clear(PtrArrayMap& v) {ClearPtrArrayMap(v);}
};

/**********************************
描述: 清除集合-集合 (清除前先清除内部集合), 适用于 set<vector<Object>*>
参数: 
		v		: vector / list / set

返回值: 		
**********************************/
template<class SetSet> void ClearSetSet(SetSet& v)
{
	for(SetSet::iterator	it = v.begin(); 
							it != v.end(); 
							++it)
	{
		(*it)->clear();
		delete (*it);
	}

	v.clear();
}

template<class SetSet> struct SetSet_Cleaner
{
	static void Clear(SetSet& v) {ClearSetSet(v);}
};

/**********************************
描述: 清除指针集合-集合 (清除前先清除内部指针集合), 适用于 set<vector<Object*>*>
参数: 
		v		: vector / list / set

返回值: 		
**********************************/
template<class PtrSetSet> void ClearPtrSetSet(PtrSetSet& v)
{
	for(PtrSetSet::iterator	it = v.begin(); 
							it != v.end(); 
							++it)
	{
		ClearPtrSet(**it);
		delete (*it);
	}

	v.clear();
}

template<class PtrSetSet> struct PtrSetSet_Cleaner
{
	static void Clear(PtrSetSet& v) {ClearPtrSetSet(v);}
};

/**********************************
描述: 清除指针数组集合影射 (清除前先清除指针数组集合), 适用于 map<vector<Object*[]>*>
参数: 
		v		: vector / list / set

返回值: 		
**********************************/
template<class PtrArraySetSet> void ClearPtrArraySetSet(PtrArraySetSet& v)
{
	for(PtrArraySetSet::iterator	it = v.begin(); 
									it != v.end(); 
									++it)
	{
		ClearPtrArraySet(**it);
		delete (*it);
	}

	v.clear();
}

template<class PtrArraySetSet> struct PtrArraySetSet_Cleaner
{
	static void Clear(PtrArraySetSet& v) {ClearPtrArraySetSet(v);}
};

/**********************************
描述: 清除集合影射 (清除前先清除集合), 适用于 map<key, vector<Object>*>
参数: 
		v		: map

返回值: 		
**********************************/
template<class SetMap> void ClearSetMap(SetMap& v)
{
	for(SetMap::iterator	it = v.begin(); 
							it != v.end(); 
							++it)
	{
		it->second->clear();
		delete it->second;
	}

	v.clear();
}

template<class SetMap> struct SetMap_Cleaner
{
	static void Clear(SetMap& v) {ClearSetMap(v);}
};

/**********************************
描述: 清除指针集合影射 (清除前先清除指针集合), 适用于 map<key, vector<Object*>*>
参数: 
		v		: map

返回值: 		
**********************************/
template<class PtrSetMap> void ClearPtrSetMap(PtrSetMap& v)
{
	for(PtrSetMap::iterator	it = v.begin(); 
							it != v.end(); 
							++it)
	{
		ClearPtrSet(*(it->second));
		delete it->second;
	}

	v.clear();
}

template<class PtrSetMap> struct PtrSetMap_Cleaner
{
	static void Clear(PtrSetMap& v) {ClearPtrSetMap(v);}
};

/**********************************
描述: 清除指针数组集合影射 (清除前先清除指针数组集合), 适用于 map<key, vector<Object*[]>*>
参数: 
		v		: map

返回值: 		
**********************************/
template<class PtrArraySetMap> void ClearPtrArraySetMap(PtrArraySetMap& v)
{
	for(PtrArraySetMap::iterator	it = v.begin(); 
									it != v.end(); 
									++it)
	{
		ClearPtrArraySet(*(it->second));
		delete it->second;
	}

	v.clear();
}

template<class PtrArraySetMap> struct PtrArraySetMap_Cleaner
{
	static void Clear(PtrArraySetMap& v) {ClearPtrArraySetMap(v);}
};

/**********************************
描述: 清除映射-影射 (清除前先清除内部映射), 适用于 map<key, map<key2, Object>*>
参数: 
v		: map

返回值: 		
**********************************/
template<class MapMap> void ClearMapMap(MapMap& v)
{
	for(MapMap::iterator	it = v.begin(); 
							it != v.end(); 
							++it)
	{
		it->second->clear();
		delete it->second;
	}

	v.clear();
}

template<class MapMap> struct MapMap_Cleaner
{
	static void Clear(MapMap& v) {ClearMapMap(v);}
};

/**********************************
描述: 清除指针映射-影射 (清除前先清除指针内部映射), 适用于 map<key, map<key2, Object*>*>
参数: 
		v		: map

返回值: 		
**********************************/
template<class PtrMapMap> void ClearPtrMapMap(PtrMapMap& v)
{
	for(PtrMapMap::iterator	it = v.begin(); 
							it != v.end(); 
							++it)
	{
		ClearPtrMap(*(it->second));
		delete it->second;
	}

	v.clear();
}

template<class PtrMapMap> struct PtrMapMap_Cleaner
{
	static void Clear(PtrMapMap& v) {ClearPtrMapMap(v);}
};

/**********************************
描述: 清除指针映射-影射 (清除前先清除指针数组内部映射), 适用于 map<key, map<key2, Object*[]>*>
参数: 
		v		: map

返回值: 		
**********************************/
template<class PtrArrayMapMap> void ClearPtrArrayMapMap(PtrArrayMapMap& v)
{
	for(PtrArrayMapMap::iterator	it = v.begin(); 
									it != v.end(); 
									++it)
	{
		ClearPtrArrayMap(*(it->second));
		delete it->second;
	}

	v.clear();
}

template<class PtrArrayMapMap> struct PtrArrayMapMap_Cleaner
{
	static void Clear(PtrArrayMapMap& v) {ClearPtrArrayMapMap(v);}
};

/************************************************************************/
/*                           指针集合容器                               */
/************************************************************************/
template<class Set, class Cleaner> struct SetWrapper
{
	typedef typename Set::iterator			iterator;
	typedef typename Set::const_iterator	const_iterator;
	typedef typename Set::value_type		value_type;
	typedef typename Set::reference			reference;
	typedef typename Set::const_reference	const_reference;
	typedef typename Set::pointer			pointer;
	typedef typename Set::const_pointer		const_pointer;
	typedef typename Set::size_type			size_type;
	typedef typename Set::difference_type	difference_type;

	SetWrapper()
	{
	}

	virtual ~SetWrapper()
	{
		Clear();
	}

	void Clear()
	{
		if(!IsEmpty())
		{
			Cleaner::Clear(m_set);
		}
	}

	Set& operator *			()			{return m_set;}
	const Set& operator *	()	const	{return m_set;}
	Set* operator ->		()			{return &m_set;}
	const Set* operator ->	()	const	{return &m_set;}
	Set& Get				()			{return m_set;}
	operator Set&			()			{return m_set;}
	bool IsEmpty			()	const	{return m_set.empty();}
	size_t Size				()	const	{return m_set.size();}

protected:
	Set m_set;

	DECLARE_NO_COPY_CLASS(SetWrapper)
};

template<class Set, class Cleaner> struct VectorWrapper : public SetWrapper<Set, Cleaner>
{
	VectorWrapper()
	{
	}

	reference		operator []	(size_type i)			{return m_set[i];}
	const_reference operator []	(size_type i)	const	{return m_set[i];}

	DECLARE_NO_COPY_CLASS(VectorWrapper)
};

/************************************************************************/
/*                         指针数组集合容器                             */
/************************************************************************/


/************************************************************************/
/*                           指针映射容器                               */
/************************************************************************/
template<class Map, class Cleaner> struct MapWrapper
{
	typedef typename Map::iterator			iterator;
	typedef typename Map::const_iterator	const_iterator;
	typedef typename Map::key_type			key_type;
	typedef typename Map::mapped_type		mapped_type;
	typedef typename Map::value_type		value_type;
	typedef typename Map::reference			reference;
	typedef typename Map::const_reference	const_reference;
	typedef typename Map::pointer			pointer;
	typedef typename Map::size_type			size_type;
	typedef typename Map::difference_type	difference_type;

	MapWrapper()
	{
	}

	~MapWrapper()
	{
		Clear();
	}

	void Clear()
	{
		if(!IsEmpty())
		{
			Cleaner::Clear(m_map);
		}
	}

	Map&				operator *	()								{return m_map;}
	const Map&			operator *	()						const	{return m_map;}
	Map*				operator ->	()								{return &m_map;}
	const Map*			operator ->	()						const	{return &m_map;}
	mapped_type&		operator []	(const key_type& key)			{return m_map[key];}
	const mapped_type&	operator []	(const key_type& key)	const	{return m_map[key];}
	Map& Get			()			{return m_map;}
	operator Map&		()			{return m_map;}
	bool IsEmpty		()	const	{return m_map.empty();}
	size_t Size			()	const	{return m_map.size();}

private:
	Map m_map;

	DECLARE_NO_COPY_CLASS(MapWrapper)
};

/************************************************************************/
/*                            比较仿函数                                */
/************************************************************************/
struct bool_comp_func
{
	bool operator() (bool v1, bool v2) const
	{
		if(!v1)
			return false;
		if(v1 == v2)
			return false;

		return true;
	}
};

template<class T>
// T -> (signed / unsigned) short / int / long / __int64
struct integer_comp_func
{
	bool operator() (T v1, T v2) const
	{
		return v1 < v2;
	}
};

typedef integer_comp_func<short>				short_comp_func;
typedef integer_comp_func<int>					int_comp_func;
typedef integer_comp_func<long>					long_comp_func;
typedef integer_comp_func<__int64>				int64_comp_func;
typedef integer_comp_func<unsigned short>		ushort_comp_func;
typedef integer_comp_func<unsigned int>			uint_comp_func;
typedef integer_comp_func<unsigned long>		ulong_comp_func;
typedef integer_comp_func<unsigned __int64>		uint64_comp_func;

struct float_comp_func
{
	bool operator() (float v1, float v2) const
	{
		float disc	= v1 - v2;
		if(fabsf(disc) < 1E-5)
			return false;

		return disc < 0;
	}
};

struct double_comp_func
{
	bool operator() (double v1, double v2) const
	{
		double disc	= v1 - v2;
		if(fabs(disc) < 1E-8)
			return false;

		return disc < 0;
	}
};

template<class T, bool CASE = false>
// T -> (unsigned) char / wchar_t
struct character_comp_func
{
	bool operator() (T v1, T v2) const
	{
		if(!CASE)
		{
			if(v1 >= 'A' && v1 <= 'Z')	v1 += 32;
			if(v2 >= 'A' && v2 <= 'Z')	v2 += 32;
		}

		return v1 < v2;
	}
};

typedef character_comp_func<char, true>				char_case_comp_func;
typedef character_comp_func<unsigned char, true>	uchar_case_comp_func;
typedef character_comp_func<wchar_t, true>			wchar_case_comp_func;
typedef character_comp_func<char, false>			char_ucase_comp_func;
typedef character_comp_func<unsigned char, false>	uchar_ucase_comp_func;
typedef character_comp_func<wchar_t, false>			wchar_ucase_comp_func;

template<class T, bool CASE = false>
// T -> TCHAR* / CString
struct str_comp_func
{
	//比较函数。
	bool operator() (const T &A, const T &B) const
	{
		if(!CASE)
			return lstrcmpi((LPCTSTR)A, (LPCTSTR)B) < 0;
		else
			return lstrcmp((LPCTSTR)A, (LPCTSTR)B) < 0;
	}
};

typedef str_comp_func<LPCTSTR, true>		case_tchar_comp_func;
typedef str_comp_func<LPCTSTR, false>		uncase_tchar_comp_func;
typedef str_comp_func<CString, true>		case_string_comp_func;
typedef str_comp_func<CString, false>		uncase_string_comp_func;
typedef case_tchar_comp_func				tchar_ptr_case_comp_func;
typedef uncase_tchar_comp_func				tchar_ptr_ucase_comp_func;
typedef case_string_comp_func				string_case_comp_func;
typedef uncase_string_comp_func				string_ucase_comp_func;
/************************************************************************/
/*                            排序仿函数                                */
/************************************************************************/
template<bool ASC = true>
struct bool_sort_func
{
	bool operator() (bool v1, bool v2) const
	{
		if(v1 == v2)
			return false;

		bool result = !v1;
		return ASC ? result : !result;
	}
};

typedef bool_sort_func<true>	bool_asc_sort_func;
typedef bool_sort_func<false>	bool_desc_sort_func;

template<class T, bool ASC = true>
// T -> (signed / unsigned) short / int / long / __int64
struct integer_sort_func
{
	bool operator() (T v1, T v2) const
	{
		if(v1 == v2)
			return false;

		bool result = v1 < v2;
		return ASC ? result : !result;
	}
};

typedef integer_sort_func<short,			true>		short_asc_sort_func;
typedef integer_sort_func<unsigned short,	true>		ushort_asc_sort_func;
typedef integer_sort_func<int,				true>		int_asc_sort_func;
typedef integer_sort_func<unsigned int,		true>		uint_asc_sort_func;
typedef integer_sort_func<long,				true>		long_asc_sort_func;
typedef integer_sort_func<unsigned long,	true>		ulong_asc_sort_func;
typedef integer_sort_func<__int64,			true>		int64_asc_sort_func;
typedef integer_sort_func<unsigned __int64,	true>		uint64_asc_sort_func;
typedef integer_sort_func<short,			false>		short_desc_sort_func;
typedef integer_sort_func<unsigned short,	false>		ushort_desc_sort_func;
typedef integer_sort_func<int,				false>		int_desc_sort_func;
typedef integer_sort_func<unsigned int,		false>		uint_desc_sort_func;
typedef integer_sort_func<long,				false>		long_desc_sort_func;
typedef integer_sort_func<unsigned long,	false>		ulong_desc_sort_func;
typedef integer_sort_func<__int64,			false>		int64_desc_sort_func;
typedef integer_sort_func<unsigned __int64,	false>		uint64_desc_sort_func;

template<bool ASC = true>
struct float_sort_func
{
	bool operator() (float v1, float v2) const
	{
		float disc	= v1 - v2;
		if(fabsf(disc) < 1E-5)
			return false;

		bool result = disc < 0;
		return ASC ? result : !result;
	}
};

typedef float_sort_func<true>		float_asc_sort_func;
typedef float_sort_func<false>		float_desc_sort_func;

template<bool ASC = true>
struct double_sort_func
{
	bool operator() (double v1, double v2) const
	{
		double disc	= v1 - v2;
		if(fabs(disc) < 1E-8)
			return false;

		bool result = disc < 0;
		return ASC ? result : !result;
	}
};

typedef double_sort_func<true>		double_asc_sort_func;
typedef double_sort_func<false>		double_desc_sort_func;

template<class T, bool ASC = true, bool CASE = false>
// T -> (unsigned) char / wchar_t
struct character_sort_func
{
	bool operator() (T v1, T v2) const
	{
		if(!CASE)
		{
			if(v1 >= 'A' && v1 <= 'Z')	v1 += 32;
			if(v2 >= 'A' && v2 <= 'Z')	v2 += 32;
		}

		if(v1 == v2)
			return false;

		bool result = v1 < v2;
		return ASC ? result : !result;
	}
};

typedef character_sort_func<char, true, true>				char_asc_case_sort_func;
typedef character_sort_func<unsigned char, true, true>		uchar_asc_case_sort_func;
typedef character_sort_func<wchar_t, true, true>			wchar_asc_case_sort_func;
typedef character_sort_func<char, true, false>				char_asc_ucase_sort_func;
typedef character_sort_func<unsigned char, true, false>		uchar_asc_ucase_sort_func;
typedef character_sort_func<wchar_t, true, false>			wchar_asc_ucase_sort_func;
typedef character_sort_func<char, false, true>				char_desc_case_sort_func;
typedef character_sort_func<unsigned char, false, true>		uchar_desc_case_sort_func;
typedef character_sort_func<wchar_t, false, true>			wchar_desc_case_sort_func;
typedef character_sort_func<char, false, false>				char_desc_ucase_sort_func;
typedef character_sort_func<unsigned char, false, false>	uchar_desc_ucase_sort_func;
typedef character_sort_func<wchar_t, false, false>			wchar_desc_ucase_sort_func;

template<class T, bool ASC = true, bool CASE = false>
// T -> TCHAR* / CString
struct str_sort_func
{
	bool operator() (const T& v1, const T& v2) const
	{
		bool result;

		if(CASE)
		{
			int v = lstrcmp((LPCTSTR)v1, (LPCTSTR)v2);
			if(v == 0)
				result = false;
			else
				result = v < 0;
		}
		else
		{
			int v = lstrcmpi((LPCTSTR)v1, (LPCTSTR)v2);
			if(v == 0)
				result = false;
			else
				result = v < 0;
		}

		return ASC ? result : !result;
	}
};

typedef str_sort_func<TCHAR*, true, true>		tchar_ptr_asc_case_sort_func;
typedef str_sort_func<CString, true, true>		string_asc_case_sort_func;
typedef str_sort_func<TCHAR*, true, false>		tchar_ptr_asc_ucase_sort_func;
typedef str_sort_func<CString, true, false>		string_asc_ucase_sort_func;
typedef str_sort_func<TCHAR*, false, true>		tchar_ptr_desc_case_sort_func;
typedef str_sort_func<CString, false, true>		string_desc_case_sort_func;
typedef str_sort_func<TCHAR*, false, false>		tchar_ptr_desc_ucase_sort_func;
typedef str_sort_func<CString, false, false>	string_desc_ucase_sort_func;

/************************************************************************/
/*					   smart_ptr 单实体或数组智能指针                    */
/************************************************************************/

template<class _Ty>
struct simple_deleter
{
	static void delete_ptr(_Ty* pv) {delete pv;}
};

template<class _Ty>
struct global_simple_deleter
{
	static void delete_ptr(_Ty* pv) {::delete pv;}
};

template<class _Ty>
struct array_deleter
{
	static void delete_ptr(_Ty* pv) {delete[] pv;}
};

template<class _Ty>
struct global_array_deleter
{
	static void delete_ptr(_Ty* pv) {::delete[] pv;}
};

template<class _Ty, class _Deleter>
class smart_ptr
{
public:
	smart_ptr(_Ty* _Ptr = 0)					: _Myptr(_Ptr)				{}
	smart_ptr(smart_ptr<_Ty, _Deleter>& _Right)	: _Myptr(_Right.release())	{}

	~smart_ptr()
	{
		reset();
	}

	smart_ptr<_Ty, _Deleter>& reset(_Ty* _Ptr = 0)
	{
		if (_Ptr != _Myptr)
		{
			if(_Myptr)
				_Deleter::delete_ptr(_Myptr);

			_Myptr = _Ptr;
		}

		return *this;
	}

	smart_ptr<_Ty, _Deleter>& reset(smart_ptr<_Ty, _Deleter>& _Right)
	{
		if (this != &_Right)
			reset(_Right.release());

		return *this;
	}

	_Ty* release()
	{
		_Ty* _Ptr	= _Myptr;
		_Myptr		= 0;

		return _Ptr;
	}

	smart_ptr<_Ty, _Deleter>& operator = (_Ty* _Ptr)						{return reset(_Ptr);}
	smart_ptr<_Ty, _Deleter>& operator = (smart_ptr<_Ty, _Deleter>& _Right)	{return reset(_Right);}

	bool is_valid		()	const	{return _Myptr != 0;}
	_Ty& operator *		()	const	{return *_Myptr;}
	_Ty* get			()	const	{return _Myptr;}
	_Ty* operator ->	()	const	{return _Myptr;}
	operator _Ty*		()	const	{return _Myptr;}

private:
	template<class _Other> smart_ptr<_Ty, _Deleter>					(const smart_ptr<_Ty, _Other>&);
	template<class _Other> smart_ptr<_Ty, _Deleter>&	reset		(const smart_ptr<_Ty, _Other>&);
	template<class _Other> smart_ptr<_Ty, _Deleter>&	operator =	(const smart_ptr<_Ty, _Other>&);

	template<class _Other> smart_ptr<_Ty, _Deleter>					(const smart_ptr<_Other, _Deleter>&);
	template<class _Other> smart_ptr<_Ty, _Deleter>&	reset		(const smart_ptr<_Other, _Deleter>&);
	template<class _Other> smart_ptr<_Ty, _Deleter>&	operator =	(const smart_ptr<_Other, _Deleter>&);

protected:
	_Ty* _Myptr;
};


/************************************************************************/
/*				    smart_simple_ptr 单实体智能指针                      */
/************************************************************************/

template<class _Ty>
class smart_simple_ptr : public smart_ptr<_Ty, simple_deleter<_Ty>>
{
public:
	smart_simple_ptr(_Ty* _Ptr = 0)									: smart_ptr(_Ptr)	{}
	smart_simple_ptr(smart_simple_ptr<_Ty>& _Right)					: smart_ptr(_Right)	{}
	smart_simple_ptr(smart_ptr<_Ty, simple_deleter<_Ty>>& _Right)	: smart_ptr(_Right)	{}

	smart_simple_ptr<_Ty>& operator = (smart_ptr<_Ty, simple_deleter<_Ty>>& _Right)
	{return (smart_simple_ptr<_Ty>&)__super::operator = (_Right);}

	smart_simple_ptr<_Ty>& operator = (smart_simple_ptr<_Ty>& _Right)
	{return (smart_simple_ptr<_Ty>&)__super::operator = (_Right);}

	smart_simple_ptr<_Ty>& operator = (_Ty* _Ptr)
	{return (smart_simple_ptr<_Ty>&)__super::operator = (_Ptr);}

private:
	template<class _Other> smart_simple_ptr<_Ty>				(const smart_ptr<_Ty, _Other>&);
	template<class _Other> smart_simple_ptr<_Ty>&	operator =	(const smart_ptr<_Ty, _Other>&);

	template<class _Other> smart_simple_ptr<_Ty>				(const smart_simple_ptr<_Other>&);
	template<class _Other> smart_simple_ptr<_Ty>&	operator =	(const smart_simple_ptr<_Other>&);
};

/************************************************************************/
/*		   smart_gd_simple_ptr 单实体智能指针 (使用全局 delete)          */
/************************************************************************/

template<class _Ty>
class smart_gd_simple_ptr : public smart_ptr<_Ty, global_simple_deleter<_Ty>>
{
public:
	smart_gd_simple_ptr(_Ty* _Ptr = 0)										: smart_ptr(_Ptr)	{}
	smart_gd_simple_ptr(smart_gd_simple_ptr<_Ty>& _Right)					: smart_ptr(_Right)	{}
	smart_gd_simple_ptr(smart_ptr<_Ty, global_simple_deleter<_Ty>>& _Right)	: smart_ptr(_Right)	{}

	smart_gd_simple_ptr<_Ty>& operator = (smart_ptr<_Ty, global_simple_deleter<_Ty>>& _Right)
	{return (smart_gd_simple_ptr<_Ty>&)__super::operator = (_Right);}

	smart_gd_simple_ptr<_Ty>& operator = (smart_gd_simple_ptr<_Ty>& _Right)
	{return (smart_gd_simple_ptr<_Ty>&)__super::operator = (_Right);}

	smart_gd_simple_ptr<_Ty>& operator = (_Ty* _Ptr)
	{return (smart_gd_simple_ptr<_Ty>&)__super::operator = (_Ptr);}

private:
	template<class _Other> smart_gd_simple_ptr<_Ty>					(const smart_ptr<_Ty, _Other>&);
	template<class _Other> smart_gd_simple_ptr<_Ty>&	operator =	(const smart_ptr<_Ty, _Other>&);

	template<class _Other> smart_gd_simple_ptr<_Ty>					(const smart_gd_simple_ptr<_Other>&);
	template<class _Other> smart_gd_simple_ptr<_Ty>&	operator =	(const smart_gd_simple_ptr<_Other>&);
};

/************************************************************************/
/*                   smart_array_ptr 数组智能指针                        */
/************************************************************************/

template<class _Ty>
class smart_array_ptr : public smart_ptr<_Ty, array_deleter<_Ty>>
{
public:
	smart_array_ptr(_Ty* _Ptr = 0)								: smart_ptr(_Ptr)	{}
	smart_array_ptr(smart_simple_ptr<_Ty>& _Right)				: smart_ptr(_Right)	{}
	smart_array_ptr(smart_ptr<_Ty, array_deleter<_Ty>>& _Right)	: smart_ptr(_Right)	{}

	smart_array_ptr<_Ty>& operator = (smart_ptr<_Ty, array_deleter<_Ty>>& _Right)
	{return (smart_array_ptr<_Ty>&)__super::operator = (_Right);}

	smart_array_ptr<_Ty>& operator = (smart_array_ptr<_Ty>& _Right)
	{return (smart_array_ptr<_Ty>&)__super::operator = (_Right);}

	smart_array_ptr<_Ty>& operator = (_Ty* _Ptr)
	{return (smart_array_ptr<_Ty>&)__super::operator = (_Ptr);}

private:
	template<class _Other> smart_array_ptr<_Ty>					(const smart_ptr<_Ty, _Other>&);
	template<class _Other> smart_array_ptr<_Ty>&	operator =	(const smart_ptr<_Ty, _Other>&);

	template<class _Other> smart_array_ptr<_Ty>					(const smart_array_ptr<_Other>&);
	template<class _Other> smart_array_ptr<_Ty>&	operator =	(const smart_array_ptr<_Other>&);
};

/************************************************************************/
/*          smart_gd_array_ptr 单实体智能指针 (使用全局 delete)          */
/************************************************************************/

template<class _Ty>
class smart_gd_array_ptr : public smart_ptr<_Ty, global_array_deleter<_Ty>>
{
public:
	smart_gd_array_ptr(_Ty* _Ptr = 0)										: smart_ptr(_Ptr)	{}
	smart_gd_array_ptr(smart_gd_array_ptr<_Ty>& _Right)						: smart_ptr(_Right)	{}
	smart_gd_array_ptr(smart_ptr<_Ty, global_array_deleter<_Ty>>& _Right)	: smart_ptr(_Right)	{}

	smart_gd_array_ptr<_Ty>& operator = (smart_ptr<_Ty, global_array_deleter<_Ty>>& _Right)
	{return (smart_gd_array_ptr<_Ty>&)__super::operator = (_Right);}

	smart_gd_array_ptr<_Ty>& operator = (smart_gd_array_ptr<_Ty>& _Right)
	{return (smart_gd_array_ptr<_Ty>&)__super::operator = (_Right);}

	smart_gd_array_ptr<_Ty>& operator = (_Ty* _Ptr)
	{return (smart_gd_array_ptr<_Ty>&)__super::operator = (_Ptr);}

private:
	template<class _Other> smart_gd_array_ptr<_Ty>				(const smart_ptr<_Ty, _Other>&);
	template<class _Other> smart_gd_array_ptr<_Ty>&	operator =	(const smart_ptr<_Ty, _Other>&);

	template<class _Other> smart_gd_array_ptr<_Ty>				(const smart_gd_array_ptr<_Other>&);
	template<class _Other> smart_gd_array_ptr<_Ty>&	operator =	(const smart_gd_array_ptr<_Other>&);
};
