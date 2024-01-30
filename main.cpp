#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SOCKET_TYPE_INET	AF_INET, SOCK_STREAM, IPPROTO_TCP
#define SA					struct sockaddr
#define SAIN				struct	sockaddr_in
#define IP					"168.0.0.2"
#define FALSE				0
#define	TRUE				1
#define	MAX_CLIENTS			15
#define PORT				6461
#define NO_FLAG				0
#define	FAIL				-1
#define NSDR				1024 // The number of socket descriptors to be checked
#define CHECK( VAL )		if ( VAL == FAIL ) return FAIL;
#define ERROR( WHO, VAL )	if ( VAL == FAIL ) { perror( WHO ); break ;}
#define CLOSE( SOCK )		if ( SOCK != FAIL  ) close ( SOCK );
#define OK					std::cout << "OK" << std::endl;

struct sockaddr_in serverAddrInit( void )
{
	struct	sockaddr_in serverAddr;

	memset(&serverAddr, 0, sizeof( struct sockaddr_in ));
	serverAddr.sin_len = sizeof( struct sockaddr_in );
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons( PORT );
	serverAddr.sin_addr.s_addr = htonl( INADDR_ANY );
	
	return ( serverAddr );
}

int	initServer()
{
	int		sock = -1, _sock = -1, baddr, listn;
	struct	sockaddr_in serverAddr;

	// create a socket
	sock = socket( SOCKET_TYPE_INET ); CHECK( sock );

	fcntl( sock, F_SETFL, O_NONBLOCK, FD_CLOEXEC );

	/* ANCHOR fcntl argument
		* F_SETFL:
			Command to modify the file status flags of the socket.

		* O_NONBLOCK:
			Sets the socket to non-blocking mode. In this mode,
			operations like read(), write(), connect(), and accept()
			on this socket will not block program execution. Instead,
			if the operation cannot be completed immediately,
			these calls will return with an EWOULDBLOCK or EAGAIN error.
		
		* FD_CLOEXEC:
			Sets the close-on-exec flag for the socket file descriptor.
			When this flag is set, the socket will be automatically
			closed if the program executes a new process using exec() system calls.
			This is important for preventing the socket from being unintentionally
			passed on to child processes created with fork() followed by exec(),
			ensuring that these child processes do not unintentionally share the socket.
	*/

	serverAddr = serverAddrInit();
	// bind socket with address and port
	baddr = bind( sock, ( SA * )&serverAddr,  sizeof( struct sockaddr_in ) ); CHECK( baddr );
	// listen for connections on a socket
	listn = listen( sock,  MAX_CLIENTS ); CHECK( listn );

	return ( sock );
}

int main()
{
	int so, ac, rc, sn, se;
	struct	sockaddr_in clientAddr;
	char	buffer[200];
	socklen_t clientAddrSize = sizeof( SAIN );
	fd_set	readfds;
	const char *http_header = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Connection: close\r\n"
    "\r\n";

	const char *html_content = 
	"<html>\r\n"
	"<head><title>Test Page</title></head>\r\n"
	"<body>\r\n"
	"<h1>Hello, World!</h1>\r\n"
	"<p>This is a test page.</p>\r\n"
	"</body>\r\n"
	"</html>\r\n";

	do
	{
		// initiaze the server by creating a socket bind it and listen to it
		so = initServer();
		ERROR( "init", so );

		// select
		while ( TRUE )
		{
			FD_ZERO( &readfds );
			FD_SET( so, &readfds );

			se = select( NSDR, &readfds, NULL, NULL, NULL );
			ERROR( "select", se );

			ac = accept( so, ( SA * )&clientAddr, &clientAddrSize );
			ERROR( "accept", ac );
			sn = send( ac, http_header, strlen( http_header ), NO_FLAG );
			sn = send( ac, html_content, strlen( html_content ), NO_FLAG );
			ERROR("send", sn );
			// sleep( 4 );
			// CLOSE( ac );
			shutdown( ac, SHUT_WR );
		}

	} while ( FALSE );

	CLOSE( so );

	return 0;
}
