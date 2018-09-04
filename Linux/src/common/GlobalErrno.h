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

#pragma once

#include "GlobalDef.h"

#include <errno.h>
#include <sysexits.h>

#define ERROR_INVALID_STATE				EPERM
#define ERROR_INVALID_PARAMETER			EINVAL
#define	ERROR_BROKEN_PIPE				EPIPE
#define	ERROR_AGAIN						EAGAIN
#define	ERROR_WOULDBLOCK				EAGAIN
#define ERROR_READ_FAULT				EFAULT
#define ERROR_WRITE_FAULT				EFAULT
#define ERROR_HANDLES_CLOSED			EBADFD
#define ERROR_IO_PENDING				EINPROGRESS
#define ERROR_INTR						EINTR
#define ERROR_EMPTY						ENODATA
#define ERROR_FILE_TOO_LARGE			EFBIG
#define ERROR_INVALID_OPERATION			EPERM
#define ERROR_CANCELLED					ECANCELED
#define ERROR_UNKNOWN					ENOMSG
#define ERROR_OBJECT_NOT_FOUND			EBADSLT
#define ERROR_INVALID_INDEX				ENOANO
#define ERROR_OPERATION_ABORTED			ECANCELED
#define ERROR_CONNABORTED				ECONNABORTED
#define ERROR_ADDRNOTAVAIL				EADDRNOTAVAIL
#define ERROR_PFNOSUPPORT				EPFNOSUPPORT
#define ERROR_AFNOSUPPORT				EAFNOSUPPORT
#define ERROR_TIMEDOUT					ETIMEDOUT
#define	ERROR_PROTO						EPROTO
#define ERROR_CONNECTION_COUNT_LIMIT	ENOSR
#define ERROR_VERIFY_CHECK				EBADRQC
#define ERROR_CREATE_FAILED				EMFILE
#define ERROR_INVALID_DATA				EBADMSG
#define ERROR_BAD_LENGTH				EMSGSIZE
#define ERROR_CALL_NOT_IMPLEMENTED		EPERM
#define ERROR_INCORRECT_SIZE			EMSGSIZE
#define ERROR_CONNRESET					ECONNRESET
#define ERROR_CONNREFUSED				ECONNREFUSED
#define ERROR_HOSTUNREACH				EHOSTUNREACH
#define ERROR_INVALID_NAME				ENOENT
#define ERROR_BAD_FILE_TYPE				EBADF
#define ERROR_FILE_NOT_FOUND			ENOENT
#define ERROR_FUNCTION_FAILED			EFAULT
#define ERROR_INVALID_PASSWORD			EACCES
#define ERROR_INVALID_ACCESS			EACCES
#define ERROR_NOT_READY					EPERM
#define ERROR_BAD_FORMAT				EBADMSG
#define ERROR_BUFFER_OVERFLOW			E2BIG
#define ERROR_DESTINATION_ELEMENT_FULL	EXFULL

#define EXIT_CODE_OK					EX_OK
#define EXIT_CODE_CONFIG				EX_CONFIG
#define EXIT_CODE_SOFTWARE				EX_SOFTWARE

/*
* Socket error codes.
*/
#ifndef WSABASEERR

/*
* All Sockets error constants are biased by WSABASEERR from
* the "normal"
*/
#define WSABASEERR					10000

/*
* Sockets definitions of regular Microsoft C error constants
*/
#define WSAEINTR					(WSABASEERR+4)
#define WSAEBADF					(WSABASEERR+9)
#define WSAEACCES					(WSABASEERR+13)
#define WSAEFAULT					(WSABASEERR+14)
#define WSAEINVAL					(WSABASEERR+22)
#define WSAEMFILE					(WSABASEERR+24)

/*
 * Sockets definitions of regular Berkeley error constants
 */
#define WSAEWOULDBLOCK				(WSABASEERR+35)
#define WSAEINPROGRESS				(WSABASEERR+36)
#define WSAEALREADY					(WSABASEERR+37)
#define WSAENOTSOCK					(WSABASEERR+38)
#define WSAEDESTADDRREQ				(WSABASEERR+39)
#define WSAEMSGSIZE					(WSABASEERR+40)
#define WSAEPROTOTYPE				(WSABASEERR+41)
#define WSAENOPROTOOPT				(WSABASEERR+42)
#define WSAEPROTONOSUPPORT			(WSABASEERR+43)
#define WSAESOCKTNOSUPPORT			(WSABASEERR+44)
#define WSAEOPNOTSUPP				(WSABASEERR+45)
#define WSAEPFNOSUPPORT				(WSABASEERR+46)
#define WSAEAFNOSUPPORT				(WSABASEERR+47)
#define WSAEADDRINUSE				(WSABASEERR+48)
#define WSAEADDRNOTAVAIL			(WSABASEERR+49)
#define WSAENETDOWN					(WSABASEERR+50)
#define WSAENETUNREACH				(WSABASEERR+51)
#define WSAENETRESET				(WSABASEERR+52)
#define WSAECONNABORTED				(WSABASEERR+53)
#define WSAECONNRESET				(WSABASEERR+54)
#define WSAENOBUFS					(WSABASEERR+55)
#define WSAEISCONN					(WSABASEERR+56)
#define WSAENOTCONN					(WSABASEERR+57)
#define WSAESHUTDOWN				(WSABASEERR+58)
#define WSAETOOMANYREFS				(WSABASEERR+59)
#define WSAETIMEDOUT				(WSABASEERR+60)
#define WSAECONNREFUSED				(WSABASEERR+61)
#define WSAELOOP					(WSABASEERR+62)
#define WSAENAMETOOLONG				(WSABASEERR+63)
#define WSAEHOSTDOWN				(WSABASEERR+64)
#define WSAEHOSTUNREACH				(WSABASEERR+65)
#define WSAENOTEMPTY				(WSABASEERR+66)
#define WSAEPROCLIM					(WSABASEERR+67)
#define WSAEUSERS					(WSABASEERR+68)
#define WSAEDQUOT					(WSABASEERR+69)
#define WSAESTALE					(WSABASEERR+70)
#define WSAEREMOTE					(WSABASEERR+71)

/*
 * Extended Sockets error constant definitions
 */
#define WSASYSNOTREADY				(WSABASEERR+91)
#define WSAVERNOTSUPPORTED			(WSABASEERR+92)
#define WSANOTINITIALISED			(WSABASEERR+93)
#define WSAEDISCON					(WSABASEERR+101)
#define WSAENOMORE					(WSABASEERR+102)
#define WSAECANCELLED				(WSABASEERR+103)
#define WSAEINVALIDPROCTABLE		(WSABASEERR+104)
#define WSAEINVALIDPROVIDER			(WSABASEERR+105)
#define WSAEPROVIDERFAILEDINIT		(WSABASEERR+106)
#define WSASYSCALLFAILURE			(WSABASEERR+107)
#define WSASERVICE_NOT_FOUND		(WSABASEERR+108)
#define WSATYPE_NOT_FOUND			(WSABASEERR+109)
#define WSA_E_NO_MORE				(WSABASEERR+110)
#define WSA_E_CANCELLED				(WSABASEERR+111)
#define WSAEREFUSED					(WSABASEERR+112)

/*
 * Error return codes from gethostbyname() and gethostbyaddr()
 * (when using the resolver). Note that these errors are
 * retrieved via WSAGetLastError() and must therefore follow
 * the rules for avoiding clashes with error numbers from
 * specific implementations or language run-time systems.
 * For this reason the codes are based at WSABASEERR+1001.
 * Note also that [WSA]NO_ADDRESS is defined only for
 * compatibility purposes.
 */

/* Authoritative Answer: Host not found */
#define WSAHOST_NOT_FOUND			(WSABASEERR+1001)

/* Non-Authoritative: Host not found, or SERVERFAIL */
#define WSATRY_AGAIN				(WSABASEERR+1002)

/* Non-recoverable errors, FORMERR, REFUSED, NOTIMP */
#define WSANO_RECOVERY				(WSABASEERR+1003)

/* Valid name, no data record of requested type */
#define WSANO_DATA					(WSABASEERR+1004)

/*
 * Define QOS related error return codes
 *
 */
#define  WSA_QOS_RECEIVERS			(WSABASEERR + 1005)
         /* at least one Reserve has arrived */
#define  WSA_QOS_SENDERS			(WSABASEERR + 1006)
         /* at least one Path has arrived */
#define  WSA_QOS_NO_SENDERS			(WSABASEERR + 1007)
         /* there are no senders */
#define  WSA_QOS_NO_RECEIVERS		(WSABASEERR + 1008)
         /* there are no receivers */
#define  WSA_QOS_REQUEST_CONFIRMED	(WSABASEERR + 1009)
         /* Reserve has been confirmed */
#define  WSA_QOS_ADMISSION_FAILURE	(WSABASEERR + 1010)
         /* error due to lack of resources */
#define  WSA_QOS_POLICY_FAILURE		(WSABASEERR + 1011)
         /* rejected for administrative reasons - bad credentials */
#define  WSA_QOS_BAD_STYLE			(WSABASEERR + 1012)
         /* unknown or conflicting style */
#define  WSA_QOS_BAD_OBJECT			(WSABASEERR + 1013)
         /* problem with some part of the filterspec or providerspecific
          * buffer in general */
#define  WSA_QOS_TRAFFIC_CTRL_ERROR	(WSABASEERR + 1014)
         /* problem with some part of the flowspec */
#define  WSA_QOS_GENERIC_ERROR		(WSABASEERR + 1015)
         /* general error */
#define  WSA_QOS_ESERVICETYPE		(WSABASEERR + 1016)
         /* invalid service type in flowspec */
#define  WSA_QOS_EFLOWSPEC			(WSABASEERR + 1017)
         /* invalid flowspec */
#define  WSA_QOS_EPROVSPECBUF		(WSABASEERR + 1018)
         /* invalid provider specific buffer */
#define  WSA_QOS_EFILTERSTYLE		(WSABASEERR + 1019)
         /* invalid filter style */
#define  WSA_QOS_EFILTERTYPE		(WSABASEERR + 1020)
         /* invalid filter type */
#define  WSA_QOS_EFILTERCOUNT		(WSABASEERR + 1021)
         /* incorrect number of filters */
#define  WSA_QOS_EOBJLENGTH			(WSABASEERR + 1022)
         /* invalid object length */
#define  WSA_QOS_EFLOWCOUNT			(WSABASEERR + 1023)
         /* incorrect number of flows */
#define  WSA_QOS_EUNKOWNPSOBJ		(WSABASEERR + 1024)
         /* unknown object in provider specific buffer */
#define  WSA_QOS_EPOLICYOBJ			(WSABASEERR + 1025)
         /* invalid policy object in provider specific buffer */
#define  WSA_QOS_EFLOWDESC			(WSABASEERR + 1026)
         /* invalid flow descriptor in the list */
#define  WSA_QOS_EPSFLOWSPEC		(WSABASEERR + 1027)
         /* inconsistent flow spec in provider specific buffer */
#define  WSA_QOS_EPSFILTERSPEC		(WSABASEERR + 1028)
         /* invalid filter spec in provider specific buffer */
#define  WSA_QOS_ESDMODEOBJ			(WSABASEERR + 1029)
         /* invalid shape discard mode object in provider specific buffer */
#define  WSA_QOS_ESHAPERATEOBJ		(WSABASEERR + 1030)
         /* invalid shaping rate object in provider specific buffer */
#define  WSA_QOS_RESERVED_PETYPE	(WSABASEERR + 1031)
         /* reserved policy element in provider specific buffer */

#endif /* ifdef WSABASEERR */
