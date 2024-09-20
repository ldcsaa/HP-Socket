/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Author	: Bruce Liang
 * Website	: https://github.com/ldcsaa
 * Project	: https://github.com/ldcsaa/HP-Socket
 * Blog		: http://www.cnblogs.com/ldcsaa
 * Wiki		: http://www.oschina.net/p/hp-socket
 * QQ Group	: 44636872, 75375912
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

#include "../Include/HPSocket/HPTypeDef.h"

/************************************************************************
名称：全局常量
描述：声明组件的公共全局常量
************************************************************************/

/* IOCP Socket 缓冲区最小值 */
#define MIN_SOCKET_BUFFER_SIZE					88
/* 小文件最大字节数 */
#define MAX_SMALL_FILE_SIZE						0x3FFFFF
/* 最大连接时长 */
#define MAX_CONNECTION_PERIOD					(MAXLONG / 2)
/* IOCP 处理接收事件时最大额外读取次数 */
#define MAX_IOCP_CONTINUE_RECEIVE				100

/* Server/Agent 最大连接数 */
#define MAX_CONNECTION_COUNT					(5 * 1000 * 1000)
/* Server/Agent 默认最大连接数 */
#define DEFAULT_CONNECTION_COUNT				10000
/* Server/Agent 默认 Socket 对象缓存锁定时间 */
#define DEFAULT_FREE_SOCKETOBJ_LOCK_TIME		DEFAULT_OBJECT_CACHE_LOCK_TIME
/* Server/Agent 默认 Socket 缓存池大小 */
#define DEFAULT_FREE_SOCKETOBJ_POOL				DEFAULT_OBJECT_CACHE_POOL_SIZE
/* Server/Agent 默认 Socket 缓存池回收阀值 */
#define DEFAULT_FREE_SOCKETOBJ_HOLD				DEFAULT_OBJECT_CACHE_POOL_HOLD
/* Server/Agent 默认内存块缓存池大小 */
#define DEFAULT_FREE_BUFFEROBJ_POOL				DEFAULT_BUFFER_CACHE_POOL_SIZE
/* Server/Agent 默认内存块缓存池回收阀值 */
#define DEFAULT_FREE_BUFFEROBJ_HOLD				DEFAULT_BUFFER_CACHE_POOL_HOLD
/* Client 默认内存块缓存池大小 */
#define DEFAULT_CLIENT_FREE_BUFFER_POOL_SIZE	60
/* Client 默认内存块缓存池回收阀值 */
#define DEFAULT_CLIENT_FREE_BUFFER_POOL_HOLD	60
/* Client/Agent 默认同步连接超时时间 */
#define DEFAULT_SYNC_CONNECT_TIMEOUT			10000
/* IPv4 默认绑定地址 */
#define  DEFAULT_IPV4_BIND_ADDRESS				_T("0.0.0.0")
/* IPv6 默认绑定地址 */
#define  DEFAULT_IPV6_BIND_ADDRESS				_T("::")
/* IPv4 广播地址 */
#define DEFAULT_IPV4_BROAD_CAST_ADDRESS			_T("255.255.255.255")

/* SOCKET 默认发送缓冲区大小 */
#define DEFAULT_SOCKET_SNDBUFF_SIZE				(16 * 1024)

/* TCP 默认通信数据缓冲区大小 */
#define DEFAULT_TCP_SOCKET_BUFFER_SIZE			DEFAULT_BUFFER_CACHE_CAPACITY
/* TCP 默认心跳包间隔 */
#define DEFALUT_TCP_KEEPALIVE_TIME				(60 * 1000)
/* TCP 默认心跳确认包检测间隔 */
#define DEFALUT_TCP_KEEPALIVE_INTERVAL			(20 * 1000)
/* TCP Server 默认 Listen 队列大小 */
#define DEFAULT_TCP_SERVER_SOCKET_LISTEN_QUEUE	SOMAXCONN
/* TCP Server 默认预投递 Accept 数量 */
#define DEFAULT_TCP_SERVER_ACCEPT_SOCKET_COUNT	300

/* UDP 最大数据报文最大长度 */
#define MAXIMUM_UDP_MAX_DATAGRAM_SIZE			(16 * DEFAULT_BUFFER_CACHE_CAPACITY)
/* UDP 默认数据报文最大长度 */
#define DEFAULT_UDP_MAX_DATAGRAM_SIZE			1432
/* UDP 默认 Receive 预投递数量 */
#define DEFAULT_UDP_POST_RECEIVE_COUNT			300
/* UDP 默认监测包尝试次数 */
#define DEFAULT_UDP_DETECT_ATTEMPTS				3
/* UDP 默认监测包发送间隔 */
#define DEFAULT_UDP_DETECT_INTERVAL				(60 * 1000)

/* TCP Pack 包长度位数 */
#define TCP_PACK_LENGTH_BITS					22
/* TCP Pack 包长度掩码 */
#define TCP_PACK_LENGTH_MASK					0x3FFFFF
/* TCP Pack 包最大长度硬限制 */
#define TCP_PACK_MAX_SIZE_LIMIT					0x3FFFFF
/* TCP Pack 包默认最大长度 */
#define TCP_PACK_DEFAULT_MAX_SIZE				0x040000
/* TCP Pack 包头标识值硬限制 */
#define TCP_PACK_HEADER_FLAG_LIMIT				0x0003FF
/* TCP Pack 包头默认标识值 */
#define TCP_PACK_DEFAULT_HEADER_FLAG			0x000000

/* 默认压缩/解压数据缓冲器长度 */
#define DEFAULT_COMPRESS_BUFFER_SIZE			(16 * 1024)

/* 垃圾回收检查间隔（毫秒） */
#define GC_CHECK_INTERVAL						(15 * 1000)

#define HOST_SEPARATOR_CHAR						'^'
#define PORT_SEPARATOR_CHAR						':'
#define IPV6_ADDR_BEGIN_CHAR					'['
#define IPV6_ADDR_END_CHAR						']'
#define IPV4_ADDR_SEPARATOR_CHAR				'.'
#define IPV6_ADDR_SEPARATOR_CHAR				':'
#define IPV6_ZONE_INDEX_CHAR					'%'

#define ENSURE_STOP()							{if(GetState() != SS_STOPPED) {Stop();} Wait();}
#define ENSURE_HAS_STOPPED()					{ASSERT(GetState() == SS_STOPPED); if(GetState() != SS_STOPPED) return;}
