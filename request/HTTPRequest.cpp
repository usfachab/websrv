#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest( int clientSock )
{
	contentLength		=	FALSE;
	npos				= 	FAIL;
	connStatus			=	TRUE;
	headerEnd			=	FALSE;
	clientSocket		=	clientSock;
	std::string	path	=	"./request/folder/" + fileNameGen();
	bodyFile			= 	open( path.c_str() , O_CREAT | O_RDWR, 0666 );
}

HTTPRequest::~HTTPRequest() {}

std::string HTTPRequest::fileNameGen() const
{
	const std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::srand( std::time( NULL ) );
	std::string	filename;

	for ( int i = 0; i < 25; i++ )
		filename += CHARACTERS[ rand() % CHARACTERS.length() ];
	return ( filename );
}

void	HTTPRequest::startParsingRequest()
{
	int rc;
	char buffer[ BUFFER_SIZE ];
	std::string startLine;
	std::string	bodyRest;

	COUT( "startParsingRequest" );
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
				bodyRest = clientRequest.substr( clientRequest.find( CRLF ) + 4 );
				if ( !bodyRest.empty() )
					write( bodyFile,  bodyRest.c_str(), bodyRest.length() );
				parseMethodAndURI();
				parseQueries();
				parseHeaders();
			}
		}
		else if ( rc == -1 )
			throw std::invalid_argument( "recv failed" );
		else
			throw std::invalid_argument( "client close connection" );
	}
	if ( contentLength > 0 )
		parseBody( contentLength );
	
}

void	HTTPRequest::parseMethodAndURI()
{
	std::string startLine;

	COUT( "Parsing start line" );
	try
	{
		startLine = clientRequest.substr( 0, clientRequest.find( "\r\n" ) );

		if ( !startLine.empty() && startLine.back() == '\r' )
			startLine.pop_back();
		method	=	startLine.substr( 0, startLine.find_first_of( ' ' )  );
		uri		=	startLine.substr( method.length() + 1, startLine.find_last_of( ' ' ) -  method.length() - 1 );
		version =	startLine.substr( method.length() + uri.length() + 2 );
	}
	catch( const std::exception& e )
	{
		throw std::invalid_argument( "Parsing start line failed" );
	}
}

void	HTTPRequest::validateUriAndParseQueries()
{
	if ( !uri.empty() )
	{
		// chack if it starts with forward slash 
		if ( uri.front() != '/' )
			throw std::invalid_argument( "bad request: missing / at the start of uri" );
		if ( std::count( uri.begin(), uri.end(), ' ' ) != 0 )
			throw std::invalid_argument( "bad request: uri contain space" );
		
		// size_t	queryStrigStart = uri.find_first_of( '?' );

		// if ( uri.length() > 6000 )
		// 	throw std::invalid_argument( "uri length exceeted the limit which is 6000 character" );
		// if ( std::count( uri.begin(), uri.end(), '?' ) > 1 )
		// 	throw std::invalid_argument( "invalid uri: using a reserved delemiter" );
		// if ( uri.find_first_of( '?' ) != npos )
		// {
		// 	queryStrigStart++;
		// 	COUT( uri.substr( queryStrigStart ) );
		// }
	}
	else
		throw std::invalid_argument( "empty uri" );
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
		throw std::invalid_argument( "Parsing Headers failed" );
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
		write( bodyFile, buffer,  rc );
	}
	else if ( rc == -1 )
		throw std::invalid_argument( "recv failed" );
	else
		throw std::invalid_argument( "client close connection" );
}
/*----------------------------------Get Methods ------------------------------------*/

std::string HTTPRequest::getHeaders( const std::string& header_key ) const
{
	std::map<std::string, std::string>::const_iterator it;

	it  = headers.find( header_key );

	if ( it != headers.end() )
		return ( it->second );

	return ( "Not found" );
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

bool HTTPRequest::getConnectionStatus() const
{
	return ( connStatus );
}
