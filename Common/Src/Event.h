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

class CEvt
{
public:
	CEvt(BOOL bManualReset = FALSE, BOOL bInitialState = FALSE, LPCTSTR pszName = nullptr, LPSECURITY_ATTRIBUTES pSecurity = nullptr)
	{
		m_hEvent = ::CreateEvent(pSecurity, bManualReset, bInitialState, pszName);
		ASSERT(IsValid());
	}

	~CEvt()
	{
		if(IsValid())
			VERIFY(::CloseHandle(m_hEvent));
	}

	BOOL Open(DWORD dwAccess, BOOL bInheritHandle, LPCTSTR pszName)
	{
		if(IsValid())
			VERIFY(::CloseHandle(m_hEvent));

		m_hEvent = ::OpenEvent(dwAccess, bInheritHandle, pszName);
		return(IsValid());
	}

	BOOL Pulse()	{return(::PulseEvent(m_hEvent));}
	BOOL Reset()	{return(::ResetEvent(m_hEvent));}
	BOOL Set()		{return(::SetEvent(m_hEvent));}

	HANDLE& GetHandle	() 	{return m_hEvent;}
	operator HANDLE		()	{return m_hEvent;}
	BOOL IsValid		()	{return m_hEvent != nullptr;}

private:
	CEvt(const CEvt&);
	CEvt operator = (const CEvt&);

private:
	HANDLE m_hEvent;
};

