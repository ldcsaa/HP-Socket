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

#define SINGLETON_THIS(ClassName)		ClassName::GetThis()
#define SINGLETON_INSTANCE(ClassName)	ClassName::GetInstance()
#define SINGLETON_OBJECT(ObjName)		SINGLETON_INSTANCE(C##ObjName)

#define DEFINE_SINGLETON(ClassName)											\
	ClassName* ClassName::m_pThis = nullptr;

#define DEFINE_P_THIS(ClassName)											\
		DEFINE_SINGLETON(ClassName)

#define DECLARE_SINGLETON_INTERFACE(ClassName)								\
public:																		\
	static ClassName* GetThis()		{return m_pThis;}						\
	static ClassName& GetInstance() {return *m_pThis;}						\
protected:																	\
	static ClassName* m_pThis;

#define DECLARE_SINGLETON_CREATE_INSTANCE(ClassName)						\
public:																		\
	static BOOL CreateInstance()											\
	{																		\
		if(!m_pThis)														\
			m_pThis = new ClassName;										\
																			\
		return m_pThis != nullptr;											\
	}																		\
																			\
	static BOOL DeleteInstance()											\
	{																		\
		if(m_pThis)															\
		{																	\
			delete m_pThis;													\
			m_pThis = nullptr;												\
		}																	\
																			\
		return m_pThis == nullptr;											\
	}

#define DECLARE_PRIVATE_DEFAULT_CONSTRUCTOR(ClassName)						\
private:																	\
	ClassName(){}

#define DECLARE_PRIVATE_COPY_CONSTRUCTOR(ClassName)							\
private:																	\
	ClassName(const ClassName&);											\
	ClassName& operator = (const ClassName&);

#define DECLARE_NO_COPY_CLASS(className)									\
		DECLARE_PRIVATE_COPY_CONSTRUCTOR(className)


#define DECLARE_SINGLETON_IMPLEMENT_NO_CREATE_INSTANCE(ClassName)			\
	DECLARE_SINGLETON_INTERFACE(ClassName)									\
	DECLARE_PRIVATE_DEFAULT_CONSTRUCTOR(ClassName)							\
	DECLARE_PRIVATE_COPY_CONSTRUCTOR(ClassName)								

#define DECLARE_SINGLETON_IMPLEMENT_NO_DEFAULT_CONSTRUCTOR(ClassName)		\
	DECLARE_SINGLETON_CREATE_INSTANCE(ClassName)							\
	DECLARE_PRIVATE_COPY_CONSTRUCTOR(ClassName)

#define DECLARE_SINGLETON_IMPLEMENT(ClassName)								\
	DECLARE_SINGLETON_IMPLEMENT_NO_DEFAULT_CONSTRUCTOR(ClassName)			\
	DECLARE_PRIVATE_DEFAULT_CONSTRUCTOR(ClassName)

#define DECLARE_SINGLETON_NO_DEFAULT_CONSTRUCTOR(ClassName)					\
	DECLARE_SINGLETON_INTERFACE(ClassName)									\
	DECLARE_SINGLETON_IMPLEMENT_NO_DEFAULT_CONSTRUCTOR(ClassName)

#define DECLARE_SINGLETON(ClassName)										\
	DECLARE_SINGLETON_NO_DEFAULT_CONSTRUCTOR(ClassName)						\
	DECLARE_PRIVATE_DEFAULT_CONSTRUCTOR(ClassName)


template<class T>
class CSingleObject
{
public:
	CSingleObject	()	{T::CreateInstance();}
	~CSingleObject	()	{T::DeleteInstance();}
	T* GetPointer	()	{return T::GetThis();}
	T& GetObject	()	{return T::GetInstance();}
	BOOL IsValid	()	{return GetPointer() != nullptr;}
};

#define DECLARE_SINGLE_OBJECT(ClassName) CSingleObject<ClassName> _##ClassName##_Single_Object_;