Modifications
--------------------
1. kcp.c ignore warning: "-Wconversion", "-Wsign-conversion"
2. kcp.c ikcp_input(): "if (conv != kcp->conv) return -1;" -> "conv = kcp->conv;"