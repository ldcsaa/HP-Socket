/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.3.1
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
Module:  HPSocket for C DLL

Desc: 导出纯 C 函数，让其它语言（如：C / C# / Delphi 等）能方便地使用 HPSocket

Usage:
		方法一：
		--------------------------------------------------------------------------------------
		0. （C/C++ 程序）包含 HPSocket4C.h 头文件
		1. 调用 ::Create_HP_XxxListener() 函数创建监听器对象
		2. 调用 ::Create_HP_Xxx(pListener) 函数创建 HPSocket 对象
		3. 调用 ::HP_Set_FN_Xxx_OnYyy(pListener, ...) 函数设置监听器的回调函数
		4. 调用相关导出函数操作 HPSocket 对象
		5. ...... ......
		6. 调用 ::Destroy_HP_Xxx(pSocket) 函数销毁 HPSocket 对象
		7. 调用 ::Destroy_HP_XxxListener(pListener) 函数销毁监听器对象

		方法二：
		--------------------------------------------------------------------------------------
		1. 应用程序把需要用到的导出函数封装到特定语言的包装类中
		2. 通过包装类封装后，以面向对象的方式使用 HPSocket

Release:
		1. Bin/x86/HPSocket4C.dll		- (32位/MBCS/Release)
		2. Bin/x86/HPSocket4C_D.dll		- (32位/MBCS/DeBug)
		3. Bin/x86/HPSocket4C_U.dll		- (32位/UNICODE/Release)
		4. Bin/x86/HPSocket4C_UD.dll	- (32位/UNICODE/DeBug)
		5. Bin/x64/HPSocket4C.dll		- (64位/MBCS/Release)
		6. Bin/x64/HPSocket4C_D.dll		- (64位/MBCS/DeBug)
		7. Bin/x64/HPSocket4C_U.dll		- (64位/UNICODE/Release)
		8. Bin/x64/HPSocket4C_UD.dll	- (64位/UNICODE/DeBug)

******************************************************************************/
