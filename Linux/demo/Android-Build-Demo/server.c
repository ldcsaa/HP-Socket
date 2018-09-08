#include <stdio.h>
#include <unistd.h>
#include "src/HPSocket4C.h"

En_HP_HandleResult OnReceive(HP_Server pSender, HP_CONNID dwConnID, const BYTE* pData, int iLength)
{
	printf("[ConnID:%d, Length: %d] %s\n", dwConnID, iLength, pData);
	return HR_OK;
}

int main(int argc, char* const argv[])
{
	printf("HP-Socket for Android\n");
	HP_TcpPackServerListener listener = Create_HP_TcpPackServerListener();
	HP_TcpPackServer svr = Create_HP_TcpPackServer(listener);
	HP_Set_FN_Server_OnReceive(listener, OnReceive);
	HP_TcpPackServer_SetMaxPackSize(svr, 0x40000);
	HP_TcpPackServer_SetPackHeaderFlag(svr, 0x169);
	if (HP_Server_Start(svr, "0.0.0.0", 5555))
	{
		printf("Server start success, and listen on 0.0.0.0:5555\n");
		while(1)
		{
			sleep(1000);
		}
	} else {
		printf("Server start fault, error code is %d\n", HP_Server_GetLastError(svr));
	}
	return 0;
}
