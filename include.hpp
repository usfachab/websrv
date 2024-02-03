#ifndef INCLUDE_HPP
#define INCLUDE_HPP

#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <cstring>
#include <fstream>
#include <vector>

#define GOOD				FALSE
#define CLOSESOCKET			TRUE
#define CLOSE( VAL, SOCK )	if ( VAL == TRUE ){ close( SOCK );  return ; }
#define OK					std::cout << "OK" << std::endl;
#define BUFFER_SIZE			1024
#define SOCKET_TYPE_INET	AF_INET, SOCK_STREAM, IPPROTO_TCP
#define SA					struct sockaddr
#define SAIN				struct	sockaddr_in
#define IP					"127.0.0.2"
#define FALSE				0
#define	TRUE				1
#define	MAX_CLIENTS			32
#define PORT				6461
#define NO_FLAG				0
#define	FAIL				-1
#define NSDR				1024 // The number of socket descriptors to be checked
#define CHECK( VAL )		if ( VAL == FAIL ) return FAIL;
// #define CLOSE( SOCK )		if ( SOCK != FAIL  ) close ( SOCK );
#define ERROR( WHO, VAL )	if ( VAL == FAIL ) { perror( WHO ); break; }
#define EXIT( WHO, VAL )	if ( VAL == FAIL ) { perror( WHO ); exit( errno );}
#define COUT( VAL )			std::cout << VAL << std::endl;
#define CERR( VAL )			std::cerr << VAL << std::endl;
#define	TIME_OUT( VAL )		if ( VAL == 0 ) { std::cerr << "Select: Time out" << std::endl; break; }
#define	RECV_ERROR( VAL )	if (rc < 0) { if (errno != EWOULDBLOCK) { perror("  recv() failed"); close_conn = TRUE; } break; }
#define RES_HEADER	"HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nConnection: close\r\n\r\n"
#define RES_BODY	"<html>\r\n<body>\r\n<h1>Hello, World!</h1>\r\n</body>\r\n</html>\r\n"



#endif