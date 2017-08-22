/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 2.3.20
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
 
#pragma once

/* timeGetTime() ��װ���� */
DWORD TimeGetTime();

/**********************************
����: ��ȡ��ǰʱ����ԭʼʱ���ʱ���
����: 
		dwOriginal	: ԭʼʱ�䣨���룩��ͨ���� timeGetTime() �� GetTickCount() ��ȡ

����ֵ:	�뵱ǰ timeGetTime() ֮���ʱ���
**********************************/
DWORD GetTimeGap32(DWORD dwOriginal);

#if _WIN32_WINNT >= _WIN32_WINNT_WS08
/**********************************
����: ��ȡ��ǰʱ����ԭʼʱ���ʱ���
����: 
		ullOriginal	: ԭʼʱ�䣨���룩��ͨ���� GetTickCount64() ��ȡ

����ֵ:	�뵱ǰ GetTickCount64() ֮���ʱ���
**********************************/
ULONGLONG GetTimeGap64(ULONGLONG ullOriginal);
#endif

/**********************************
����: ����Windows��Ϣ
����: 
			bDispatchQuitMsg	: �Ƿ�ת�� WM_QUIT ��Ϣ
									TRUE : ת����Ĭ�ϣ�
									FALSE: ��ת���������� FALSE

����ֵ:		TRUE  : ������Ϣ
			FALSE : bDispatchQuitMsg ����Ϊ FALSE ���յ� WM_QUIT ��Ϣ		
**********************************/
BOOL PeekMessageLoop(BOOL bDispatchQuitMsg = TRUE);

/**********************************
����: �ȴ�ָ��ʱ��, ͬʱ����Windows��Ϣ
����: (�ο�: MsgWaitForMultipleObjectsEx() )
		dwHandles		: ����Ԫ�ظ���
		szHandles		: ����������
		dwMilliseconds	: �ȴ�ʱ�� (����)
		dwWakeMask		: ��Ϣ���˱�ʶ
		dwFlags			: �ȴ�����

����ֵ: (0 ~ dwHandles - 1): �ȴ��ɹ�
		WAIT_TIMEOUT		: ��ʱ
		WAIT_FAILED			: ִ��ʧ��
**********************************/
DWORD WaitForMultipleObjectsWithMessageLoop(DWORD dwHandles, HANDLE szHandles[], DWORD dwMilliseconds = INFINITE, DWORD dwWakeMask = QS_ALLINPUT, DWORD dwFlags = MWMO_INPUTAVAILABLE);

/**********************************
����: �ȴ�ָ��ʱ��, ͬʱ����Windows��Ϣ
����: (�ο�: MsgWaitForMultipleObjectsEx() )
		hHandle			: ������
		dwMilliseconds	: �ȴ�ʱ�� (����)
		dwWakeMask		: ��Ϣ���˱�ʶ
		dwFlags			: �ȴ�����

����ֵ: TRUE: �ȴ��ɹ���FALSE: ��ʱ		
**********************************/
BOOL MsgWaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds = INFINITE, DWORD dwWakeMask = QS_ALLINPUT, DWORD dwFlags = MWMO_INPUTAVAILABLE);

/**********************************
����: �ȴ�ָ��ʱ��, ͬʱ����Windows��Ϣ
����: (�ο�: MsgWaitForMultipleObjectsEx() )
		dwMilliseconds	: �ȴ�ʱ�� (����)
		dwWakeMask		: ��Ϣ���˱�ʶ
		dwFlags			: �ȴ�����

����ֵ: MsgWaitForMultipleObjectsEx() �����Ĳ������		
**********************************/
void WaitWithMessageLoop(DWORD dwMilliseconds, DWORD dwWakeMask = QS_ALLINPUT, DWORD dwFlags = MWMO_INPUTAVAILABLE);

/**********************************
����: �ȴ���Sleep()�����ȴ�ĳ������С��ָ��ֵ
����: 
		plWorkingItemCount		: ���ӱ���
		lMaxWorkingItemCount	: ָ��ֵ
		dwCheckInterval			: ����� (����)

����ֵ: 		
**********************************/
void WaitForWorkingQueue(long* plWorkingItemCount, long lMaxWorkingItemCount, DWORD dwCheckInterval);
/**********************************
����: �ȴ���Sleep()�����ȴ�ĳ��������С�� 0
����: 
		plWorkingItemCount		: ���ӱ���
		dwCheckInterval			: ����� (����)

����ֵ: 		
**********************************/
void WaitForComplete	(long* plWorkingItemCount, DWORD dwCheckInterval);

/**********************************
����: �ȴ���WaitWithMessageLoop()�����ȴ�ĳ������С��ָ��ֵ
����: 
		plWorkingItemCount		: ���ӱ���
		lMaxWorkingItemCount	: ָ��ֵ
		dwCheckInterval			: ����� (����)

����ֵ: 		
**********************************/
void MsgWaitForWorkingQueue	(long* plWorkingItemCount, long lMaxWorkingItemCount, DWORD dwCheckInterval = 10);
/**********************************
����: �ȴ���WaitWithMessageLoop()�����ȴ�ĳ��������С�� 0
����: 
		plWorkingItemCount		: ���ӱ���
		dwCheckInterval			: ����� (����)

����ֵ: 		
**********************************/
void MsgWaitForComplete		(long* plWorkingItemCount, DWORD dwCheckInterval = 10);
