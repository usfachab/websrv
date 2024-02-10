#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest( int clientSock ) : crs( clientSock ) {}

HTTPRequest::~HTTPRequest() {}

void	HTTPRequest::startParsingRequest()
{
	crs.hundredContinue = FALSE;

	if ( crs.headerEnd == FALSE )
		receiveHeader();
	if ( crs.headerEnd == TRUE && crs.once == TRUE )
	{
		parseMethodAndURI();
		validateUriAndParseQueries();
		parseHeaders();
		output();
	}
	if ( crs.hundredContinue == FALSE )
	{
		COUT( "hundredContinue == FALSE" );
		startParsingBodies();
	}
}

void	HTTPRequest::receiveHeader()
{
	int		rc;
	char	buffer[ BUFFER_SIZE ];

	rc = recv( crs.clientSocket, buffer, BUFFER_SIZE - 1, NO_FLAG );
	if ( rc > 0 )
	{
		buffer[ rc ] = 0;
		crs.clientRequest.append( buffer, rc );
		if ( crs.clientRequest.find( CRLF ) != std::string::npos )
		{
			crs.headerEnd = TRUE;
			crs.bodYrest = crs.clientRequest.substr( crs.clientRequest.find( CRLF ) + 4 );
		}
	}
	else if ( rc == -1 )
		throw std::invalid_argument( "recv failed: receiveHeader" );
	else
		throw std::invalid_argument( "client close connection" );
}

void	HTTPRequest::parseMethodAndURI()
{
	std::string startLine;

	try
	{
		startLine = crs.clientRequest.substr( 0, crs.clientRequest.find( "\r\n" ) );
		if ( !startLine.empty() && startLine.back() == '\r' )
			startLine.pop_back();
		crs.method	=	startLine.substr( 0, startLine.find( ' ' )  );
		crs.uri		=	startLine.substr( crs.method.length() + 1, startLine.find_last_of( ' ' ) -  crs.method.length() - 1 );
		crs.version =	startLine.substr( crs.method.length() + crs.uri.length() + 2 );
		if ( crs.method.empty() || crs.uri.empty() || crs.version.empty() )
			throw std::exception();
		if ( crs.method == "GET" )
			crs.ignoreBody = TRUE;
		if ( crs.method == "POST" )
		{
			std::string bodyFilePath	=	"./request/folder/" + randomFileNameGen();
			crs.bodyFile				=	open( bodyFilePath.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0666 );
		}
	}
	catch( const std::exception& e )
	{
		throw std::invalid_argument( "Bad request: Parsing start line failed" );
	}
}

void	HTTPRequest::validateUriAndParseQueries()
{
	validateUri();

	size_t queryPos = crs.uri.find('?');

    if ( queryPos != std::string::npos )
	{
        std::string queryString = crs.uri.substr( queryPos + 1 );
        crs.uri = crs.uri.substr( 0, queryPos );
        splitAndStoreQueries( queryString );
    }
}

void HTTPRequest::validateUri()
{
    if ( crs.uri.empty() )
        throw std::invalid_argument("Empty URI");

    if ( crs.uri.front() != '/' )
        throw std::invalid_argument("Bad request: missing / at the start of URI");

    if ( crs.uri.find(' ') != std::string::npos )
        throw std::invalid_argument("Bad request: URI contains space");

    if ( crs.uri.length() > 1024 )
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

        crs.queries[key] = value;
    }
}

void HTTPRequest::parseHeaders()
{
	std::string									line, first, second;
	std::vector<std::string>					header_lines;
	std::vector<std::string>::const_iterator	it;
	std::stringstream							headerStream ( crs.clientRequest );

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
				crs.contentLength = std::atoi( second.c_str() );
			if ( first == "Transfer-Encoding" && second == "chunked")
				crs.chunkedEncoding = TRUE;
			else if ( first == "Expect" && second == "100-continue")
				crs.hundredContinue = TRUE;
			if ( crs.contentLength > 0 && crs.chunkedEncoding == TRUE )
				throw std::exception();
			crs.headers[ first ] = second;
		}

		crs.once = FALSE;
		crs.clientRequest.clear();
	}
	catch(const std::exception& e)
	{
		throw std::invalid_argument( "Bad request: misconfiguration in header lines" );
	}
}

void	HTTPRequest::startParsingBodies()
{
	COUT( "startParsingBodies" );
	if ( crs.ignoreBody == FALSE )
	{
		if ( crs.chunkedEncoding == TRUE )
			chunkedBody();
		if ( crs.contentLength > 0 )
			regularBody();
	}
}

void	HTTPRequest::chunkedBody()
{
	char buffer[ BUFFER_SIZE ];

	COUT( "Parse chunked body" );
	if ( !crs.bodYrest.empty() )
		handleBodYrest();
	else
	{
		int rc = recv( crs.clientSocket, buffer, BUFFER_SIZE - 1, NO_FLAG );
		buffer[ rc ] = 0;
		exit(1);
	}
}

// void	HTTPRequest::handleBodYrest()
// {
// 	COUT(crs.bodYrest);
// 	std::string line;
// 	std::stringstream ss( crs.bodYrest );

// 	if ( !std::getline( ss, line, '\r' ) )
// 		return ;
	
// 	crs.currentChunkSize = std::strtol( line.c_str(), NULL, 16 );
// 	if ( crs.currentChunkSize == LONG_MAX || crs.currentChunkSize == LONG_MIN )
// 		throw std::invalid_argument( "bad request: chunk size corrupted" );
// 	COUT( crs.currentChunkSize );
// 	if ( crs.currentChunkSize != 0 )
// 	{
// 		std::string toWrite( crs.bodYrest.substr( line.length() + 2 ,  crs.currentChunkSize ) ); // crs.currentChunkSize would give a segv;
// 		COUT( toWrite.length() );
// 		write( crs.bodyFile, toWrite.c_str(), crs.currentChunkSize );
// 		ss.ignore();
// 	}

// 	std::string	getNextChunkSizeHex = crs.bodYrest.substr( crs.currentChunkSize + ( line.length() + 2 ) );
// 	if ( !getNextChunkSizeHex.empty() )
// 	{
// 		std::string tmp = getNextChunkSizeHex.substr( 2, getNextChunkSizeHex.length() - 2 );
// 		crs.nextChunkSize = std::strtol( tmp.c_str(), NULL, 16 );
// 	}
// }

size_t eofChunk( size_t startPos, std::string& chunk )
{
	bool cr = false;
	bool fl = false;

	size_t crPos = 0;
	size_t flPos = 0;

	for ( int i = startPos; i < chunk.length(); i++ )
	{
		if ( chunk.at( i ) == '\r' )
		{
			cr = !cr;
			crPos = i;
		}
		else if ( chunk.at( i ) == '\n' )
		{
			fl = !fl;
			flPos = i;
		}
		if ( cr == true && fl == true && flPos == crPos + 1 )
			return ( crPos );
	}
		return ( std::string::npos );
}

void	HTTPRequest::handleBodYrest()
{
	long		chunkSize;
	std::string	hexliteral;
	std::string	toWrite;
	std::string	chunkContent;
	size_t		crlfPos;

	hexliteral	=	crs.bodYrest.substr( 0, crs.bodYrest.find( '\r' ) );
	chunkSize	=	std::strtol( hexliteral.c_str(), NULL, 16 );

	crs.bodYrest = crs.bodYrest.substr( hexliteral.length() + 2 );

	crlfPos = eofChunk( 0, crs.bodYrest );

	if ( crlfPos != std::string::npos )
		chunkContent = crs.bodYrest.substr( 0, crlfPos );
	else
		chunkContent = crs.bodYrest.substr( 0 , crs.bodYrest.length() );

	if ( chunkSize > crs.bodYrest.length() )
		
	COUT( chunkContent );
	// COUT( hexliteral );
	// COUT( chunkSize );
	// COUT(crs.bodYrest);
	crs.bodYrest.clear();	
}



void HTTPRequest::regularBody()
{
	int rc;
	char buffer[ BUFFER_SIZE ];

	if ( !crs.bodYrest.empty() )
	{
		write( crs.bodyFile, crs.bodYrest.c_str(), crs.bodYrest.length() );
		crs.bodYrest.clear();
	}
	rc = recv( crs.clientSocket, buffer, BUFFER_SIZE - 1, NO_FLAG );
	if ( rc > 0 )
	{
		buffer[ rc ] = 0;
		write( crs.bodyFile, buffer,  rc );
	}
	else if ( rc <= 0 )
	{
		close( crs.bodyFile );
		throw std::invalid_argument( "End writing to body file or connection closed" );
	}
	// check the content length with received length
}

std::string HTTPRequest::randomFileNameGen()
{
	const std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::srand( std::time( NULL ) );
	std::string	filename;

	for ( int i = 0; i < 25; i++ )
		filename += CHARACTERS[ rand() % CHARACTERS.length() ];
	return ( filename );
}

/*----------------------------------Get Methods ------------------------------------*/

std::string HTTPRequest::getHeaders( const std::string& header_key ) const
{
	std::map<std::string, std::string>::const_iterator it;

	it  = crs.headers.find( header_key );

	if ( it != crs.headers.end() )
		return ( it->second );

	return ( "Not found" );
}

std::string HTTPRequest::getMethod() const
{
	return ( crs.method );
}

std::string HTTPRequest::getURI() const
{
	return ( crs.uri );
}

std::string HTTPRequest::getVersion() const
{
	return ( crs.version );
}

bool HTTPRequest::getConnectionStatus() const
{
	return ( crs.connStatus );
}

void HTTPRequest::output()
{
	COUT( " ---------------------- Request ---------------------- " );
	std::cout  << crs.clientRequest << std::endl;
	COUT( " -------------------- start line --------------------- " );
	std::cout << "Method: " << crs.method << " ---- " << "uri: " << crs.uri << " ---- " << "version: " << crs.version << std::endl;

	COUT( " ---------------------- Queries ---------------------- " );
	for ( auto &kv : crs.queries )
		std::cout << "key: " << kv.first << " value: " << kv.second << std::endl;

	COUT( " ---------------------- Headers ---------------------- " );
	for ( auto &kv : crs.headers )
		std::cout << "key: " << kv.first << " value: " << kv.second << std::endl;
	COUT( " ---------------------- ------- ---------------------- " );
}