#include "helper.h"
#include "../../src/common/FileHelper.h"

#include <iostream>
using namespace std;

app_arg g_app_arg;

char app_arg::OPTIONS[] = ":a:p:b:d:j:t:e:i:c:l:s:m:o:z:x:y:n:r:u:k:w:q:hv";

app_arg::app_arg()
{
	// -a
	remote_addr		= IPV4_LOOPBACK_ADDRESS;
	// -p
	port			= DEF_TCP_UDP_PORT;
	// -b
	bind_addr		= "";
	// -d
	local_port		= 0;
	// -j
	reject_addr		= "";
	// -n
	async			= true;
	// -t
	thread_count	= DEFAULT_WORKER_THREAD_COUNT;
	// -e
	test_times		= 100;
	// -i
	test_interval	= 1;
	// -c
	conn_count		= 100;
	// -l
	data_length		= 5000;
	// -s
	send_policy		= SP_PACK;
	// -m
	max_conn		= 10000;
	// -q
	keep_alive		= true;

	// -o
	cast_mode		= CM_MULTICAST;
	// -r
	reuse_addr		= true;
	// -u
	ip_loop			= true;
	// -k
	ttl				= 1;

	// -x
	http_port		= DEF_HTTP_PORT;
	// -y
	https_port		= DEF_HTTPS_PORT;
	// -z
	http_use_cookie	= true;
	// -w
	http_with_listener = true;;
}

app_arg::~app_arg()
{
	PRINTLN("bye ~ bye ~");
}

void app_arg::ParseArgs(int argc, char* const argv[])
{
	int c;
	CString strOptArg;


	while((c = ::getopt(argc, argv, OPTIONS)) != -1)
	{
		strOptArg = optarg;

		if(strOptArg == "-")
			strOptArg.Empty();

		switch(c)
		{
		case 'a': remote_addr		= strOptArg;						break;
		case 'p': port				= (USHORT)atoi(strOptArg);			break;
		case 'b': bind_addr			= strOptArg;						break;
		case 'd': local_port		= (USHORT)atoi(strOptArg);			break;
		case 'j': reject_addr		= strOptArg;						break;
		case 'n': async				= (bool)atoi(strOptArg);			break;
		case 't': thread_count		= (DWORD)atoi(strOptArg);			break;
		case 'e': test_times		= (DWORD)atoi(strOptArg);			break;
		case 'i': test_interval		= (DWORD)atoi(strOptArg);			break;
		case 'c': conn_count		= (DWORD)atoi(strOptArg);			break;
		case 'l': data_length		= (DWORD)atoi(strOptArg);			break;
		case 's': send_policy		= (EnSendPolicy)atoi(strOptArg);	break;
		case 'm': max_conn			= (DWORD)atoi(strOptArg);			break;
		case 'q': keep_alive		= (bool)atoi(strOptArg);			break;
		case 'o': cast_mode			= (EnCastMode)atoi(strOptArg);		break;
		case 'r': reuse_addr		= (bool)atoi(strOptArg);			break;
		case 'u': ip_loop			= (bool)atoi(strOptArg);			break;
		case 'k': ttl				= (int)atoi(strOptArg);				break;
		case 'x': http_port			= (USHORT)atoi(strOptArg);			break;
		case 'y': https_port		= (USHORT)atoi(strOptArg);			break;
		case 'z': http_use_cookie	= (bool)atoi(strOptArg);			break;
		case 'w': http_with_listener= (bool)atoi(strOptArg);			break;
		case 'v':					PrintVersion();	exit(EXIT_CODE_OK);
		case 'h':					PrintUsage();	exit(EXIT_CODE_OK);
		case '?':					ERROR_EXIT2(EXIT_CODE_CONFIG, ERROR_INVALID_PARAMETER);
		case ':':					ERROR_EXIT2(EXIT_CODE_CONFIG, ERROR_INVALID_PARAMETER);
		default:					ERROR_EXIT2(EXIT_CODE_CONFIG, ERROR_OBJECT_NOT_FOUND);
		}
	}
}

void app_arg::PrintUsage()
{
	PRINTLN("-------------------------- Command Line Args -------------------------");
	PRINTLN("-%s: %-20s-%s: %-20s-%s: %-20s", "a", "remote_addr", "b", "bind_addr", "c", "conn_count");
	PRINTLN("-%s: %-20s-%s: %-20s-%s: %-20s", "d", "local_port", "e", "test_times", "h", "(PRINT THIS USAGE)");
	PRINTLN("-%s: %-20s-%s: %-20s-%s: %-20s", "i", "test_interval", "j", "reject_addr", "k", "ttl");
	PRINTLN("-%s: %-20s-%s: %-20s-%s: %-20s", "l", "data_length", "m", "max_conn", "n", "async");
	PRINTLN("-%s: %-20s-%s: %-20s-%s: %-20s", "o", "cast_mode", "p", "port", "q", "keep_alive");
	PRINTLN("-%s: %-20s-%s: %-20s-%s: %-20s", "r", "reuse_addr", "s", "send_policy", "t", "thread_count");
	PRINTLN("-%s: %-20s-%s: %-20s-%s: %-20s", "u", "ip_loop", "v", "(PRINT VERSION)", "w", "http_with_listener");
	PRINTLN("-%s: %-20s-%s: %-20s-%s: %-20s", "x", "http_port", "y", "https_port", "z", "http_use_cookie");
	PRINTLN("-----------------------------------------------------------------------");
}

void app_arg::PrintVersion()
{
	DWORD dwVersion = ::GetHPSocketVersion();
	DWORD dwMagor	= dwVersion >> 24;
	DWORD dwMinor	= (dwVersion >> 16) & 0xFF;
	DWORD dwRevise	= (dwVersion >> 8) & 0xFF;
	DWORD dwBuild	= dwVersion & 0xFF;

	PRINTLN("HP-Socket for Linux v%d.%d.%d [BN:%02d]", dwMagor, dwMinor, dwRevise, dwBuild);
}

void app_arg::ShowPFMTestArgs(BOOL bAgent)
{
	PRINTLN("PFM Test Args:");
	PRINTLN("---------------+---------------");
	PRINTLN("%14s : %s", "remote_addr",		(LPCTSTR)remote_addr);
	PRINTLN("%14s : %u", "port",			port);
	PRINTLN("%14s : %u", "test_times",		test_times);
	PRINTLN("%14s : %u", "test_interval",	test_interval);
	PRINTLN("%14s : %u", "conn_count",		conn_count);
	PRINTLN("%14s : %u", "data_length",		data_length);
	if(bAgent) {
	PRINTLN("%14s : %u", "thread_count",	thread_count);
	PRINTLN("%14s : %u", "max_conn",		max_conn); }
	PRINTLN("---------------+---------------");
}

CCommandParser::CCommandParser(CCommandParser::EnAppType enAppType, CMD_FUNC fnCmds[CT_MAX])
: m_enAppType(enAppType)
{
	m_szCmdNameFuncs[CT_START]	= {"start"		, fnCmds[CT_START]};
	m_szCmdNameFuncs[CT_STOP]	= {"stop"		, fnCmds[CT_STOP]};
	m_szCmdNameFuncs[CT_STATUS]	= {"status"		, fnCmds[CT_STATUS]};
	m_szCmdNameFuncs[CT_SEND]	= {"send"		, fnCmds[CT_SEND]};
	m_szCmdNameFuncs[CT_PAUSE]	= {"pause"		, fnCmds[CT_PAUSE]};
	m_szCmdNameFuncs[CT_KICK]	= {"kick"		, fnCmds[CT_KICK]};
	m_szCmdNameFuncs[CT_KICK_L]	= {"kickl"		, fnCmds[CT_KICK_L]};
	m_szCmdNameFuncs[CT_KICK_S]	= {"kicks"		, fnCmds[CT_KICK_S]};
	m_szCmdNameFuncs[CT_CONNECT]= {"connect"	, fnCmds[CT_CONNECT]};
	m_szCmdNameFuncs[CT_STAT]	= {"stat"		, fnCmds[CT_STAT]};

	Reset();
}

BOOL CCommandParser::Run()
{
	if(m_thWorker.IsRunning())
		return FALSE;

	Reset();

	return m_thWorker.Start(this, &CCommandParser::WorkerProc) && WaitForExit();
}

BOOL CCommandParser::WaitForExit()
{
	return m_thWorker.Join();
}

void CCommandParser::WorkerProc(PVOID pv)
{
	CString strLine;

	PrintUsage();
	printf("> ");

	while(TRUE)
	{
		Reset();

		if(!std::getline(cin, strLine))
		{
			PRINTLN();
			break;
		}

		Parse((LPTSTR)(LPCTSTR)strLine, strLine.GetLength());

		printf("> ");
	}

	/*
	PrintUsage();

	char* lpszLine	= nullptr;
	SIZE_T nSize	= 0;

	printf("> ");

	while(TRUE)
	{
		Reset();

		SSIZE_T nRead = getline(&lpszLine, &nSize, stdin);

		if(nRead == EOF)
		{
			PRINTLN();
			break;
		}

		lpszLine[--nRead] = 0;
		Parse(lpszLine, nRead);

		printf("> ");

		if(nSize >= 4096)
		{
			free(lpszLine);

			lpszLine = nullptr;
			nSize	 = 0;
		}
	}

	free(lpszLine);

	*/
}

void CCommandParser::Parse(LPTSTR lpszLine, SSIZE_T nLength)
{
	LPTSTR lpszArg = lpszLine;
	LPTSTR lpszCmd = ::StrSep2(&lpszArg, " \t");

	if(::IsStrEmpty(lpszCmd))
		return;

	int i = CT_START;

	for(; i < CT_MAX; i++)
	{
		if(strcmp(lpszCmd, m_szCmdNameFuncs[i].name) == 0)
			break;
	}

	EnCmdType type = (EnCmdType)i;

	if(type == CT_MAX)
	{
		if(strcmp(lpszCmd, "?") == 0)
			PrintUsage();
		else if(strcmp(lpszCmd, "args") == 0)
			app_arg::PrintUsage();
		else
		{
			PRINTLN("%s: command not FOUND ...", lpszCmd);
		}

		return;
	}

	if( (m_enAppType == AT_CLIENT &&
			(type == CT_KICK || type == CT_KICK_L || type == CT_KICK_S))
		|| (m_enAppType == AT_SERVER && type == CT_CONNECT)	)
	{
		PRINTLN("%s: command not SUPPORTED ...", lpszCmd);
		return;
	}
	
	if(m_szCmdNameFuncs[type].func == nullptr)
	{
		PRINTLN("%s: command not HANDLED ...", lpszCmd);
		return;
	}

	ParseCmdArgs(type, lpszArg);
}

void CCommandParser::ParseCmdArgs(EnCmdType type, LPTSTR lpszArg)
{
	ASSERT(type >= CT_START && type < CT_MAX);

	LPTSTR lpszParam1 = lpszArg;

	if(m_enAppType != AT_CLIENT || type != CT_SEND)
		lpszParam1 = ::StrSep2(&lpszArg, " \t");

	if(type == CT_START || type == CT_STOP || type == CT_STATUS || type == CT_STAT)
	{
		if(!::IsStrEmpty(lpszParam1))
			goto ERROR_USAGE;
	}
	else if(type == CT_SEND)
	{
		LPTSTR lpszMsg = nullptr;

		if(m_enAppType == AT_CLIENT)
			lpszMsg = lpszParam1;
		else
		{
			LPTSTR lpszConnID = lpszParam1;

			if(::IsStrEmpty(lpszConnID))
				goto ERROR_USAGE;
			if((m_dwConnID = atol(lpszConnID)) <= 0)
				goto ERROR_USAGE;

			lpszMsg = lpszArg;
		}

		while(!::IsStrEmpty(lpszMsg))
		{
			if(lpszMsg[0] != ' ' && lpszMsg[0] != '\t')
				break;

			++lpszMsg;
		}

		if(::IsStrEmpty(lpszMsg))
			goto ERROR_USAGE;

		m_strMessage = lpszMsg;
	}
	else if(type == CT_PAUSE || type == CT_KICK)
	{
		LPTSTR lpszFlag = nullptr;

		if(m_enAppType == AT_CLIENT)
			lpszFlag = lpszParam1;
		else
		{
			LPTSTR lpszConnID = lpszParam1;

			if(::IsStrEmpty(lpszConnID))
				goto ERROR_USAGE;
			if((m_dwConnID = atol(lpszConnID)) <= 0)
				goto ERROR_USAGE;

			lpszFlag = ::StrSep2(&lpszArg, " \t");
		}

		if(::IsStrEmpty(lpszFlag))
			m_bFlag = TRUE;
		else
		{
			if(!::IsStrEmpty(::StrSep2(&lpszArg, " \t")))
				goto ERROR_USAGE;
			if(::IsStrEmpty(lpszFlag) || (lpszFlag[0] != '0' && lpszFlag[0] != '1') || lpszFlag[1] != 0)
				goto ERROR_USAGE;

			m_bFlag = (BOOL)(lpszFlag[0] - '0');
		}
	}
	else if(type == CT_KICK_L || type == CT_KICK_S)
	{
		ASSERT(m_enAppType != AT_CLIENT);

		LPTSTR lpszSeconds = lpszParam1;

		if(::IsStrEmpty(lpszSeconds))
		{
			m_dwSeconds	= 60;
			m_bFlag		= TRUE;
		}
		else
		{
			if(((int)(m_dwSeconds = atoi(lpszSeconds))) <= 0)
				goto ERROR_USAGE;

			LPTSTR lpszFlag = ::StrSep2(&lpszArg, " \t");

			if(::IsStrEmpty(lpszFlag))
				m_bFlag = TRUE;
			else
			{
				if(!::IsStrEmpty(::StrSep2(&lpszArg, " \t")))
					goto ERROR_USAGE;
				if(::IsStrEmpty(lpszFlag) || (lpszFlag[0] != '0' && lpszFlag[0] != '1') || lpszFlag[1] != 0)
					goto ERROR_USAGE;

				m_bFlag = (BOOL)(lpszFlag[0] - '0');
			}
		}
	}
	else if(type == CT_CONNECT)
	{
		ASSERT(m_enAppType == AT_AGENT);

		LPTSTR lpszHost = lpszParam1;

		if(::IsStrEmpty(lpszHost))
		{
			m_strRemoteAddr	= g_app_arg.remote_addr;
			m_usRemotePort	= g_app_arg.port;
		}
		else
		{
			LPTSTR lpszPort = ::StrSep2(&lpszArg, " \t");

			if(::IsStrEmpty(lpszPort) || !::IsStrEmpty(::StrSep2(&lpszArg, " \t")))
				goto ERROR_USAGE;
			if((m_usRemotePort = (USHORT)atoi(lpszPort)) <= 0)
				goto ERROR_USAGE;

			m_strRemoteAddr = lpszHost;
		}
	}

	m_szCmdNameFuncs[type].func(this);

	return;

ERROR_USAGE:
	PRINTLN("'%s' usage: %s", m_szCmdNameFuncs[type].name, (LPCTSTR)GetCmdUsage(type));
}

void CCommandParser::Reset()
{
	m_bFlag			= TRUE;
	m_dwConnID		= 0;
	m_usRemotePort	= 0;
	m_dwSeconds		= 0;
	m_strMessage	= nullptr;
	m_strRemoteAddr	= nullptr;
}

void CCommandParser::PrintUsage()
{
	PRINTLN("------ ACTION -----+------------------------ USAGE ----------------------");

	PrintCmdUsage();

	PRINTLN("%-18s : %6s%s", "Print Usage", "", "?");
	PRINTLN("%-18s : %3s%s", "Print Command Args", "", "args");
	PRINTLN("%-18s : %1s%s", "Quit", "", "Ctrl+D");
	PRINTLN("-------------------+-----------------------------------------------------");
}

void CCommandParser::PrintCmdUsage()
{
	if(m_szCmdNameFuncs[CT_START].func)
	PRINTLN("%-18s : %2s%s", "Start Up", "", (LPCTSTR)GetCmdUsage(CT_START));
	if(m_szCmdNameFuncs[CT_STOP].func)
	PRINTLN("%-18s : %3s%s", "Shut Down", "", (LPCTSTR)GetCmdUsage(CT_STOP));
	if(m_szCmdNameFuncs[CT_STATUS].func)
	PRINTLN("%-18s : %1s%s", "Show Status", "", (LPCTSTR)GetCmdUsage(CT_STATUS));
	if(m_szCmdNameFuncs[CT_CONNECT].func && m_enAppType == AT_AGENT)
	PRINTLN("%-18s : %s%s", "Connect To", "", (LPCTSTR)GetCmdUsage(CT_CONNECT));
	if(m_szCmdNameFuncs[CT_SEND].func)
	PRINTLN("%-18s : %3s%s", "Send Message", "", (LPCTSTR)GetCmdUsage(CT_SEND));
	if(m_szCmdNameFuncs[CT_PAUSE].func)
	PRINTLN("%-18s : %2s%s", "Pause/Unpause Recv", "", (LPCTSTR)GetCmdUsage(CT_PAUSE));
	if(m_enAppType != AT_CLIENT) {
	if(m_szCmdNameFuncs[CT_KICK].func)
	PRINTLN("%-18s : %3s%s", "Kick Connection", "", (LPCTSTR)GetCmdUsage(CT_KICK));
	if(m_szCmdNameFuncs[CT_KICK_L].func)
	PRINTLN("%-18s : %2s%s", "Kick Long Conns", "", (LPCTSTR)GetCmdUsage(CT_KICK_L));
	if(m_szCmdNameFuncs[CT_KICK_S].func)
	PRINTLN("%-18s : %2s%s", "Kick Silence Conns", "", (LPCTSTR)GetCmdUsage(CT_KICK_S)); }
	if(m_enAppType == AT_SERVER) {
	if(m_szCmdNameFuncs[CT_STAT].func)
	PRINTLN("%-18s : %3s%s", "Stat R/S bytes", "", (LPCTSTR)GetCmdUsage(CT_STAT));	}
}

CString CCommandParser::GetCmdUsage(CCommandParser::EnCmdType type)
{
	CString strUsage = m_szCmdNameFuncs[type].name;

	switch(type)
	{
	case CT_SEND:
		if(m_enAppType != AT_CLIENT)
			strUsage += " {ConnID}";
		strUsage += " {Message}";
		break;
	case CT_PAUSE:
		if(m_enAppType != AT_CLIENT)
			strUsage += " {ConnID}";
		strUsage += " [0|1]  # (0 - unpause, 1 - pause)";
		break;
	case CT_KICK: 
		if(m_enAppType != AT_CLIENT)
			strUsage += " {ConnID} [0|1]  # (0 - unforced, 1 - forced)";
		else
			strUsage = nullptr;
		break;
	case CT_KICK_L:
	case CT_KICK_S:
		if(m_enAppType != AT_CLIENT)
			strUsage += " [{Seconds}] [0|1]  # (default: {60, 1})";
		else
			strUsage = nullptr;
		break;
	case CT_CONNECT:
		if(m_enAppType != AT_SERVER)
			strUsage += " [{Host} {Port}]";
		else
			strUsage = nullptr;
		break;
	default	:
		;
	}

	return strUsage;
}

void CCommandParser::PrintStatus(EnServiceState enStatus, LPCTSTR lpszName)
{
	CString strStatus;

	if(m_enAppType == AT_SERVER)
		strStatus += "Server";
	else if(m_enAppType == AT_AGENT)
		strStatus += "Agent";
	else if(m_enAppType == AT_CLIENT)
		strStatus += "Client";
	else
		ASSERT(FALSE);

	if(!::IsStrEmpty(lpszName))
		strStatus.AppendChar(' ').Append(lpszName);

	if(enStatus == SS_STARTING)
		strStatus += " is starting";
	else if(enStatus == SS_STARTED)
		strStatus += " has started";
	else if(enStatus == SS_STOPPING)
		strStatus += " is stopping";
	else if(enStatus == SS_STOPPED)
		strStatus += " has stopped";
	else
		ASSERT(FALSE);

	puts(strStatus);
}

#ifdef _NEED_HTTP

void CHttpCommandParser::ParseCmdArgs(EnCmdType type, LPTSTR lpszArg)
{
	ASSERT(type >= CT_START && type < CT_MAX);

	switch(type)
	{
	case CT_START:
		{
			if(m_enAppType == CCommandParser::AT_SERVER)
			{
				LPTSTR lpszParam1 = ::StrSep2(&lpszArg, " \t");

				if(!::IsStrEmpty(lpszParam1))
					goto ERROR_USAGE;
			}
			else if(!ParseCmdOptions(lpszArg, ":s:"))
				goto ERROR_USAGE;
		}

		break;

	case CT_STOP:
	case CT_STAT:
	case CT_STATUS:
		{
			LPTSTR lpszParam1 = ::StrSep2(&lpszArg, " \t");

			if(!::IsStrEmpty(lpszParam1))
				goto ERROR_USAGE;
		}

		break;

	case CT_SEND:
		{
			if(m_enAppType != AT_CLIENT)
			{
				LPTSTR lpszConnID = ::StrSep2(&lpszArg, " \t");

				if(::IsStrEmpty(lpszConnID))
					goto ERROR_USAGE;
				if((m_dwConnID = atol(lpszConnID)) <= 0)
					goto ERROR_USAGE;
			}

			if(m_enAppType == AT_SERVER)
			{
				if(!ParseCmdOptions(lpszArg, ":s:d:"))
					goto ERROR_USAGE;
			}
			else
			{
				if(!ParseCmdOptions(lpszArg, ":p:x:h:d:f:"))
					goto ERROR_USAGE;

				if(m_strPath.IsEmpty())
					m_strPath = "/";
				if(m_strMethod.IsEmpty())
					m_strMethod = HTTP_METHOD_GET;
			}
		}

		break;

	case CT_KICK:
		{
			LPTSTR lpszConnID = ::StrSep2(&lpszArg, " \t");

			if(::IsStrEmpty(lpszConnID))
				goto ERROR_USAGE;
			if((m_dwConnID = atol(lpszConnID)) <= 0)
				goto ERROR_USAGE;

			if(m_enAppType == AT_SERVER && !ParseCmdOptions(lpszArg, ":s:"))
				goto ERROR_USAGE;
			else if(m_enAppType == AT_AGENT && !::IsStrEmpty(::StrSep2(&lpszArg, " \t")))
				goto ERROR_USAGE;
		}

		break;

	case CT_KICK_L:
	case CT_KICK_S:
		{
			m_dwSeconds	= 60;

			if(::IsStrEmpty(lpszArg))
				break;

			char c = ::TrimLeft(&lpszArg, " \t")[0];

			if(c == 0)
				break;
			else if(c == '-')
			{
				if(m_enAppType != AT_SERVER || !ParseCmdOptions(lpszArg, ":s:"))
					goto ERROR_USAGE;
			}
			else
			{
				LPTSTR lpszSeconds = ::StrSep2(&lpszArg, " \t");

				if(((int)(m_dwSeconds = atoi(lpszSeconds))) <= 0)
					goto ERROR_USAGE;

				if(::IsStrEmpty(lpszArg))
					break;

				char c = ::TrimLeft(&lpszArg, " \t")[0];

				if(c == 0)
					break;
				else if(c == '-')
				{
					if(m_enAppType != AT_SERVER || !ParseCmdOptions(lpszArg, ":s:"))
						goto ERROR_USAGE;
				}
				else
					goto ERROR_USAGE;
			}
		}

		break;

	case CT_PAUSE:
		{
			if(m_enAppType != AT_CLIENT)
			{
				LPTSTR lpszConnID = ::StrSep2(&lpszArg, " \t");

				if(::IsStrEmpty(lpszConnID))
					goto ERROR_USAGE;
				if((m_dwConnID = atol(lpszConnID)) <= 0)
					goto ERROR_USAGE;
			}

			m_bFlag = TRUE;

			if(::IsStrEmpty(lpszArg))
				break;

			char c = ::TrimLeft(&lpszArg, " \t")[0];

			if(c == 0)
				break;
			else if(c == '-')
			{
				if(m_enAppType != AT_SERVER || !ParseCmdOptions(lpszArg, ":s:"))
					goto ERROR_USAGE;
			}
			else
			{
				LPTSTR lpszFlag = ::StrSep2(&lpszArg, " \t");

				if((lpszFlag[0] != '0' && lpszFlag[0] != '1') || lpszFlag[1] != 0)
					goto ERROR_USAGE;

				m_bFlag = (BOOL)(lpszFlag[0] - '0');

				if(::IsStrEmpty(lpszArg))
					break;

				char c = ::TrimLeft(&lpszArg, " \t")[0];

				if(c == 0)
					break;
				else if(c == '-')
				{
					if(m_enAppType != AT_SERVER || !ParseCmdOptions(lpszArg, ":s:"))
						goto ERROR_USAGE;
				}
				else
					goto ERROR_USAGE;
			}
		}

		break;

	case CT_CONNECT:
		{
			if(m_enAppType == AT_AGENT)
			{
				LPTSTR lpszHost = ::StrSep2(&lpszArg, " \t");

				if(::IsStrEmpty(lpszHost))
				{
					m_strRemoteAddr	= g_app_arg.remote_addr;
					m_usRemotePort	= 0;
				}
				else
				{
					LPTSTR lpszPort = ::StrSep2(&lpszArg, " \t");

					if(::IsStrEmpty(lpszPort) || !::IsStrEmpty(::StrSep2(&lpszArg, " \t")))
						goto ERROR_USAGE;
					if((m_usRemotePort = (USHORT)atoi(lpszPort)) <= 0)
						goto ERROR_USAGE;

					m_strRemoteAddr = lpszHost;
				}
			}
			else if(m_enAppType == AT_CLIENT)
			{
				m_bFlag		= FALSE;	
				m_strPath	= ::StrSep2(&lpszArg, " \t");

				if(m_strPath.IsEmpty())
					goto ERROR_USAGE;

				if(strnicmp(m_strPath, STR_HTTPS_SCHEMA, strlen(STR_HTTPS_SCHEMA)) == 0)
					m_bHttps = TRUE;
				else if(strnicmp(m_strPath, STR_HTTP_SCHEMA, strlen(STR_HTTP_SCHEMA)) == 0)
					m_bHttps = FALSE;
				else
					goto ERROR_USAGE;

				if(!ParseCmdOptions(lpszArg, ":x:h:d:f:r:"))
					goto ERROR_USAGE;

				if(m_strMethod.IsEmpty())
					m_strMethod = HTTP_METHOD_GET;
			}
		}

		break;

	default:
		ASSERT(FALSE);
	}

	m_szCmdNameFuncs[type].func(this);

	return;

ERROR_USAGE:
	PRINTLN("'%s' usage: %s", m_szCmdNameFuncs[type].name, (LPCTSTR)GetCmdUsage(type));
}

BOOL CHttpCommandParser::ParseCmdOptions(LPCTSTR lpszArg, LPCTSTR lpszOptions)
{
	if(::IsStrEmpty(lpszArg))
		return TRUE;

	vector<CString> vtItem;

	if(!SplitStr(lpszArg, vtItem, " \t", "\'\""))
		return FALSE;

	int iSize = (int)vtItem.size();

	if(iSize % 2 != 0)
		return FALSE;

	for(int i = 0; i < iSize; i += 2)
	{
		CString& strOpt = vtItem[i];
		CString& strVal = vtItem[i + 1];

		if(strOpt.GetLength() != 2 || strOpt[0] != '-')
			return FALSE;

		TCHAR c = strOpt[1];

		if(c == ':' || ::StrChr(lpszOptions, c) == nullptr)
			return FALSE;

		if(c == 's')
		{
			if(strVal != "0" && strVal != "1")
				return FALSE;

			m_bHttps = (BOOL)(strVal[0] - '0');
		}
		else if(c == 'p')
		{
			m_strPath = strVal;
		}
		else if(c == 'x')
		{
			m_strMethod = strVal;
			m_strMethod.MakeUpper();
		}
		else if(c == 'h')
		{
			int iPos = strVal.Find(':');

			if(iPos <= 0)
				return FALSE;

			m_vtHeaders.push_back(strVal.Left(iPos));
			m_vtHeaders.push_back(strVal.Mid(iPos + 1).Trim());
		}
		else if(c == 'd')
		{
			m_strData = strVal;
		}
		else if(c == 'f')
		{
			m_strFilePath = strVal;
		}
		else if(c == 'r')
		{
			if(strVal != "0" && strVal != "1")
				return FALSE;

			m_bFlag = (BOOL)(strVal[0] - '0');
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}

void CHttpCommandParser::Reset()
{
	__super::Reset();
	m_vtHeaders.clear();

	m_bHttps		= FALSE;
	m_strPath		= nullptr;
	m_strMethod		= nullptr;
	m_strData		= nullptr;
	m_strFilePath	= nullptr;
}

void CHttpCommandParser::PrintCmdUsage()
{
	switch(m_enAppType)
	{
	case AT_SERVER:
		if(m_szCmdNameFuncs[CT_START].func)
		PRINTLN("%-18s : %2s%s", "Start Up", "", (LPCTSTR)GetCmdUsage(CT_START));
		if(m_szCmdNameFuncs[CT_STOP].func)
		PRINTLN("%-18s : %3s%s", "Shut Down", "", (LPCTSTR)GetCmdUsage(CT_STOP));
		if(m_szCmdNameFuncs[CT_STATUS].func)
		PRINTLN("%-18s : %1s%s", "Show Status", "", (LPCTSTR)GetCmdUsage(CT_STATUS));
		if(m_szCmdNameFuncs[CT_SEND].func)
		PRINTLN("%-18s : %3s%s", "Send WS Message", "", (LPCTSTR)GetCmdUsage(CT_SEND));
		if(m_szCmdNameFuncs[CT_PAUSE].func)
		PRINTLN("%-18s : %2s%s", "Pause/Unpause Recv", "", (LPCTSTR)GetCmdUsage(CT_PAUSE));
		if(m_szCmdNameFuncs[CT_KICK].func)
		PRINTLN("%-18s : %3s%s", "Release Connection", "", (LPCTSTR)GetCmdUsage(CT_KICK));
		if(m_szCmdNameFuncs[CT_KICK_L].func)
		PRINTLN("%-18s : %2s%s", "Kick Long Conns", "", (LPCTSTR)GetCmdUsage(CT_KICK_L));
		if(m_szCmdNameFuncs[CT_KICK_S].func)
		PRINTLN("%-18s : %2s%s", "Kick Silence Conns", "", (LPCTSTR)GetCmdUsage(CT_KICK_S));
		if(m_szCmdNameFuncs[CT_STAT].func)
		PRINTLN("%-18s : %3s%s", "Stat R/S bytes", "", (LPCTSTR)GetCmdUsage(CT_STAT));
		break;
	case AT_AGENT:
		if(m_szCmdNameFuncs[CT_START].func)
		PRINTLN("%-18s : %2s%s", "Start Up", "", (LPCTSTR)GetCmdUsage(CT_START));
		if(m_szCmdNameFuncs[CT_STOP].func)
		PRINTLN("%-18s : %3s%s", "Shut Down", "", (LPCTSTR)GetCmdUsage(CT_STOP));
		if(m_szCmdNameFuncs[CT_STATUS].func)
		PRINTLN("%-18s : %1s%s", "Show Status", "", (LPCTSTR)GetCmdUsage(CT_STATUS));
		if(m_szCmdNameFuncs[CT_CONNECT].func)
		PRINTLN("%-18s : %s%s", "Connect To", "", (LPCTSTR)GetCmdUsage(CT_CONNECT));
		if(m_szCmdNameFuncs[CT_SEND].func)
		PRINTLN("%-18s : %3s%s", "Send Message", "", (LPCTSTR)GetCmdUsage(CT_SEND));
		if(m_szCmdNameFuncs[CT_PAUSE].func)
		PRINTLN("%-18s : %2s%s", "Pause/Unpause Recv", "", (LPCTSTR)GetCmdUsage(CT_PAUSE));
		if(m_szCmdNameFuncs[CT_KICK].func)
		PRINTLN("%-18s : %3s%s", "Kick Connection", "", (LPCTSTR)GetCmdUsage(CT_KICK));
		if(m_szCmdNameFuncs[CT_KICK_L].func)
		PRINTLN("%-18s : %2s%s", "Kick Long Conns", "", (LPCTSTR)GetCmdUsage(CT_KICK_L));
		if(m_szCmdNameFuncs[CT_KICK_S].func)
		PRINTLN("%-18s : %2s%s", "Kick Silence Conns", "", (LPCTSTR)GetCmdUsage(CT_KICK_S));
		break;
	case AT_CLIENT:
		if(m_szCmdNameFuncs[CT_START].func)
		PRINTLN("%-18s : %2s%s", "Start Up", "", (LPCTSTR)GetCmdUsage(CT_START));
		if(m_szCmdNameFuncs[CT_STOP].func)
		PRINTLN("%-18s : %3s%s", "Shut Down", "", (LPCTSTR)GetCmdUsage(CT_STOP));
		if(m_szCmdNameFuncs[CT_STATUS].func)
		PRINTLN("%-18s : %1s%s", "Show Status", "", (LPCTSTR)GetCmdUsage(CT_STATUS));
		if(m_szCmdNameFuncs[CT_CONNECT].func)
		PRINTLN("%-18s : %s%s", "Open URL", "", (LPCTSTR)GetCmdUsage(CT_CONNECT));
		if(m_szCmdNameFuncs[CT_SEND].func)
		PRINTLN("%-18s : %3s%s", "Send Message", "", (LPCTSTR)GetCmdUsage(CT_SEND));
		if(m_szCmdNameFuncs[CT_PAUSE].func)
		PRINTLN("%-18s : %2s%s", "Pause/Unpause Recv", "", (LPCTSTR)GetCmdUsage(CT_PAUSE));
		break;
	default:
		ASSERT(FALSE);
	}
}

CString CHttpCommandParser::GetCmdUsage(CCommandParser::EnCmdType type)
{
	CString strUsage = m_szCmdNameFuncs[type].name;

	switch(type)
	{
	case CT_START:
		if(m_enAppType != AT_SERVER)
			strUsage += " [-s 0|1]  # (s-0 - http, s-1 - https)";
		break;
	case CT_SEND:
		if(m_enAppType == AT_SERVER)
			strUsage += " {ConnID} [-s 0|1] -d {WS-Data}  # (WS only, s-0 - ws, s-1 - wss)";
		else
		{
			if(m_enAppType != AT_CLIENT)
				strUsage += " {ConnID}";
			strUsage += " [-x {Method}] [-p {Path}] [-h {Header}]* [-d {Data}] [-f {File}]";
		}
		break;
	case CT_PAUSE:
		if(m_enAppType != AT_CLIENT)
			strUsage += " {ConnID}";
		strUsage += " [0|1]";
		if(m_enAppType == AT_SERVER)
			strUsage += " [-s 0|1]";
		strUsage += "  # (0 - unpause, 1 - pause";
		if(m_enAppType == AT_SERVER)
			strUsage += ", s-0 - http, s-1 - https";
		strUsage += ")";
		break;
	case CT_KICK:
		if(m_enAppType == AT_CLIENT)
			strUsage = nullptr;
		else
		{
			strUsage += " {ConnID}";
			if(m_enAppType == AT_SERVER)
				strUsage += " [-s 0|1]  # (s-0 - http, s-1 - https)";
		}
		break;
	case CT_KICK_L:
	case CT_KICK_S:
		if(m_enAppType == AT_CLIENT)
			strUsage = nullptr;
		else
		{
			strUsage += " [{Seconds}]";
			if(m_enAppType == AT_SERVER)
				strUsage += " [-s 0|1]  # (default: 60s, s-0 - http, s-1 - https)";
			else
				strUsage += "  # (default: 60s)";
		}
		break;
	case CT_CONNECT:
		if(m_enAppType == AT_AGENT)
			strUsage += " [{Host} {Port}]";
		else if(m_enAppType == AT_CLIENT)
			strUsage += " {URL} [-x {Method}] [-h {Header}]* [-d {Data}] [-f {File}] [-r 0|1]  # (r-0 - reuse connection, r-1 - force reconnect)";
		else
			strUsage = nullptr;
		break;
	default	:
		;
	}

	return strUsage;
}

#endif

void server_statistics_info::Reset(BOOL bResetClientCount)
{
	if(bResetClientCount)
		m_lClientCount	= 0L;

	m_llTotalSent		= 0L;
	m_llTotalReceived	= 0L;
}

void server_statistics_info::AddTotalRecv(int iLength)
{
	::InterlockedExchangeAdd(&m_llTotalReceived, iLength);
}

void server_statistics_info::AddTotalSend(int iLength)
{
	::InterlockedExchangeAdd(&m_llTotalSent, iLength);
}

void server_statistics_info::CheckClientCount()
{
	if(m_lClientCount == 0)
	{
		CCriSecLock lock(m_cs);

		if(m_lClientCount == 0)
		{
			Reset(FALSE);
		}
	}

	InterlockedIncrement(&m_lClientCount);
}

void server_statistics_info::CheckStatistics()
{
	if(m_lClientCount > 0)
	{
		CCriSecLock lock(m_cs);

		if(m_lClientCount > 0)
		{
			::InterlockedDecrement(&m_lClientCount);

			if(m_lClientCount == 0)
			{
				::WaitFor(500L);
				::PostServerStatics((LONGLONG)m_llTotalSent, (LONGLONG)m_llTotalReceived);

				printf("> ");
				fflush(stdout);
			}
		}
	}
}

void client_statistics_info::Reset()
{
	m_iConnected		= 0;
	m_dwBeginTickCount	= 0;
	m_dwTimeconsuming	= 0;
	m_llTotalReceived	= 0;
	m_llTotalSent		= 0;
	m_llExpectReceived	= (LONGLONG)g_app_arg.test_times * (LONGLONG)g_app_arg.conn_count * (LONGLONG)g_app_arg.data_length;
}

void client_statistics_info::StartTest()
{
	m_dwBeginTickCount = ::TimeGetTime();
}

void client_statistics_info::AddTotalRecv(int iLength)
{
	::InterlockedExchangeAdd(&m_llTotalReceived, iLength);

	if(m_llTotalReceived == m_llExpectReceived)
	{
		m_dwTimeconsuming = ::GetTimeGap32(m_dwBeginTickCount);
		::PostTimeConsuming(m_dwTimeconsuming);

		printf("> ");
		fflush(stdout);
	}

	ASSERT(m_llTotalReceived <= m_llExpectReceived);
}

void client_statistics_info::AddTotalSend(int iLength)
{
	::InterlockedExchangeAdd(&m_llTotalSent, iLength);
}

void client_statistics_info::TermConnected()
{
	if(m_iConnected >= 0)
		m_iConnected = -0xFFFFFF;
}

void client_statistics_info::AddConnected()
{
	::InterlockedIncrement(&m_iConnected);
}

int client_statistics_info::GetConnected()
{
	return m_iConnected;
}

void client_statistics_info::CheckStatistics(BOOL bCheckSend)
{
	::WaitFor(100L);

	CString strMsg;
	strMsg.Format(	_T("*** Summary: expect - %lld, send - %lld, recv - %lld ***"),
					m_llExpectReceived, m_llTotalSent, m_llTotalReceived);

	::LogMsg(strMsg);

	if(m_llExpectReceived == m_llTotalReceived && (!bCheckSend || m_llTotalSent == m_llExpectReceived))
		strMsg.Format(_T("*** Success: time consuming - %u millisecond ! ***"), m_dwTimeconsuming);
	else
		strMsg.Format(_T("*** Fail: manual terminated ? (or data lost) ***"));

	::LogMsg(strMsg);
}

info_msg* info_msg::Construct(CONNID dwConnID, LPCTSTR lpszEvent, int iContentLength, LPCTSTR lpszContent, LPCTSTR lpszName)
{
	return new info_msg(dwConnID, lpszEvent, iContentLength, lpszContent, lpszName);
}

void info_msg::Destruct(info_msg* pMsg)
{
	delete pMsg;
}

info_msg::info_msg(CONNID dwConnID, LPCTSTR lpszEvent, int iContentLength, LPCTSTR lpszContent, LPCTSTR lpszName)
: connID(dwConnID), evt(lpszEvent), contentLength(iContentLength), content(lpszContent), name(nullptr)
{
	if(lpszName)
	{
		int len = lstrlen(lpszName);

		if(len > 0)
		{
			name = new TCHAR[len + 1];
			memcpy((LPSTR)name, lpszName, (len + 1) * sizeof(TCHAR));
		}
	}
}

info_msg::~info_msg()
{
	if(name)
		delete[] name;

	if(contentLength > 0)
		delete[] content;
}

inline CString SafeString(LPCTSTR lpszName)
{
	CString str(lpszName);

	if(lpszName) str.AppendChar(' ');

	return str;
}

inline CString SafeString2(LPCTSTR lpszName)
{
	CString str(lpszName);

	if(lpszName) str.Append(_T(" #"));

	return str;
}

void LogServerStart(LPCTSTR lpszAddress, USHORT port, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %sServer Start OK --> (%s#%d)"), (LPCTSTR)SafeString(lpszName), lpszAddress, port);
	LogMsg(msg);
}

void LogServerStartFail(DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %sServer Start Fail --> %s (%d) [%d]"), (LPCTSTR)SafeString(lpszName), lpszDesc, code, ::GetLastError());
	LogMsg(msg);
}

void LogServerStop(LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %sServer Stop"), (LPCTSTR)SafeString(lpszName));

	LogMsg(msg);
}

void LogServerStopFail(DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %sServer Stop Fail --> %s (%d)"), (LPCTSTR)SafeString(lpszName), lpszDesc, code);
	LogMsg(msg);
}

void LogAgentStart(LPCTSTR lpszAddress, BOOL bAsync, LPCTSTR lpszName)
{
	if(!lpszAddress || lpszAddress[0] == 0)
		lpszAddress = _T("***");

	char c = bAsync ? 'A' : 'S';

	CString msg;
	msg.Format(_T("# %sAgent Start OK --> (%s - %c)"), (LPCTSTR)SafeString(lpszName), lpszAddress, c);
	LogMsg(msg);
}

void LogAgentStarting(LPCTSTR lpszAddress, BOOL bAsync, LPCTSTR lpszName)
{
	if(!lpszAddress || lpszAddress[0] == 0)
		lpszAddress = _T("***");

	char c = bAsync ? 'A' : 'S';

	CString msg;
	msg.Format(_T("# %sAgent Starting --> (%s - %d)"), (LPCTSTR)SafeString(lpszName), lpszAddress, c);
	LogMsg(msg);
}

void LogAgentStartFail(DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %sAgent Start Fail --> %s (%d) [%d]"), (LPCTSTR)SafeString(lpszName), lpszDesc, code, ::GetLastError());
	LogMsg(msg);
}

void LogAgentStopping(CONNID dwConnID, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %sAgent Stopping --> (%zu)"), (LPCTSTR)SafeString(lpszName), dwConnID);
	LogMsg(msg);
}

void LogAgentStop(LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %sAgent Stop"), (LPCTSTR)SafeString(lpszName));

	LogMsg(msg);
}

void LogAgentStopFail(DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %sAgent Stop Fail --> %s (%d) [%d]"), (LPCTSTR)SafeString(lpszName), lpszDesc, code, ::GetLastError());
	LogMsg(msg);
}

void LogAgentSendFail(int iSequence, int iSocketIndex, DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %sAgent Send Fail [SOCK: %d, SEQ: %d] --> %s (%d)"), (LPCTSTR)SafeString(lpszName), iSocketIndex, iSequence, lpszDesc, code);
	LogMsg(msg);
}

void LogClientStart(LPCTSTR lpszAddress, USHORT port, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %sClient Start OK --> (%s#%d)"), (LPCTSTR)SafeString(lpszName), lpszAddress, port);
	LogMsg(msg);
}

void LogClientStarting(LPCTSTR lpszAddress, USHORT port, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %sClient Starting --> (%s#%d)"), (LPCTSTR)SafeString(lpszName), lpszAddress, port);
	LogMsg(msg);
}

void LogClientStartFail(DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %sClient Start Fail --> %s (%d) [%d]"), (LPCTSTR)SafeString(lpszName), lpszDesc, code, ::GetLastError());
	LogMsg(msg);
}

void LogClientStopping(CONNID dwConnID, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %sClient Stopping --> (%zu)"), (LPCTSTR)SafeString(lpszName), dwConnID);
	LogMsg(msg);
}

void LogClientStop(LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %sClient Stop"), (LPCTSTR)SafeString(lpszName));

	LogMsg(msg);
}

void LogClientStopFail(DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %sClient Stop Fail --> %s (%d) [%d]"), (LPCTSTR)SafeString(lpszName), lpszDesc, code, ::GetLastError());
	LogMsg(msg);
}

void LogClientSendFail(int iSequence, int iSocketIndex, DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %sClient Send Fail [SOCK: %d, SEQ: %d] --> %s (%d)"), (LPCTSTR)SafeString(lpszName), iSocketIndex, iSequence, lpszDesc, code);
	LogMsg(msg);
}

void LogSend(CONNID dwConnID, LPCTSTR lpszContent, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %s%zu Send OK --> %s"), (LPCTSTR)SafeString2(lpszName), dwConnID, lpszContent);
	LogMsg(msg);
}

void LogSending(CONNID dwConnID, LPCTSTR lpszContent, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %s%zu Sending --> %s"), (LPCTSTR)SafeString2(lpszName), dwConnID, lpszContent);
	LogMsg(msg);
}

void LogSendFail(CONNID dwConnID, DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %s%zu Send Fail --> %s (%d)"), (LPCTSTR)SafeString2(lpszName), dwConnID, lpszDesc, code);
	LogMsg(msg);
}

void LogDisconnect(CONNID dwConnID, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %s%zu, Disconnect OK"), (LPCTSTR)SafeString2(lpszName), dwConnID);
	LogMsg(msg);
}

void LogDisconnectFail(CONNID dwConnID, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %s%zu, Disconnect Fail"), (LPCTSTR)SafeString2(lpszName), dwConnID);
	LogMsg(msg);
}

void LogDisconnect2(CONNID dwConnID, BOOL bForce, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %s%zu, Disconnect OK (%c)"), (LPCTSTR)SafeString2(lpszName), dwConnID, bForce ? 'F' : 'N');
	LogMsg(msg);
}

void LogDisconnectFail2(CONNID dwConnID, BOOL bForce, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %s%zu, Disconnect Fail (%c)"), (LPCTSTR)SafeString2(lpszName), dwConnID, bForce ? 'F' : 'N');
	LogMsg(msg);
}

void LogDisconnectLong(DWORD dwSeconds, BOOL bForce, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %sDisconnect OK (%us, %c)"), (LPCTSTR)SafeString(lpszName), dwSeconds, bForce ? 'F' : 'N');
	LogMsg(msg);
}

void LogDisconnectFailLong(DWORD dwSeconds, BOOL bForce, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %sDisconnect Fail (%us, %c)"), (LPCTSTR)SafeString(lpszName), dwSeconds, bForce ? 'F' : 'N');
	LogMsg(msg);
}

void LogPause(CONNID dwConnID, BOOL bPause, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %s%zu, %s OK"), (LPCTSTR)SafeString2(lpszName), dwConnID, bPause ? "Pause" : "Unpause");
	LogMsg(msg);
}

void LogPauseFail(CONNID dwConnID, BOOL bPause, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %s%zu, %s Fail --> %s (%d)"), (LPCTSTR)SafeString2(lpszName), dwConnID, bPause ? "Pause" : "Unpause", ::GetLastErrorStr(), ::GetLastError());
	LogMsg(msg);
}

void LogConnect(LPCTSTR lpszAddress, USHORT usPort, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %sConnecting to %s#%d ..."), (LPCTSTR)SafeString(lpszName), lpszAddress, usPort);
	LogMsg(msg);
}

void LogConnectFail(DWORD code, LPCTSTR lpszDesc, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %sConnect Fail --> %s (%d)"), (LPCTSTR)SafeString(lpszName), lpszDesc, code);
	LogMsg(msg);
}

void LogRelease(CONNID dwConnID, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %s%zu, Release OK"), (LPCTSTR)SafeString2(lpszName), dwConnID);
	LogMsg(msg);
}

void LogReleaseFail(CONNID dwConnID, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %s%zu, Release Fail"), (LPCTSTR)SafeString2(lpszName), dwConnID);
	LogMsg(msg);
}

void LogDetect(CONNID dwConnID, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %s%zu, Detect Connection OK"), (LPCTSTR)SafeString2(lpszName), dwConnID);
	LogMsg(msg);
}

void LogDetectFail(CONNID dwConnID, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("# %s%zu, Detect Connection Fail"), (LPCTSTR)SafeString2(lpszName), dwConnID);
	LogMsg(msg);
}

void LogOnConnect(CONNID dwConnID, const CString& strAddress, USHORT usPort, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("local address: %s#%d"), (LPCTSTR)strAddress, usPort);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_CONNECT, content_len, lpszContent, lpszName);

	LogInfoMsg(msg);
}

void LogOnConnect2(CONNID dwConnID, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("  > [ %s%zu, %s ]"), (LPCTSTR)SafeString2(lpszName), dwConnID, EVT_ON_CONNECT);
	LogMsg(msg);
}

void LogOnConnect3(CONNID dwConnID, const CString& strAddress, USHORT usPort, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("remote address: %s#%d"), (LPCTSTR)strAddress, usPort);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_CONNECT, content_len, lpszContent, lpszName);

	LogInfoMsg(msg);
}

void LogOnHandShake2(CONNID dwConnID, LPCTSTR lpszName)
{
	CString msg;
	msg.Format(_T("  > [ %s%zu, %s ]"), (LPCTSTR)SafeString2(lpszName), dwConnID, EVT_ON_HAND_SHAKE);
	LogMsg(msg);
}

void LogOnClose(CONNID dwConnID, LPCTSTR lpszName)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_CLOSE, 0, nullptr, lpszName);

	LogInfoMsg(msg);
}

void PostOnSend(CONNID dwConnID, const BYTE* pData, int iLength, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[20];
	wsprintf(lpszContent, _T("(%d bytes)"), iLength);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_SEND, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnReceive(CONNID dwConnID, const BYTE* pData, int iLength, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[20];
	wsprintf(lpszContent, _T("(%d bytes)"), iLength);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_RECEIVE, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnReceiveCast(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, const BYTE* pData, int iLength, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("<%s#%d> (%d bytes)"), lpszAddress, usPort, iLength);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_RECEIVE, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnClose(CONNID dwConnID, LPCTSTR lpszName)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_CLOSE, 0, nullptr, lpszName);

	PostInfoMsg(msg);
}

void PostOnError(CONNID dwConnID, int enOperation, int iErrorCode, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("OP: %d, CODE: %d"), enOperation, iErrorCode);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_ERROR, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnAccept(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, BOOL bPass, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("%s (%s#%d)"), bPass ? _T("PASS") : _T("REJECT"), lpszAddress, usPort);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_ACCEPT, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnAccept2(CONNID dwConnID, LPCTSTR lpszName)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_ACCEPT, 0, nullptr, lpszName);

	PostInfoMsg(msg);
}

void PostOnHandShake(CONNID dwConnID, LPCTSTR lpszName)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_HAND_SHAKE, 0, nullptr, lpszName);

	PostInfoMsg(msg);
}

void PostOnPrepareListen(LPCTSTR lpszAddress, USHORT usPort, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("bind address: %s#%d"), lpszAddress, usPort);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(0, EVT_ON_PREPARE_LISTEN, content_len, lpszContent, lpszName);

	LogInfoMsg(msg);
}

void PostOnPrepareConnect(CONNID dwConnID, LPCTSTR lpszName)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_PREPARE_CONNECT, 0, nullptr, lpszName);

	LogInfoMsg(msg);
}

void PostOnConnect(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("local address: %s#%d"), lpszAddress, usPort);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_CONNECT, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnConnect2(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("remote address: %s#%d"), lpszAddress, usPort);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_CONNECT, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnConnect3(CONNID dwConnID, LPCTSTR lpszName)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_CONNECT, 0, nullptr, lpszName);

	PostInfoMsg(msg);
}

void PostOnShutdown(LPCTSTR lpszName)
{
	info_msg* msg = info_msg::Construct(0, EVT_ON_SHUTDOWN, 0, nullptr, lpszName);

	PostInfoMsg(msg);
}

void PostServerStatics(const LONGLONG& llTotalSent, const LONGLONG& llTotalReceived, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("*** Summary: send - %lld, recv - %lld ***"), llTotalSent, llTotalReceived);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(0, EVT_ON_END_TEST, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostServerTemporaryStatics(const LONGLONG& llTotalSent, const LONGLONG& llTotalReceived, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("*** Summary: send - %lld, recv - %lld ***"), llTotalSent, llTotalReceived);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(0, EVT_ON_STAT_TEST, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostTimeConsuming(DWORD dwTickCount, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("Total Time Consuming: %u"), dwTickCount);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(0, EVT_ON_END_TEST, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

#ifdef _NEED_HTTP

void PostOnMessageBegin(CONNID dwConnID, LPCTSTR lpszName)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_MESSAGE_BEGIN, 0, nullptr, lpszName);

	PostInfoMsg(msg);
}

void PostOnRequestLine(CONNID dwConnID, LPCSTR lpszMethod, USHORT usUrlFieldSet, LPCSTR lpszUrl, LPCTSTR lpszName)
{
	USES_CONVERSION;

	int content_len		= (int)(strlen(lpszMethod) + strlen(lpszUrl) + 20);
	LPTSTR lpszContent	= new TCHAR[content_len];

	wsprintf(lpszContent, _T("[%s/0x%02X] : %s"), A2T(lpszMethod), usUrlFieldSet, A2T(lpszUrl));
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_REQUEST_LINE, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnStatusLine(CONNID dwConnID, USHORT usStatusCode, LPCSTR lpszDesc, LPCTSTR lpszName)
{
	USES_CONVERSION;

	int content_len		= (int)(strlen(lpszDesc) + 10);
	LPTSTR lpszContent	= new TCHAR[content_len];

	wsprintf(lpszContent, _T("(%u) : %s"), usStatusCode, A2T(lpszDesc));
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_STATUS_LINE, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnHeader(CONNID dwConnID, LPCSTR lpszHeaderName, LPCSTR lpszHeaderValue, LPCTSTR lpszName)
{
	USES_CONVERSION;

	int content_len		= (int)(strlen(lpszHeaderName) + strlen(lpszHeaderValue) + 10);
	LPTSTR lpszContent	= new TCHAR[content_len];

	wsprintf(lpszContent, _T("%s: %s"), A2T(lpszHeaderName), A2T(lpszHeaderValue));
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_HEADER, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnHeadersComplete(CONNID dwConnID, LPCSTR lpszSummary, LPCTSTR lpszName)
{
	USES_CONVERSION;

	static LPCTSTR PREFIX = _T("* * * * * * * * * Summary * * * * * * * * *\r\n");
	static int PREFIX_LEN = lstrlen(PREFIX);

	LPCTSTR lpszSummaryT = A2CT(lpszSummary);


	int content_len		= lstrlen(lpszSummaryT) + PREFIX_LEN + 1;
	LPTSTR lpszContent	= new TCHAR[content_len];

	memcpy(lpszContent, PREFIX, PREFIX_LEN * sizeof(TCHAR));
	memcpy(lpszContent + PREFIX_LEN, lpszSummaryT, (content_len - PREFIX_LEN) * sizeof(TCHAR));

	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_HEADERS_COMPLETE, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnBody(CONNID dwConnID, const BYTE* pData, int iLength, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[20];
	wsprintf(lpszContent, _T("(%d bytes)"), iLength);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_BODY, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnChunkHeader(CONNID dwConnID, int iLength, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[20];
	wsprintf(lpszContent, _T("(%d bytes)"), iLength);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_CHUNK_HEADER, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnChunkComplete(CONNID dwConnID, LPCTSTR lpszName)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_CHUNK_COMPLETE, 0, nullptr, lpszName);

	PostInfoMsg(msg);
}

void PostOnMessageComplete(CONNID dwConnID, LPCTSTR lpszName)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_MESSAGE_COMPLETE, 0, nullptr, lpszName);

	PostInfoMsg(msg);
}

void PostOnUpgrade(CONNID dwConnID, EnHttpUpgradeType enUpgradeType, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[20];
	wsprintf(lpszContent, _T("(type: %d)"), enUpgradeType);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_UPGRADE, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnParseError(CONNID dwConnID, int iErrorCode, LPCSTR lpszErrorDesc, LPCTSTR lpszName)
{
	USES_CONVERSION;

	int content_len		= (int)(strlen(lpszErrorDesc) + 10);
	LPTSTR lpszContent	= new TCHAR[content_len];

	wsprintf(lpszContent, _T("(%i) : %s"), iErrorCode, A2T(lpszErrorDesc));
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_PARSE_ERROR, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnWSMessageHeader(CONNID dwConnID, BOOL bFinal, BYTE iReserved, BYTE iOperationCode, const BYTE lpszMask[4], ULONGLONG ullBodyLen, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[100];
	wsprintf(lpszContent, _T("(fin: %d, rsv: 0x%x, oc: 0x%x, mask: %d, len: %lld)"), bFinal, iReserved, iOperationCode, lpszMask ? 1 : 0, ullBodyLen);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_WS_MSG_HEADER, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnWSMessageBody(CONNID dwConnID, const BYTE* pData, int iLength, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[20];
	wsprintf(lpszContent, _T("(%d bytes)"), iLength);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_WS_MSG_BODY, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostOnWSMessageComplete(CONNID dwConnID, LPCTSTR lpszName)
{
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_WS_MSG_COMPLETE, 0, nullptr, lpszName);

	PostInfoMsg(msg);
}

void PostUncompressBody(CONNID dwConnID, int iLength, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[40];
	wsprintf(lpszContent, _T("(%d bytes)"), iLength);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_UNCOMPRESS_BODY, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

void PostUncompressBodyFail(CONNID dwConnID, int iResult, LPCTSTR lpszName)
{
	LPTSTR lpszContent = new TCHAR[40];
	wsprintf(lpszContent, _T("(rs: %d)"), iResult);
	int content_len = lstrlen(lpszContent);
	info_msg* msg = info_msg::Construct(dwConnID, EVT_ON_UNCOMPRESS_BODY_FAIL, content_len, lpszContent, lpszName);

	PostInfoMsg(msg);
}

#endif

void PostInfoMsg(info_msg* msg)
{
	LogInfoMsg(msg);
}

void LogInfoMsg(info_msg* pInfoMsg)
{
	CString msg;

	if(pInfoMsg->name)
	{
		if(pInfoMsg->connID > 0)
		{
			if(pInfoMsg->contentLength > 0)
				msg.Format(_T("[ %s #%zu, %s ] -> %s"), pInfoMsg->name, pInfoMsg->connID, pInfoMsg->evt, pInfoMsg->content);
			else
				msg.Format(_T("[ %s #%zu, %s ]"), pInfoMsg->name, pInfoMsg->connID, pInfoMsg->evt);
		}
		else
		{
			if(pInfoMsg->contentLength > 0)
				msg.Format(_T("[ %s - %s ] -> %s"), pInfoMsg->name, pInfoMsg->evt, pInfoMsg->content);
			else
				msg.Format(_T("[ %s - %s ]"), pInfoMsg->name, pInfoMsg->evt);
		}
	}
	else
	{
		if(pInfoMsg->connID > 0)
		{
			if(pInfoMsg->contentLength > 0)
				msg.Format(_T("[ %zu, %s ] -> %s"), pInfoMsg->connID, pInfoMsg->evt, pInfoMsg->content);
			else
				msg.Format(_T("[ %zu, %s ]"), pInfoMsg->connID, pInfoMsg->evt);
		}
		else
		{
			if(pInfoMsg->contentLength > 0)
				msg.Format(_T("[ %s ] -> %s"), pInfoMsg->evt, pInfoMsg->content);
			else
				msg.Format(_T("[ %s ]"), pInfoMsg->evt);
		}
	}

	LogMsg(msg);

	info_msg::Destruct(pInfoMsg);
}

void LogMsg(const CString& msg)
{
	puts(msg);
}

BOOL SplitStr(LPCTSTR lpszSrc, vector<CString>& vtItem, LPCTSTR pszSepectors, LPCTSTR pszQMarks)
{
	vtItem.clear();

	CString strQMarks		= pszQMarks;
	CString strSepectors	= pszSepectors;

	if(strSepectors.IsEmpty())
		strSepectors = _T(" ");

	if(!strQMarks.IsEmpty())
		if(strQMarks.FindOneOf(strSepectors) != -1)
			return FALSE;

	BOOL bRetVal	= TRUE;
	CString strSrc	= lpszSrc;

	while(!strSrc.Trim(strSepectors).IsEmpty())
	{
		CString strItem;

		int iSrcLen	= strSrc.GetLength();
		int iPos1	= strSrc.FindOneOf(strSepectors);
		int iPos2	= !strQMarks.IsEmpty() ? strSrc.FindOneOf(strQMarks) : -1;
		int iPos3	= -1;

		if(iPos1 == -1 && iPos2 == -1)
			strItem = strSrc;
		else if(iPos1 != -1 && (iPos1 < iPos2 || iPos2 == -1))
			strItem = strSrc.Left(iPos1);
		else	// (iPos1 > iPos2 || iPos1 == -1)
		{
			TCHAR tc	= strSrc[iPos2];
			iPos3		= strSrc.Find(tc, iPos2 + 1);
			if(iPos3 != -1)
				strItem = strSrc.Mid(iPos2 + 1, iPos3 - iPos2 - 1);
			else
			{
				vtItem.clear();
				bRetVal = FALSE;
				break;
			}
		}

		vtItem.push_back(strItem);

		strSrc = strSrc.Right(iPos3 == -1 ? (iSrcLen - (iPos1 == -1 ? strItem.GetLength() : iPos1 + 1)) : (iSrcLen - iPos3 - 1));
	}

	return bRetVal;
}

sa_family_t GuessAddrFamily(LPCTSTR lpszAddress)
{
	if (!lpszAddress || lpszAddress[0] == 0)
		return AF_UNSPEC;

	if(::StrChr(lpszAddress, ':'))
		return AF_INET6;

	TCHAR c;
	int arr[4];

	if(stscanf(lpszAddress, _T("%d.%d.%d.%d%c"), &arr[0], &arr[1], &arr[2], &arr[3], &c) != 4)
		return AF_UNSPEC;

	for(int i = 0; i < 4; i++)
	{
		if(arr[i] < 0 || arr[i] > 255)
			return AF_UNSPEC;
	}

	return AF_INET;
}

CBufferPtr* GeneratePkgBuffer(DWORD seq, LPCTSTR lpszName, short age, LPCTSTR lpszDesc)
{
	USES_CONVERSION;

	LPCSTR name	 = T2A((LPTSTR)lpszName);
	LPCSTR desc	 = T2A((LPTSTR)lpszDesc);
	int desc_len = (int)strlen(desc) + 1;
	int body_len = (int)offsetof(TPkgBody, desc) + desc_len;

	TPkgBody* pBody = (TPkgBody*)alloca(body_len);
	memset(pBody, 0, body_len);

	pBody->age = age;
	strcpy(pBody->name, name);
	strcpy(pBody->desc, desc);

	TPkgHeader header;
	header.seq = seq;
	header.body_len = body_len;

	return GeneratePkgBuffer(header, *pBody);
}

CBufferPtr* GeneratePkgBuffer(const TPkgHeader& header, const TPkgBody& body)
{
	int header_len	= sizeof(TPkgHeader);
	int body_len	= header.body_len;

	CBufferPtr* pBuffer = new CBufferPtr(header_len + body_len);

	memcpy(pBuffer->Ptr(), (BYTE*)&header, header_len);
	memcpy(pBuffer->Ptr() + header_len, (BYTE*)&body, body_len);

	return pBuffer;
}

LPCTSTR GetLoopbackAddress(LPCTSTR lpszLikeAddress)
{
	sa_family_t f = GuessAddrFamily(lpszLikeAddress);

	if(f == AF_INET)
		return IPV4_LOOPBACK_ADDRESS;
	if(f == AF_INET6)
		return IPV6_LOOPBACK_ADDRESS;

	return nullptr;
}

LPCTSTR GetAnyAddress(LPCTSTR lpszLikeAddress)
{
	sa_family_t f = GuessAddrFamily(lpszLikeAddress);

	if(f == AF_INET)
		return IPV4_ANY_ADDRESS;
	if(f == AF_INET6)
		return IPV6_ANY_ADDRESS;

	return nullptr;
}

LPCTSTR g_lpszDefaultCookieFile = GetDefaultCookieFile();

LPCTSTR GetDefaultCookieFile()
{
	static TCHAR c_szCookieFile[MAX_PATH] = {0};

	if(c_szCookieFile[0] == 0)
	{
		CString strName = ::GetModuleFileName();
		ASSERT(!strName.IsEmpty());

		strName.Append(".cki");
		lstrcpy(c_szCookieFile, strName);
	}

	return c_szCookieFile;
}

#ifdef _NEED_SSL

#include "../../src/common/FuncHelper.h"

#define SSL_CERT_RELATIVE_PATH_1		_T("/hp-ssl-cert/")
#define SSL_CERT_RELATIVE_PATH_2		_T("/../../ssl-cert/")

CString g_c_strCAPemCertFileOrPath;
CString g_c_strPemCertFile;
CString g_c_strPemKeyFile;

CString g_s_strCAPemCertFileOrPath;
CString g_s_strPemCertFile;
CString g_s_strPemKeyFile;

int g_c_iVerifyMode					= SSL_VM_PEER;
LPCTSTR g_c_lpszCAPemCertFileOrPath	= _T("ca.crt");
LPCTSTR g_c_lpszPemCertFile			= _T("client.cer");
LPCTSTR g_c_lpszPemKeyFile			= _T("client.key");
LPCTSTR g_c_lpszKeyPasswod			= _T("123456");

int g_s_iVerifyMode					= SSL_VM_PEER | SSL_VM_FAIL_IF_NO_PEER_CERT;
LPCTSTR g_s_lpszCAPemCertFileOrPath	= _T("ca.crt");
LPCTSTR g_s_lpszPemCertFile			= _T("server.cer");
LPCTSTR g_s_lpszPemKeyFile			= _T("server.key");
LPCTSTR g_s_lpszKeyPasswod			= _T("123456");

CString g_c_strCAPemCertFileOrPath2;
CString g_c_strPemCertFile2;
CString g_c_strPemKeyFile2;

CString g_s_strCAPemCertFileOrPath2;
CString g_s_strPemCertFile2;
CString g_s_strPemKeyFile2;

int g_c_iVerifyMode2					= SSL_VM_PEER;
LPCTSTR g_c_lpszCAPemCertFileOrPath2	= _T("ca2.crt");
LPCTSTR g_c_lpszPemCertFile2			= _T("client2.cer");
LPCTSTR g_c_lpszPemKeyFile2				= _T("client2.key");
LPCTSTR g_c_lpszKeyPasswod2				= _T("ppmm");

int g_s_iVerifyMode2					= SSL_VM_PEER | SSL_VM_FAIL_IF_NO_PEER_CERT;
LPCTSTR g_s_lpszCAPemCertFileOrPath2	= _T("ca2.crt");
LPCTSTR g_s_lpszPemCertFile2			= _T("server2.cer");
LPCTSTR g_s_lpszPemKeyFile2				= _T("server2.key");
LPCTSTR g_s_lpszKeyPasswod2				= _T("ppmm");

BOOL InitSSLParams();
BOOL g_SSLParams = InitSSLParams();

BOOL InitSSLParams()
{
	::SetCurrentPathToModulePath();

	CString strCur	= ::GetCurrentDirectory();
	CString strPath	= strCur + SSL_CERT_RELATIVE_PATH_1;

	if(!CFile::IsDirectory(strPath))
		strPath = strCur + SSL_CERT_RELATIVE_PATH_2;

	if(g_c_lpszPemCertFile)
	{
		g_c_strPemCertFile	= strPath + g_c_lpszPemCertFile;
		g_c_lpszPemCertFile	= g_c_strPemCertFile;
	}

	if(g_c_lpszPemKeyFile)
	{
		g_c_strPemKeyFile	= strPath + g_c_lpszPemKeyFile;
		g_c_lpszPemKeyFile	= g_c_strPemKeyFile;
	}

	if(g_c_lpszCAPemCertFileOrPath)
	{
		g_c_strCAPemCertFileOrPath	= strPath + g_c_lpszCAPemCertFileOrPath;
		g_c_lpszCAPemCertFileOrPath	= g_c_strCAPemCertFileOrPath;
	}

	if(g_s_lpszPemCertFile)
	{
		g_s_strPemCertFile	= strPath + g_s_lpszPemCertFile;
		g_s_lpszPemCertFile	= g_s_strPemCertFile;
	}

	if(g_s_lpszPemKeyFile)
	{
		g_s_strPemKeyFile	= strPath + g_s_lpszPemKeyFile;
		g_s_lpszPemKeyFile	= g_s_strPemKeyFile;
	}

	if(g_s_lpszCAPemCertFileOrPath)
	{
		g_s_strCAPemCertFileOrPath	= strPath + g_s_lpszCAPemCertFileOrPath;
		g_s_lpszCAPemCertFileOrPath	= g_s_strCAPemCertFileOrPath;
	}

	if(g_c_lpszPemCertFile2)
	{
		g_c_strPemCertFile2  = strPath + g_c_lpszPemCertFile2;
		g_c_lpszPemCertFile2 = g_c_strPemCertFile2;
	}

	if(g_c_lpszPemKeyFile2)
	{
		g_c_strPemKeyFile2	= strPath + g_c_lpszPemKeyFile2;
		g_c_lpszPemKeyFile2	= g_c_strPemKeyFile2;
	}

	if(g_c_lpszCAPemCertFileOrPath2)
	{
		g_c_strCAPemCertFileOrPath2  = strPath + g_c_lpszCAPemCertFileOrPath2;
		g_c_lpszCAPemCertFileOrPath2 = g_c_strCAPemCertFileOrPath2;
	}

	if(g_s_lpszPemCertFile2)
	{
		g_s_strPemCertFile2  = strPath + g_s_lpszPemCertFile2;
		g_s_lpszPemCertFile2 = g_s_strPemCertFile2;
	}

	if(g_s_lpszPemKeyFile2)
	{
		g_s_strPemKeyFile2	= strPath + g_s_lpszPemKeyFile2;
		g_s_lpszPemKeyFile2	= g_s_strPemKeyFile2;
	}

	if(g_s_lpszCAPemCertFileOrPath2)
	{
		g_s_strCAPemCertFileOrPath2  = strPath + g_s_lpszCAPemCertFileOrPath2;
		g_s_lpszCAPemCertFileOrPath2 = g_s_strCAPemCertFileOrPath2;
	}

	return TRUE;
}

#endif

#ifdef _NEED_HTTP

LPCSTR HTTP_WEB_SOCKET_CLOSE_FLAG		= "$close";
const BYTE HTTP_WEB_SOCKET_MASK_KEY[]	= {0x1, 0x2, 0x3, 0x4};

CStringA& HttpVersionToString(EnHttpVersion enVersion, CStringA& strResult)
{
	strResult.Format("HTTP/%d.%d", LOBYTE(enVersion), HIBYTE(enVersion));
	return strResult;
}

CStringA& MakeSecWebSocketAccept(LPCSTR lpszKey, CStringA& strAccept)
{
	CStringA strKey(lpszKey);
	strKey.Append(HTTP_WEB_SOCKET_SEC_SALT);

	_SHA1_CTX ctx;
	BYTE buf[SHA1_BLOCK_SIZE];

	::sha1_init(&ctx);
	::sha1_update(&ctx, (BYTE*)(LPCSTR)strKey, strKey.GetLength());
	::sha1_final(&ctx, buf);

	BYTE* lpszAccept = (BYTE*)strAccept.GetBuffer(SHA1_BLOCK_SIZE * 4 / 3 + 4);

	int len = (int)::base64_encode(buf, lpszAccept, SHA1_BLOCK_SIZE, FALSE);
	strAccept.ReleaseBufferSetLength(len);

	return strAccept;
}

#endif
