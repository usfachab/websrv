#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest( int clientSock )
{
	COUT( "HTTPRequest constructor called" );
	contentLength		=	FALSE;
	connStatus			=	TRUE;
	headerEnd			=	FALSE;
	clientSocket		=	clientSock;
	chunkedEncoding     =   FALSE;
	// std::string	path	=	"./request/folder/" + fileNameGen();
	// bodyFile			= 	open( path.c_str() , O_CREAT | O_RDWR, 0666 );
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
	// int rc;
	// char buffer[ BUFFER_SIZE ];
	// std::string	bodyRest;

	// if ( !headerEnd )
	// {
	// 	rc = recv( clientSocket, buffer, BUFFER_SIZE - 1, NO_FLAG );
	// 	if ( rc > 0 )
	// 	{
	// 		buffer[ rc ] = 0;
	// 		clientRequest.append( buffer, rc );
	// 		if ( clientRequest.find( CRLF ) != std::string::npos )
	// 		{
	// 			headerEnd = TRUE;
	// 			bodyRest = clientRequest.substr( clientRequest.find( CRLF ) + 4 );
	// 			if ( !bodyRest.empty() )
	// 				write( bodyFile,  bodyRest.c_str(), bodyRest.length() );
	// 			parseMethodAndURI();
	// 			validateUriAndParseQueries();
	// 			parseHeaders();
	// 		}
	// 	}
	// }
	// else if ( rc == -1 )
	// 	throw std::invalid_argument( "recv failed" );
	// else
	// 	throw std::invalid_argument( "client close connection" );

	COUT( "start Parsing Client Request" );
	if ( headerEnd == FALSE )
		receiveHeader();
	if ( headerEnd == TRUE )
	{
		parseMethodAndURI();
		validateUriAndParseQueries();
		parseHeaders();
	}
	if ( chunkedEncoding == TRUE )
		COUT( "Chunked" ); // parseChunkedBody
	// else if ( contentLength > 0 )
	// 	parseBody( contentLength );
}

void	HTTPRequest::receiveHeader()
{
	int rc;
	char	buffer[ BUFFER_SIZE ];

	rc = recv( clientSocket, buffer, BUFFER_SIZE - 1, NO_FLAG );
	if ( rc > 0 )
	{
		buffer[ rc ] = 0;
		clientRequest.append( buffer, rc );
		if ( clientRequest.find( CRLF ) != std::string::npos )
		{
			headerEnd = TRUE;
			// rest of body
		}
	}
	else if ( rc == -1 )
		throw std::invalid_argument( "recv failed" );
	else
		throw std::invalid_argument( "client close connection" );
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

// void HTTPRequest::validateUriAndParseQueries() {
//     validateUri();
//     parseQueryString();
// }

// void HTTPRequest::validateUri()
// {
//     if ( uri.empty() )
//         throw std::invalid_argument("Empty URI");

//     if ( uri.front() != '/' )
//         throw std::invalid_argument("Bad request: missing / at the start of URI");

//     if ( uri.find(' ') != std::string::npos )
//         throw std::invalid_argument("Bad request: URI contains space");

//     if ( uri.length() > 1024 )
//         throw std::invalid_argument("Bad request: URI length exceeded the limit");
// }

// void HTTPRequest::parseQueryString()
// {
//     size_t queryPos = uri.find('?');

//     if ( queryPos != std::string::npos )
// 	{
//         std::string queryString = uri.substr( queryPos + 1 );
//         uri = uri.substr( 0, queryPos );
//         splitAndStoreQueries( queryString );
//     }
// }

// void HTTPRequest::splitAndStoreQueries( const std::string& queryString )
// {
//     std::stringstream ss( queryString );
//     std::string param;

//     while ( std::getline(ss, param, '&') )
// 	{
//         if ( param.empty() ) continue;

//         size_t equalPos		=	param.find( '=' );
//         std::string key		=	param.substr( 0, equalPos );
//         std::string value	=	equalPos != std::string::npos ? param.substr( equalPos + 1 ) : "";

//         queries[key] = value;
//     }
// }

void	HTTPRequest::validateUriAndParseQueries()
{
	if ( uri.empty() )
		throw std::invalid_argument( "empty uri" );

	std::string	query;
	std::string param;
	size_t		queryStrigStartPos = uri.find_first_of( '?' );

	if ( uri.front() != '/' )
		throw std::invalid_argument( "bad request: missing / at the start of uri" );
	if ( std::count( uri.begin(), uri.end(), ' ' ) != 0 )
		throw std::invalid_argument( "bad request: uri contain space" );
	if ( uri.length() > 1024 )
		throw std::invalid_argument( "bad request: uri length exceeted the limit" );
	
	if ( uri.find_first_of( '?' ) != std::string::npos )
	{
		query	=	uri.substr( queryStrigStartPos + 1 );
		uri.resize( uri.length() - ( query.length() + 1 ) );
		if ( !query.empty() )
		{
			std::stringstream ss( query );
			size_t numberOfQueries = std::count( query.begin(), query.end(), '&' ) + 1;

			for ( int i = 0; i <  numberOfQueries; i++)
			{
				std::getline( ss, param, '&' );
				COUT( "Param: " + param );
				if ( !param.empty() )
				{
					size_t	equalPos = param.find_first_of( '=' );
					if ( equalPos != std::string::npos )
					{
						std::string key		( param.substr( 0, equalPos ) );
						std::string value	( param.substr( key.length() + 1 , param.length() - ( key.length() + 1 ) ) );

						queries.insert( std::pair<std::string, std::string>( key, value ) );
					}
					else
						queries.insert( std::pair<std::string, std::string>( param, "" ) );
				}
			}
		}
	}
	for ( auto &kv : queries )
		std::cout << "key: " << kv.first << " second: " << kv.second << std::endl;
		
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
			if ( first == "Transfer-Encoding" && second == "chunked")
				chunkedEncoding = TRUE;
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
