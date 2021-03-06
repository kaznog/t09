/* Exit Games Photon - C++ Client Lib
 * Copyright (C) 2004-2013 by Exit Games GmbH. All rights reserved.
 * http://www.exitgames.com
 * mailto:developer@exitgames.com
 */

#pragma once

#include "Common-cpp/inc/Common.h"
#include "Common-cpp/inc/Helpers/Thread.h"
#include "Photon-cpp/inc/Internal/PhotonConnectListener.h"

#ifdef _EG_UNIX_PLATFORM
#	include "netdb.h"
#	include <errno.h>

#	ifdef _EG_PS3_PLATFORM
#		include <sys/socket.h>
#		include <sys/select.h>
#		include <sys/time.h>
#		include <netinet/in.h>
#		include <netex/errno.h>

#		undef EWOULDBLOCK
#		undef EINPROGRESS
#		undef ENETDOWN
#		undef ENETUNREACH
#		undef EMSGSIZE
#		undef ENOTCONN
#		undef ECONNREFUSED
#		undef ECONNRESET

#		define EWOULDBLOCK  SYS_NET_EWOULDBLOCK
#		define EINPROGRESS  SYS_NET_EINPROGRESS
#		define ENETDOWN     SYS_NET_ENETDOWN
#		define ENETUNREACH  SYS_NET_ENETUNREACH
#		define EMSGSIZE     SYS_NET_EMSGSIZE
#		define ENOTCONN     SYS_NET_ENOTCONN
#		define ECONNREFUSED SYS_NET_ECONNREFUSED
#		define ECONNRESET   SYS_NET_ECONNRESET
#		define select(a, b, c, d, e)	socketselect((a), (b), (c), (d), (e))
#	else
#		include <sys/ioctl.h>
#	endif

#	include <unistd.h>

#	include "arpa/inet.h"

#	ifdef _EG_MARMALADE_PLATFORM
#		define INADDR_NONE -1 // should be defined by platform's BSD-socket implementation, but for some reason it isn't for Marmalade-platform
#		include "s3eSocket.h" // only needed for error-check workaround, when passing url to ip-connect function
#	elif _EG_BLACKBERRY_PLATFORM
#		include <sys/socket.h>
#	endif

#	define INVALID_SOCKET -1
#	define IN_ADDR_T(x) in_addr_t(x)
#	define IS_INVALID_SOCKET(e) ((e) < 0)
#	define SOCKET int

#	ifdef _EG_PS3_PLATFORM
#		define LAST_ERROR sys_net_errno
#	else
#		define LAST_ERROR errno
#	endif
#else
#	define IN_ADDR_T(x) (x)
#	define IS_INVALID_SOCKET(e) ((e) == INVALID_SOCKET)
#	define LAST_ERROR WSAGetLastError()

#	undef EWOULDBLOCK
#	undef EINPROGRESS
#	undef ENETDOWN
#	undef ENETUNREACH
#	undef EMSGSIZE
#	undef ENOTCONN
#	undef ECONNREFUSED
#	undef ECONNRESET

#	define EWOULDBLOCK  WSAEWOULDBLOCK
#	define EINPROGRESS  WSAEINPROGRESS
#	define ENETDOWN     WSAENETDOWN
#	define ENETUNREACH  WSAENETUNREACH
#	define EMSGSIZE     WSAEMSGSIZE
#	define ENOTCONN     WSAENOTCONN
#	define ECONNREFUSED WSAECONNREFUSED
#	define ECONNRESET   WSAECONNRESET
#endif

#if defined _EG_APPLE_PLATFORM || defined _EG_ANDROID_PLATFORM || defined _EG_BLACKBERRY_PLATFORM // Apple, Google and Blackberry already define them theirselves, but in a really weird way (on Apple platforms y=HTONL(x) changes byteorder in dest and source(!), not only in dest like htonl(x) and like everyone would expect)
#	undef HTONS
#	undef HTONL
#endif
#define HTONS htons
#define HTONL htonl

/* If the macro is not defined, callback functions can be called
at once during call of functions startConnection or
sendPackage.

If this macro is defined, callback functions at connection or
sending of udp messages will be called in execution time of
function service.                                    */
#define PHOTON_CONNECT_CALL_CALLBACK_ONLY_IN_SERVICE

/**
Internal state of connection for PhotonConnect. 
*/
typedef enum
{
	NC_Closed = 0,			// Connection closed.
	NC_DoingDNSResolution, // Is resolving the ip address for the passed hostname.
	NC_DNSResolutionDone, // Is resolving the ip address for the passed hostname.
	NC_Connecting,			// Is connecting now.
	NC_NotConnectedError,   // There was an error of establishing connection.
	NC_Connected			// Connection is established.
} PhotonConnect_ConnectionState;

/**
Codes of internal states for processes of reception-transfer
PhotonConnect.                                                    
*/
typedef enum
{
	NC_Reported = 0,	// The current transfer state was reported.
	NC_Processing,		// Data transferring.
	NC_NeedReportOk,	// Need to inform about successful data transfer.
	NC_NeedReportError  // Need to inform about unsuccessful data transfer.
} PhotonConnect_SendRecvState;

namespace ExitGames
{
	namespace Photon
	{
		namespace Internal
		{
			class PhotonConnect
			{
			public:
				PhotonConnect(PhotonConnectListener& listener);
				virtual ~PhotonConnect(void) = 0;

				void service(void);
				bool startConnection(const char* ipAddr);
				int stopConnection(void);
				int sendPackage(const nByte* src, int count);
				PhotonConnect* retain(void);
				void release(void);

				int getBytesOut(void);
				int getBytesIn(void);
			protected:
				int recvBuffer(nByte* pBuff, int iSize, int* pBytesRead);

				static unsigned short mInstanceCount;
				PhotonConnectListener& mListener; // pointer to parent.
				int mError; // Code of last error.
				SOCKET mSocket; // Connection socket.
				int mConnectionState; // Current state of connection.
				int mSendState; // Current state of data sending.
				nByte* mpSendBuffer; // Pointer to the buffer of sent data.
				int mSendBufferSize; // Size of the data buffer for sending.
				int mBytesToSend; // Number of bytes for sending.
				int mBytesOut; // counts all bytes going out (include header)
				int	mBytesIn; // counts all bytes coming in (include header)
				struct sockaddr_in mAddr; // socket address to connect
			private:
				static int initialize(void);
				static void deinitialize(void);
				int sendBuffer(char* pBuff, int iSize);
				virtual SOCKET socket(void) = 0;
				virtual unsigned short getDefaultPort(void) = 0;
				virtual bool checkConnection(void) = 0;
				virtual int recvBuffer(nByte** ppBuff, int* pBytesRead) = 0;
				bool connect(void);
				static void getHostByName(void* pArg);

				struct hostent* mpHostEnt;
				Common::UTF8String mHostName;
				unsigned char mRefCount;
			};
		}
	}
}