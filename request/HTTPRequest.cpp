#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest( int clientSock )
{
	clientSocket	=	clientSock;
	headerEnd		=	FALSE;
	connStatus		=	TRUE;
	contentLength	=	0;
	body = fopen( "./folder/body.txt", "rw" );
}

HTTPRequest::~HTTPRequest() {}

void	HTTPRequest::startParsingRequest()
{
	int rc;
	char buffer[ BUFFER_SIZE ];
	std::string startLine;
	const size_t npos = -1;

	if ( !headerEnd )
	{
		rc = recv( clientSocket, buffer, BUFFER_SIZE - 1, NO_FLAG );
		if ( rc > 0 )
		{
			buffer[ rc ] = 0;
			clientRequest.append( buffer, rc );
			if ( clientRequest.find( CRLF ) != npos )
			{
				headerEnd = TRUE;
				// body << clientRequest.substr( clientRequest.find( CRLF ) + 4 );
				fput( clientRequest.substr( clientRequest.find( CRLF ) + 4 ).c_str(), body);
				startParsingHeaders();
			}
		}
		else
			connStatus = FALSE;
	}
	else if ( contentLength > 0 )
	{
		COUT( contentLength );
		parseBody( contentLength );
	}
}

void	HTTPRequest::startParsingHeaders()
{
	parseMethodAndURI();
	parseHeaders();
}


void	HTTPRequest::parseMethodAndURI()
{
	COUT( "Parsing start line" );
	std::string startLine;

	try
	{
		startLine = clientRequest.substr(0, clientRequest.find( "\r\n" ) );

		if ( !startLine.empty() && startLine.back() == '\r' )
			startLine.pop_back();
		method	=	startLine.substr( 0, startLine.find_first_of( ' ' )  );
		uri		=	startLine.substr( method.length() + 1, 1 );
		version =	startLine.substr( method.length() + uri.length() + 2, startLine.find_first_of( '\r' ) );
	}
	catch(const std::exception& e)
	{
		connStatus = FALSE;
		std::cerr << e.what() << '\n';
	}
	
}

void HTTPRequest::parseHeaders()
{
	std::string									line;
	std::string									first;
	std::string									second;
	std::vector<std::string>					header_lines;
	std::vector<std::string>::const_iterator	it;
	std::stringstream							headerStream ( clientRequest );

	COUT( "Parsing Headers");
	try
	{
		std::getline( headerStream, line );
		while ( std::getline( headerStream, line ) && line != "\r" )
		{
			if ( !line.empty() && line.back() == '\r' )
				line.pop_back();
			header_lines.push_back( line );
		}

		for ( it = header_lines.begin(); it != header_lines.end(); ++it )
		{
			first	=	it->substr( 0, it->find_first_of( ':' ) );
			second	=	it->substr( it->find_first_of( ' ' ) + 1 );
			if ( first == "Content-Length" )
				contentLength = std::atoi( second.c_str() );
			headers[ first ] = second;
		}
	}
	catch(const std::exception& e)
	{
		connStatus = FALSE;
		std::cerr << e.what() << '\n';
	}
}

void HTTPRequest::parseBody( size_t content_length )
{
	int rc;
	char buffer[ BUFFER_SIZE ];

	COUT( "Parsing body" );
	rc = recv( clientSocket, buffer, BUFFER_SIZE - 1, NO_FLAG );
	COUT( rc );
	if ( rc > 0 )
	{
		buffer[ rc ] = 0;
		// body << buffer;
	}
	else
		connStatus = FALSE;
}

std::string HTTPRequest::getMethod() const
{
	return ( method );
}

std::string HTTPRequest::getURI() const
{
	return ( uri );
}

std::string HTTPRequest::getVersion() const
{
	return ( version );
}

// std::string HTTPRequest::getBody() const
// {
// 	return ( body );
// }

bool HTTPRequest::getConnectionStatus() const
{
	return ( connStatus );
}

// std::string getHeader( const std::string& header_name ) const
// {

// }
// bool	HTTPRequest::appandClientRequest( int clientSocket )
// {
// 	int rc;
// 	char buffer[ BUFFER_SIZE ];

// 	bool once = TRUE;
	
// 	COUT( "appand client request" );
// 	rc = recv( clientSocket, buffer, BUFFER_SIZE - 1, NO_FLAG );
// 	if ( rc > 0 )
// 	{
// 		buffer[ rc ] = 0;
// 		clientRequest.append( buffer, rc );
// 		if ( once == true && clientRequest.find( "100-continue" ) )
// 		{
// 			send( clientSocket, CONTINUE, 26, NO_FLAG );
// 			once = FALSE;
// 		}
// 		if ( clientRequest.find( CRLF ) )
// 			body.append( clientRequest.substr( clientRequest.find( CRLF ) + 4 ) );
// 	}
// 	else if ( rc <= 0 )
// 		return ( CLOSESOCKET );
// 	return ( GOOD );
// }