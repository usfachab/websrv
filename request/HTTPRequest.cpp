#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest( int clientSocket ) : connClosed ( FALSE )
{
	connClosed = appandClientRequest( clientSocket );
	CLOSE( connClosed, clientSocket );

	connClosed = startParsingRequest( clientSocket );
	CLOSE( connClosed, clientSocket );
}

HTTPRequest::~HTTPRequest() {}

bool	HTTPRequest::appandClientRequest( int clientSocket )
{
	int rc;
	char buffer[ BUFFER_SIZE ];

	bool once = TRUE;
	
	COUT( "appand client request" );
	while ( TRUE )
	{
		rc = recv( clientSocket, buffer, BUFFER_SIZE - 1, NO_FLAG );
		if ( rc > 0 )
		{
			buffer[ rc ] = 0;
			clientRequest.append( buffer, rc );
			if ( once == true && clientRequest.find( "100-continue" ) )
			{
				send( clientSocket, CONTINUE, 26, NO_FLAG );
				once = FALSE;
			}
			if ( clientRequest.find( CRLF ) )
			{
				body.append( clientRequest.substr( clientRequest.find( CRLF ) + 4 ) );
				break ;
			}
		}
		else if ( rc <= 0 )
			return ( CLOSESOCKET );
	}
	return ( GOOD );
}

bool	HTTPRequest::startParsingRequest( int clientSocket ) // need some optimization
{
	std::string			line;
	std::vector<std::string> headers;
	std::stringstream	data( clientRequest );

	COUT( "start parsing request" );
	if ( !std::getline( data, line, '\n' ) )
		return ( CLOSESOCKET );
	else
		parseMethodAndURI( line );
	// create a headers lines vector
	while ( std::getline( data, line ) && line != "\r" )
	{
		if ( !line.empty() && line.back() == '\r' )
			line.pop_back();
		headers.push_back( line );
	}
	// turn vector to map
	if ( headers.size() <= 0 )
		return ( CLOSESOCKET );
	else
		parseHeaders( headers );
	// body appanding
	if ( contentLength > 0 )
		return parseBody( clientSocket, contentLength );
	return ( CLOSESOCKET );
}

void	HTTPRequest::parseMethodAndURI( std::string& request_line )
{
	COUT( "Parsing start line" );

	if ( !request_line.empty() && request_line.back() == '\r' )
		request_line.pop_back();
	method	=	request_line.substr( 0, request_line.find_first_of( ' ' )  );
	uri		=	request_line.substr( method.length() + 1, 1 );
	version =	request_line.substr( method.length() + uri.length() + 2, request_line.find_first_of( '\r' ) );
}

bool HTTPRequest::parseHeaders( const std::vector<std::string>& header_lines )
{
	std::vector<std::string>::const_iterator it;
	std::string	first;
	std::string	second;

	COUT( "Parsing Headers" );
	for ( it = header_lines.begin(); it != header_lines.end(); ++it )
	{
		try
		{
			first	=	it->substr( 0, it->find_first_of( ':' ) );
			second	=	it->substr( it->find_first_of( ' ' ) + 1 );
			if ( first == "Content-Length" )
				contentLength = std::atoi( second.c_str() );
			headers[ first ] = second;
		}
		catch( const std::exception& e )
		{
			return ( CLOSESOCKET );
		}
	}
	return ( GOOD );
}

bool HTTPRequest::parseBody( int clientSocket, size_t content_length )
{
	int rc;
	char buffer[ BUFFER_SIZE ];

	COUT( "Parsing body" );
	while ( TRUE )
	{
		rc = recv( clientSocket, buffer, BUFFER_SIZE - 1, NO_FLAG );
		if ( rc > 0 )
		{
			buffer[ rc ] = 0;
			body.append( buffer, rc );
		}
		else if ( rc <= 0 )
			return ( CLOSESOCKET );
	}
	return ( GOOD );
}

// std::string HTTPRequest::getMethod() const
// {

// }

//  std::string HTTPRequest::getMethod() const
//  {

//  }

//  std::string HTTPRequest::getURI() const
//  {

//  }