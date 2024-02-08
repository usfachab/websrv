#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest( int clientSock )
{
	contentLength		=	FALSE;
	connStatus			=	TRUE;
	headerEnd			=	FALSE;
	chunkedEncoding     =   FALSE;
	ignoreBody			=	FALSE;
	clientSocket		=	clientSock;
	// !std::string	path	=	"./request/folder/" + fileNameGen();
	// !bodyFile			= 	open( path.c_str() , O_CREAT | O_RDWR, 0666 );
}

HTTPRequest::~HTTPRequest() {}

std::string HTTPRequest::randomFileNameGen()
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
	if ( headerEnd == FALSE )
		receiveHeader();
	if ( headerEnd == TRUE )
	{
		parseMethodAndURI();
		validateUriAndParseQueries();
		parseHeaders();
	}
	if ( chunkedEncoding == TRUE && ignoreBody == FALSE )
		COUT( "Chunked" ); // *parseChunkedBody
	if ( contentLength > 0 && ignoreBody == FALSE )
		COUT( "normal body" ); // !cuation with bodYrest
	output();
}

void	HTTPRequest::receiveHeader()
{
	int		rc;
	char	buffer[ BUFFER_SIZE ];

	rc = recv( clientSocket, buffer, BUFFER_SIZE - 1, NO_FLAG );
	if ( rc > 0 )
	{
		buffer[ rc ] = 0;
		clientRequest.append( buffer, rc );
		if ( clientRequest.find( CRLF ) != std::string::npos )
			headerEnd = TRUE;
	}
	else if ( rc == -1 )
		throw std::invalid_argument( "recv failed" );
	else
		throw std::invalid_argument( "client close connection" );
}

void	HTTPRequest::parseMethodAndURI()
{
	std::string startLine;

	// COUT( "Parsing start line" );
	try
	{
		startLine = clientRequest.substr( 0, clientRequest.find( "\r\n" ) );
		if ( !startLine.empty() && startLine.back() == '\r' )
			startLine.pop_back();
		method	=	startLine.substr( 0, startLine.find( ' ' )  );
		uri		=	startLine.substr( method.length() + 1, startLine.find_last_of( ' ' ) -  method.length() - 1 );
		version =	startLine.substr( method.length() + uri.length() + 2 );
		if ( method == "GET" )
			ignoreBody = TRUE;
		if ( method.empty() || uri.empty() || version.empty() )
			throw std::exception();
	}
	catch( const std::exception& e )
	{
		throw std::invalid_argument( "Bad request: Parsing start line failed" );
	}
}

void	HTTPRequest::validateUriAndParseQueries()
{
	validateUri();

	size_t queryPos = uri.find('?');

    if ( queryPos != std::string::npos )
	{
        std::string queryString = uri.substr( queryPos + 1 );
        uri = uri.substr( 0, queryPos );
        splitAndStoreQueries( queryString );
    }
}

void HTTPRequest::validateUri()
{
    if ( uri.empty() )
        throw std::invalid_argument("Empty URI");

    if ( uri.front() != '/' )
        throw std::invalid_argument("Bad request: missing / at the start of URI");

    if ( uri.find(' ') != std::string::npos )
        throw std::invalid_argument("Bad request: URI contains space");

    if ( uri.length() > 1024 )
        throw std::invalid_argument("Bad request: URI length exceeded the limit");
}

void HTTPRequest::splitAndStoreQueries( const std::string& queryString )
{
    std::string param;
    std::stringstream ss( queryString );

    while ( std::getline(ss, param, '&') )
	{
        if ( param.empty() ) continue;

        size_t equalPos		=	param.find( '=' );
        std::string key		=	param.substr( 0, equalPos );
        std::string value	=	equalPos != std::string::npos ? param.substr( equalPos + 1 ) : "";

        queries[key] = value;
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

	// COUT( "Parsing Headers");
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
			second	=	it->substr( it->find_first_of( ':' ) + 2 );

			if ( first == "Content-Length" )
				contentLength = std::atoi( second.c_str() );
			if ( first == "Transfer-Encoding" && second == "chunked")
				chunkedEncoding = TRUE;
			if ( contentLength > 0 && chunkedEncoding == TRUE )
				throw std::exception();
			
			headers[ first ] = second;
		}
	}
	catch(const std::exception& e)
	{
		throw std::invalid_argument( "Bad request: misconfiguration in header lines" );
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

void HTTPRequest::output()
{
	COUT( " ---------------------- Request ---------------------- " );
	std::cout  << clientRequest << std::endl;
	COUT( " -------------------- start line --------------------- " );
	std::cout << "Method: " << method << " ---- " << "uri: " << uri << " ---- " << "version: " << version << std::endl;

	COUT( " ---------------------- Queries ---------------------- " );
	for ( auto &kv : queries )
		std::cout << "key: " << kv.first << " value: " << kv.second << std::endl;

	COUT( " ---------------------- Headers ---------------------- " );
	for ( auto &kv : headers )
		std::cout << "key: " << kv.first << " value: " << kv.second << std::endl;
	COUT( " ---------------------- ------- ---------------------- " );
}