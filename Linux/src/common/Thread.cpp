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

#include "Thread.h"

static __thread BOOL s_tlsInterrupt		= FALSE;
BOOL __CThread_Interrupt_::sm_bInitFlag	= __CThread_Interrupt_::InitSigAction();

__CThread_Interrupt_::~__CThread_Interrupt_	() {s_tlsInterrupt = FALSE;}
BOOL __CThread_Interrupt_::IsInterrupted	() {return s_tlsInterrupt;}

BOOL __CThread_Interrupt_::InitSigAction()
{
	struct sigaction act;
	sigemptyset(&act.sa_mask);

	act.sa_handler	= SignalHandler;
	act.sa_flags	= 0;

	if(IS_HAS_ERROR(sigaction(SIG_NO, &act, nullptr)))
		ERROR_ABORT();

	return TRUE;
}

void __CThread_Interrupt_::SignalHandler(int sig)
{
	if(sig == SIG_NO)
		s_tlsInterrupt = TRUE;
}
