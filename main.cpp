#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

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
#define CLOSE( SOCK )		if ( SOCK != FAIL  ) close ( SOCK );
#define ERROR( WHO, VAL )	if ( VAL == FAIL ) { perror( WHO ); break; }
#define EXIT( WHO, VAL )	if ( VAL == FAIL ) { perror( WHO ); exit( errno );}
#define COUT( VAL )			std::cout << VAL << std::endl;
#define	TIME_OUT( VAL )		if ( VAL == 0 ) { std::cerr << "Select: Time out" << std::endl; break; }
#define	RECV_ERROR( VAL )	if (rc < 0) { if (errno != EWOULDBLOCK) { perror("  recv() failed"); close_conn = TRUE; } break; }		

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
	int		rc, so = -1, opt = 1;
	struct	sockaddr_in serverAddr;

	// create a socket
	so = socket( SOCKET_TYPE_INET ); CHECK( so );

	// for ddevlopment porpose Set SO_REUSEADDR to make the socket address/port reusable
	rc = setsockopt( so, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof( opt ) ); CHECK( rc );

	rc = fcntl( so, F_SETFL, O_NONBLOCK, FD_CLOEXEC ); CHECK( rc );

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
	rc = bind( so, ( SA * )&serverAddr,  sizeof( struct sockaddr_in ) ); CHECK( rc );
	// listen for connections on a socket
	rc = listen( so,  MAX_CLIENTS ); CHECK( rc );

	return ( so );
}

#define RES_HEADER	"HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nConnection: close\r\n\r\n"
#define RES_BODY	"<html>\r\n<body>\r\n<h1>Hello, World!</h1>\r\n</body>\r\n</html>\r\n"

int main()
{
	int so, ac, rc, sn, se, maxSo, readyToReadSos, serverEnd = FALSE, newSo = -1, close_conn = FALSE, len;
	struct	sockaddr_in clientAddr;
	char	buffer[128];
	socklen_t clientAddrSize = sizeof( SAIN );
	fd_set	master_set, working_set;
	struct timeval      timeout;

	// initiaze the server by creating a socket bind it and listen for incoming connections
	so = initServer();
	EXIT( "init", so );

	FD_ZERO( &master_set );
	maxSo = so;
	FD_SET( so, &master_set );
	// if no activity after 3 minutes this program will end.
	timeout.tv_sec  = 3 * 60;
	timeout.tv_usec = 0;
	do
	{

		memcpy( &working_set, &master_set, sizeof( master_set ) );
		se = select( maxSo + 1, &working_set, NULL, NULL, &timeout );
		ERROR( "select", se );
		TIME_OUT( se );

		readyToReadSos = se;
		for ( int i = 0; i <= maxSo && readyToReadSos > 0; ++i )
		{
			// maxSo == 4
			if ( FD_ISSET( i, &working_set ) )
			{
				// readyToreadSos == 1;
				readyToReadSos--;
				// readyToreadSos == 0;
				if ( i == so ) // for accepting the incoming connections
				{
					do
					{
						// accept every incoming connection and add each one to the master_set for monitoring by select
						newSo = accept( so, ( SA * )&clientAddr, &clientAddrSize );
						ERROR( "accept", newSo );
						FD_SET( newSo, &master_set );
						COUT( "New incoming connection accepted and added to master_set" );
						if ( newSo > maxSo )
							maxSo = newSo;
						
					} while ( newSo != -1 );
				}
				else // handling read and send client socket
				{
					COUT( "Client socket is readable" );
					do
					{
						rc = recv( i, buffer, sizeof( buffer ), NO_FLAG );
						COUT( rc );
						if ( rc < 0 )
						{
							COUT( "Recv failed" );
							close_conn = TRUE;
							break;
						}
						if ( rc  == 0 )
						{
							COUT( "Connection closed" );
							close_conn = TRUE;
							break;
						}
						COUT( "Data successfuly recieved form client" );
						len = rc;
						COUT( len );
						sn = send( i, "Hello client", 12, NO_FLAG );
						if ( sn < 0 )
						{
							perror("send failed");
							close_conn = TRUE;
							break;
						}
					} while ( TRUE );
					if ( close_conn )
					{
						CLOSE( i );
						FD_CLR( i, &master_set );
						if ( i == maxSo )
						{
							while ( FD_ISSET( maxSo, &master_set ) == FALSE )
								maxSo--;
						}
					}
				}
			}
		}

	} while ( serverEnd == FALSE );

	CLOSE( so );

	return 0;
}
