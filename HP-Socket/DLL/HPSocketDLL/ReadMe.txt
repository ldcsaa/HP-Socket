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
Module:  HPSocket DLL

Usage:
		方法一：
		--------------------------------------------------------------------------------------
		0. 应用程序包含 SocketInterface.h 和 HPSocket.h 头文件
		1. 调用 HP_Create_Xxx() 函数创建 HPSocket 对象
		2. 使用完毕后调用 HP_Create_Xxx() 函数销毁 HPSocket 对象

		方法二：
		--------------------------------------------------------------------------------------
		0. 应用程序包含 SocketInterface.h 和 HPSocket.h 头文件
		1. 创建 CXxxWrapper 包装器，通过包装器智能指针使用 HPSocket 对象

Release:
		1. Bin/x86/HPSocket.dll		- (32位/MBCS/Release)
		2. Bin/x86/HPSocket_D.dll	- (32位/MBCS/DeBug)
		3. Bin/x86/HPSocket_U.dll	- (32位/UNICODE/Release)
		4. Bin/x6/HPSocket_UD.dll	- (32位/UNICODE/DeBug)
		5. Bin/x64/HPSocket.dll		- (64位/MBCS/Release)
		6. Bin/x64/HPSocket_D.dll	- (64位/MBCS/DeBug)
		7. Bin/x64/HPSocket_U.dll	- (64位/UNICODE/Release)
		8. Bin/x64/HPSocket_UD.dll	- (64位/UNICODE/DeBug)

******************************************************************************/
