Modifications
--------------------
1. kcp.c ignore warning: 4018,4245,4267,4456
2. kcp.c ikcp_input(): "if (conv != kcp->conv) return -1;" -> "conv = kcp->conv;"