#ifndef SERVER_HPP
#define SERVER_HPP
#include "../include/include.hpp"
#include "../request/HTTPRequestParser.hpp"

class Server
{
private:
	int		so, rc, maxSo;
	int		newSo, readyToReadSos, len;
	int		serverEnd, close_conn;
	char	buffer[ BUFFER_SIZE ];
	fd_set	master_set, working_set;

	struct	sockaddr_in	serverAddr, clientAddr;
	socklen_t			clientAddrSize;
	struct timeval      timeout;

	std::map<int, HTTPRequestParser> clientObject;

public:
	Server();
	~Server();
	void initServer( void );
	void serverAddrInit( void );
	void CheckReadableSockets( void );
	void acceptIncomingConnections( void );
	void recvAndSendClientData( int, fd_set* );
};

#endif