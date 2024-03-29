#ifndef INCLUDE_HPP
#define INCLUDE_HPP

#include <fcntl.h>
#include <iostream>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <utility>
#include <functional>
#include <algorithm>
#include <limits.h>
#include <errno.h>
#include <stdexcept>
#include <ios>

#include <ctime>
#include <cstring>
#include <unistd.h>


// #define	ONCE				TRUE
// #define	CONTINUE			"HTTP/1.1 100 Continue\r\n\r\n"
// #define CLOSESOCKET			TRUE
// #define GOOD					FALSE
// #define CLOSE( VAL, SOCK )	if ( VAL == TRUE ){ close( SOCK );  return ; }
// #define IP					"127.0.0.2"
#define CRLF				"\r\n\r\n"
#define OK					std::cout << "OK" << std::endl;
#define BUFFER_SIZE			2048
#define SOCKET_TYPE_INET	AF_INET, SOCK_STREAM, IPPROTO_TCP
#define SA					struct sockaddr
#define SAIN				struct	sockaddr_in
#define FALSE				0
#define	TRUE				1
#define	MAX_CLIENTS			32
#define PORT				6461
#define NO_FLAG				0
#define NSDR				1024 // The number of socket descriptors to be checked
#define CHECK( VAL )		if ( VAL == -1 ) return -1;
#define ERROR( WHO, VAL )	if ( VAL == -1 ) { perror( WHO ); break; }
#define EXIT( WHO, VAL )	if ( VAL == -1 ) { perror( WHO ); exit( errno );}
#define COUT( VAL )			std::cout << VAL << std::endl;
#define CERR( VAL )			std::cerr << VAL << std::endl;
#define	TIME_OUT( VAL )		if ( VAL == 0 ) { std::cerr << "Select: Time out" << std::endl; break; }
#define	RECV_ERROR( VAL )	if (rc <= 0) { if (errno != EWOULDBLOCK) { perror("  recv() failed"); close_conn = TRUE; } break; }
#define RES_HEADER	"HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nConnection: close\r\n\r\n"
#define RES_BODY	"<html>\r\n<body>\r\n<h1>Hello, World!</h1>\r\n</body>\r\n</html>\r\n"

struct HTTPRequestParserStruct
{
	size_t			count;
	bool			Continue;
	long			currentChunkSize;
	int				bodyFileDescriptor;
	int				clientConnectionSocket;
	bool			initialProcessingDone;
	bool 			headerProcessed;
	bool			skipRequestBody;
	bool			chunkedEncoding;
	bool			expectContinueResponse;
	bool			isChunkHeader;
	bool			multipartEncoding;
	size_t			requestBodyLength;
	std::string		remainingRequestBody;
	std::string		fullClientRequest;
    std::string 	uri, method, version;
    std::map<std::string, std::string> headers, queries;
	
	size_t			chunkHeaderStart;
	size_t			chunkHeaderEnd;
	
	HTTPRequestParserStruct( int clientSock )
	{
		Continue				=	false;
		chunkHeaderStart		=	0;
		currentChunkSize		=	-1;
		count 					= 	0;
		requestBodyLength		=	0;
		headerProcessed			=	false;
		chunkedEncoding     	=   false;
		skipRequestBody			=	false;
		expectContinueResponse 	= 	false;
		isChunkHeader			=	true;
		initialProcessingDone	=	true;
		multipartEncoding		=	false;
		clientConnectionSocket	=	clientSock;
	}
};

#endif