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
 
/******************************************************************************

Module:  SE.h
Notices: Copyright (c) 2006 Bruce Liang
Purpose: 把结构化异常转换成 C++ 异常.
Desc:
		 1. 用 C++ 方式处理结构化异常

Usage:
		 1. 项目 -> 配置属性 -> C/C++ -> 代码生成 -> 启用 C++ 异常选择"否"
		 2. 项目 -> 配置属性 -> C/C++ -> 命令行 -> 加入 "/EHac"
		 3. 在每个线程的入口处调用: MapSEx2CppEx()
		 4. 在要处理结构化异常的地方使用 try{...} catch(CSE se){...}

Rule:	

Example:	1. 见: TestSE 测试程序
			2. 下面的代码片断
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
	// 结构化异常转换成 C++ 异常.
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