#include "Server.hpp"

Server::Server() : clientAddrSize ( sizeof( SAIN ) ), serverEnd ( FALSE ), close_conn ( FALSE )
{
	serverAddrInit();
	initServer();
}

Server::~Server()
{
	CLOSE( so );
}


void Server::serverAddrInit( void )
{
	memset(&serverAddr, 0, sizeof( struct sockaddr_in ));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons( PORT );
	serverAddr.sin_addr.s_addr = htonl( INADDR_ANY );
}

void	Server::initServer( void )
{
	int	opt = 1;

	// create a socket
	so = socket( SOCKET_TYPE_INET );
	EXIT( "socket", so );

	// for ddevlopment porpose Set SO_REUSEADDR to make the socket address/port reusable
	rc = setsockopt( so, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof( opt ) );
	EXIT( "setsockopt", rc );

	rc = fcntl( so, F_SETFL, O_NONBLOCK, FD_CLOEXEC );
	EXIT( "fcntl", rc );

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

	// bind socket with address and port
	rc = bind( so, ( SA * )&serverAddr,  sizeof( struct sockaddr_in ) );
	EXIT( "bind", rc );
	// listen for connections on a socket
	rc = listen( so,  MAX_CLIENTS );
	EXIT( "listen", rc );

	maxSo = so;
	FD_ZERO( &master_set );
	FD_SET( so, &master_set );
	timeout.tv_sec  = 3 * 60; // *if no activity after 3 minutes this program will end.
	timeout.tv_usec = 0;
}

void Server::CheckReadableSockets( void )
{
	do
	{
		memcpy( &working_set, &master_set, sizeof( master_set ) );
		rc = select( maxSo + 1, &working_set, NULL, NULL, &timeout );
		ERROR( "select", rc );
		TIME_OUT( rc );

		readyToReadSos = rc;

		for ( int i = 0; i <= maxSo && readyToReadSos > 0; ++i )
		{
			if ( FD_ISSET( i, &working_set ) )
			{
				readyToReadSos--;
				if ( i == so ) // for accepting the incoming connections
					acceptIncomingConnections();
				else
					recvAndSendClientData( i );					
			}
		}
	} while ( serverEnd == FALSE );
}


void Server::acceptIncomingConnections()
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

void Server::recvAndSendClientData( int clientSocket )
{
	COUT( "Client socket is readable" );
	do
	{
		rc = recv( clientSocket, buffer, sizeof( buffer ), NO_FLAG );
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
		rc = send( clientSocket, "Hello client", 12, NO_FLAG );
		if ( rc < 0 )
		{
			perror("send failed");
			close_conn = TRUE;
			break;
		}
	} while ( TRUE );

	if ( close_conn )
	{
		CLOSE( clientSocket );
		FD_CLR( clientSocket, &master_set );
		if ( clientSocket == maxSo )
		{
			while ( FD_ISSET( maxSo, &master_set ) == FALSE )
				maxSo--;
		}
	}
}