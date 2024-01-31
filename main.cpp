#include "Server.hpp"

int main()
{
	Server srv;
	srv.CheckReadableSockets();
	
	


	// int so, ac, rc, sn, se, maxSo, readyToReadSos, serverEnd = FALSE, newSo = -1, close_conn = FALSE, len;
	// struct	sockaddr_in clientAddr;
	// char	buffer[128];
	// socklen_t clientAddrSize = sizeof( SAIN );
	// fd_set	master_set, working_set;
	// struct timeval      timeout;

	// FD_ZERO( &master_set );
	// maxSo = so;
	// FD_SET( so, &master_set );
	// // if no activity after 3 minutes this program will end.
	// timeout.tv_sec  = 3 * 60;
	// timeout.tv_usec = 0;
	// do
	// {

	// 	memcpy( &working_set, &master_set, sizeof( master_set ) );
	// 	se = select( maxSo + 1, &working_set, NULL, NULL, &timeout );
	// 	ERROR( "select", se );
	// 	TIME_OUT( se );

	// 	readyToReadSos = se;
		// for ( int i = 0; i <= maxSo && readyToReadSos > 0; ++i )
		// {
		// 	// maxSo == 4
		// 	if ( FD_ISSET( i, &working_set ) )
		// 	{
		// 		// readyToreadSos == 1;
		// 		readyToReadSos--;
		// 		// readyToreadSos == 0;
		// 		if ( i == so ) // for accepting the incoming connections
		// 		{
		// 			do
		// 			{
		// 				// accept every incoming connection and add each one to the master_set for monitoring by select
		// 				newSo = accept( so, ( SA * )&clientAddr, &clientAddrSize );
		// 				ERROR( "accept", newSo );
		// 				FD_SET( newSo, &master_set );
		// 				COUT( "New incoming connection accepted and added to master_set" );
		// 				if ( newSo > maxSo )
		// 					maxSo = newSo;
						
		// 			} while ( newSo != -1 );
		// 		}
		// 		else // handling read and send client socket
		// 		{
		// 			COUT( "Client socket is readable" );
		// 			do
		// 			{
		// 				rc = recv( i, buffer, sizeof( buffer ), NO_FLAG );
		// 				COUT( rc );
		// 				if ( rc < 0 )
		// 				{
		// 					COUT( "Recv failed" );
		// 					close_conn = TRUE;
		// 					break;
		// 				}
		// 				if ( rc  == 0 )
		// 				{
		// 					COUT( "Connection closed" );
		// 					close_conn = TRUE;
		// 					break;
		// 				}
		// 				COUT( "Data successfuly recieved form client" );
		// 				len = rc;
		// 				COUT( len );
		// 				sn = send( i, "Hello client", 12, NO_FLAG );
		// 				if ( sn < 0 )
		// 				{
		// 					perror("send failed");
		// 					close_conn = TRUE;
		// 					break;
		// 				}
		// 			} while ( TRUE );
		// 			if ( close_conn )
		// 			{
		// 				CLOSE( i );
		// 				FD_CLR( i, &master_set );
		// 				if ( i == maxSo )
		// 				{
		// 					while ( FD_ISSET( maxSo, &master_set ) == FALSE )
		// 						maxSo--;
		// 				}
		// 			}
		// 		}
		// 	}
		// }

	// } while ( serverEnd == FALSE );

	// CLOSE( so );

	return 0;
}
