#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest( int clientSocket ) : connClosed ( FALSE )
{
	parseRequest( clientSocket );
}

void HTTPRequest::parseRequest( int clientSocket )
{
	int rc;
	char buffer[ BUFFER_SIZE ];
	std::string clientData;
	std::ofstream log;
	log.open( "./logs" );
	std::vector<std::string> headers;

	while ( TRUE )
	{
		rc = recv( clientSocket, buffer, BUFFER_SIZE - 1, NO_FLAG );
		if ( rc > 0 )
		{
			buffer[ rc ] = '\0';
			clientData.append( buffer, rc );
			if ( clientData.find( "\r\n\r\n" ) ) // Header ends here
				break ;
			
		}
		else if ( rc < 0 )
		{
			log << "[ error ] recv failed";
			break ;
		}
		else if ( rc == 0 )
		{
			log << "[ error ] client close connection";
			break ;
		}
	}

	// start parsing the request
	if ( rc == FAIL || rc == FALSE )
	{
		connClosed = TRUE;
		close( clientSocket );
	}
	else
	{
		std::stringstream	data( clientData );
		std::string			line;

		if ( ! std::getline( data, line, '\r' ) )
			connClosed = TRUE;
		// parsing Method, URI and version
		if ( connClosed == FALSE )
		{
			parseMethodAndURI( line );
			// parsing headers
			while ( std::getline( data, line ) && line != "\r" )
			{
				if ( !line.empty() && line.back() == '\r' )
					line.back() = 0;
				headers.push_back( line );
			}
			if ( headers.size() <= 0 )
				connClosed = TRUE;
			else
				parseHeaders( headers );
		}
	}
}

void HTTPRequest::parseMethodAndURI( const std::string& request_line )
{
	method	=	request_line.substr( 0, request_line.find_first_of( ' ' )  );
	uri		=	request_line.substr( method.length() + 1, 1 );
	version =	request_line.substr( method.length() + uri.length() + 2, request_line.find_first_of( '\r' ) );
}

void HTTPRequest::parseHeaders( const std::vector<std::string>& header_lines )
{
	std::vector<std::string>::const_iterator it;

	for ( it = header_lines.begin(); it != header_lines.end(); ++it )
	{
		headers[ it->substr( 0, it->find_first_of( ' ' ) ) ] = ; // Hna 
	}

	for ( auto  &kv : headers )
		std::cout << kv.first << kv.second << std::endl;
}

// void HTTPRequest::parseBody(int clientSocket, size_t content_length)
// {

// }

// std::string HTTPRequest::getMethod() const
// {

// }

//  std::string HTTPRequest::getMethod() const
//  {

//  }

//  std::string HTTPRequest::getURI() const
//  {

//  }