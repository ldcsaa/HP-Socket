/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Author	: Bruce Liang
 * Website	: http://www.jessma.org
 * Project	: https://github.com/ldcsaa
 * Blog		: http://www.cnblogs.com/ldcsaa
 * Wiki		: http://www.oschina.net/p/hp-socket
 * QQ Group	: 75375912, 44636872
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
 
/******************************************************************************

Module:  SE.h
Notices: Copyright (c) 2006 Bruce Liang
Purpose: �ѽṹ���쳣ת���� C++ �쳣.
Desc:
		 1. �� C++ ��ʽ����ṹ���쳣

Usage:
		 1. ��Ŀ -> �������� -> C/C++ -> �������� -> ���� C++ �쳣ѡ��"��"
		 2. ��Ŀ -> �������� -> C/C++ -> ������ -> ���� "/EHac"
		 3. ��ÿ���̵߳���ڴ�����: MapSEx2CppEx()
		 4. ��Ҫ����ṹ���쳣�ĵط�ʹ�� try{...} catch(CSE se){...}

Rule:	

Example:	1. ��: TestSE ���Գ���
			2. ����Ĵ���Ƭ��
			void ThreadFunc(PVOID pv)
			{
				// Must be called before any exceptions are raised
				MapSEx2CppEx();

				try
				{
					*(PBYTE)0 = 0;		// Accesss violation

					int x = 0;
					x = 5 / x;			// Division by zero
				}
				catch(CSE& se)
				{
					switch(se)
					{
					case EXCEPTION_ACCESS_VIOLATION:
						// This code handles an access-violation exception
						break;
					case EXCEPTION_INT_DIVIDE_BY_ZERO:
						// This code handles a division-by-zero exception
						break;
					default:
						// We don't handle any other exceptions;
						throw;
					}
				}
			}
											
******************************************************************************/

#pragma once

#include <eh.h>

#define MapSEx2CppEx()	CSE::MapSE2CE()

class CSE
{
public:
	// �ṹ���쳣ת���� C++ �쳣.
	static void MapSE2CE() {_set_se_translator(TranslateSE2CE);}

	operator DWORD() {return m_er.ExceptionCode;}

	CSE(PEXCEPTION_POINTERS pep)
		: m_er(*pep->ExceptionRecord)
		, m_context(*pep->ContextRecord)
	{
	}

	static void _cdecl TranslateSE2CE(UINT ec, PEXCEPTION_POINTERS pep) {throw CSE(pep);}

private:
	EXCEPTION_RECORD	m_er;			// CPU independent exception informaiton
	CONTEXT				m_context;		// CPU dependent exception informaiton
};