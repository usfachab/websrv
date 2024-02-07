#include "Server.hpp"

Server::Server() : clientAddrSize ( sizeof( SAIN ) ), serverEnd ( FALSE ), close_conn ( FALSE )
{
	serverAddrInit();
	initServer();
}

Server::~Server()
{
	close( so );
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

	// for devlopment porpose Set SO_REUSEADDR to make the socket address/port reusable
	rc = setsockopt( so, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof( opt ) );
	EXIT( "setsockopt", rc );

	// rc = fcntl( so, F_SETFL, O_NONBLOCK, FD_CLOEXEC );
	rc = fcntl( so, F_SETFL, fcntl(so, F_GETFL, 0) | O_NONBLOCK);
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
				else if ( !serverEnd )
					recvAndSendClientData( i, &master_set );
			}
		}
	} while ( serverEnd == FALSE );
}


void Server::acceptIncomingConnections()
{
	while ( TRUE )
	{
		// accept every incoming connection and add each one to the master_set for monitoring by select
		newSo = accept( so, ( SA * )&clientAddr, &clientAddrSize );
		if ( newSo == -1 )
			break ;
		FD_SET( newSo, &master_set );
		HTTPRequest clientData( newSo );
		clientObject.insert( std::pair<int, HTTPRequest>( newSo, clientData ) );
		if ( newSo > maxSo )
			maxSo = newSo;
	}
}

void Server::recvAndSendClientData( int clientSocket, fd_set* master_set  )
{
	std::map<int , HTTPRequest>::iterator it;

	try
	{
		it = clientObject.find( clientSocket );
		it->second.startParsingRequest();
		std::cout << it->second.getURI() << std::endl;
	}
	catch( const std::exception& e )
	{
		FD_CLR( clientSocket, master_set );
		shutdown( clientSocket, SHUT_WR );
		std::cerr << e.what() << '\n';
	}
	
	
}