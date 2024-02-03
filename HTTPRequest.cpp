#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest( int clientSocket ) : connClosed ( FALSE )
{
	parseRequest( clientSocket );
}

void HTTPRequest::parseRequest( int clientSocket )
{
	COUT( "Parsing Request" );
	int rc;
	bool once = true;
	std::string clientRequest;
	char buffer[ BUFFER_SIZE ];
	std::vector<std::string> headers;

	contentLength = 0;

	while ( TRUE )
	{
		rc = recv( clientSocket, buffer, BUFFER_SIZE - 1, NO_FLAG );
		if ( rc > 0 )
		{
			buffer[ rc ] = 0;
			clientRequest.append( buffer, rc );
		}
		else if ( rc <= 0 )
		{
			COUT( rc );
			COUT( "recv finished" );
			break;
		}
	}
	COUT( clientRequest );
}

void HTTPRequest::parseMethodAndURI( const std::string& request_line )
{
	COUT( "Parsing start line" );
	method	=	request_line.substr( 0, request_line.find_first_of( ' ' )  );
	uri		=	request_line.substr( method.length() + 1, 1 );
	version =	request_line.substr( method.length() + uri.length() + 2, request_line.find_first_of( '\r' ) );
}

void HTTPRequest::parseHeaders( const std::vector<std::string>& header_lines )
{
	COUT( "Parsing Headers" );
	std::vector<std::string>::const_iterator it;
	std::string	first;
	std::string	second;

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
			std::cerr << e.what() << '\n';
			connClosed = TRUE;
		}
	}
	
	// for ( auto  &kv : headers )
	// 	std::cout << kv.first << ": " << kv.second << std::endl;
}

void HTTPRequest::parseBody( int clientSocket, size_t content_length )
{
	COUT( "Parsing body" );
	int rc;
	char buffer[ BUFFER_SIZE ];
	std::ofstream log;
	log.open( "./logs" );

	if ( contentLength > 0 )
	{
		while ( TRUE )
		{
			rc = recv( clientSocket, buffer, BUFFER_SIZE - 1, NO_FLAG );
			if ( rc > 0 )
			{
				buffer[ rc ] = 0;
				body.append( buffer );
			}
			else if ( rc == -1 )
			{
				log << "[ error ] recv failed";
				break ;
			}
			else if ( rc == 0 )
			{
				log << "[ error ] client colse connection";
				break ;
			}
		}
	}
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