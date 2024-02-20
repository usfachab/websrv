#include "HTTPRequestParser.hpp"

HTTPRequestParser::HTTPRequestParser( int clientConnectionSocket )
:	_s_ ( clientConnectionSocket ),
	clientDataFile( new std::ofstream )
{}

HTTPRequestParser::~HTTPRequestParser() 
{
	// if ( clientDataFile )
	// {
		// if ( clientDataFile->is_open() )
		// {
		// // 	clientDataFile->close();
		// // 	delete clientDataFile;
		// }
    // }
	// COUT( _s_.clientConnectionSocket );
	// COUT( "client object destructed" );
}

void	HTTPRequestParser::processIncomingRequest()
{	
	_s_.expectContinueResponse = FALSE;
	if ( !_s_.headerProcessed )
		fetchRequestHeader();
	if ( _s_.headerProcessed && _s_.initialProcessingDone )
	{
		extractMethodAndUri();
		validateUriAndExtractQueries();
		extractHttpHeaders();
		// output();
	}
	if ( _s_.expectContinueResponse == FALSE )
	{
		try
		{
			if ( _s_.skipRequestBody )
				return ;
			if ( _s_.chunkedEncoding )
				processChunkedRequestBody();
			// if ( _s_.multipartEncoding )
			// 	processMultipartRequestBody();
			else if ( _s_.requestBodyLength > 0 )
			{
				COUT( "process Regular Request Body" );
				processRegularRequestBody();
			}
		}
		catch(const std::exception& e)
		{
			throw std::invalid_argument( e.what() );
		}
	}
}

void	HTTPRequestParser::fetchRequestHeader()
{
	int		rc;
	char	buffer[ BUFFER_SIZE ];

	rc = recv( _s_.clientConnectionSocket, buffer, BUFFER_SIZE - 1, NO_FLAG );
	if ( rc > 0 )
	{
		buffer[ rc ] = 0;
		_s_.fullClientRequest.append( buffer, rc );
		if ( _s_.fullClientRequest.find( CRLF ) != std::string::npos )
		{
			_s_.headerProcessed = TRUE;
			_s_.remainingRequestBody = _s_.fullClientRequest.substr( _s_.fullClientRequest.find( CRLF ) + 4 );
		}
	}
	else if ( rc == -1 )
		throw std::invalid_argument( "recv failed: fetchRequestHeader" );
	else
		throw std::invalid_argument( "client close connection" );
}

void	HTTPRequestParser::extractMethodAndUri()
{
	std::string startLine;

	try
	{
		startLine = _s_.fullClientRequest.substr( 0, _s_.fullClientRequest.find( "\r\n" ) );
		if ( !startLine.empty() && startLine.back() == '\r' )
			startLine.pop_back();
		_s_.method	=	startLine.substr( 0, startLine.find( ' ' )  );
		_s_.uri		=	startLine.substr( _s_.method.length() + 1, startLine.find_last_of( ' ' ) -  _s_.method.length() - 1 );
		_s_.version =	startLine.substr( _s_.method.length() + _s_.uri.length() + 2 );
		if ( _s_.method.empty() || _s_.uri.empty() || _s_.version.empty() )
			throw std::exception();
		if ( _s_.method == "GET" )
			_s_.skipRequestBody = TRUE;
		if ( _s_.method == "POST" )
		{
			std::string bodyFilePath	=	"./data/" + generateRandomFileName();
			clientDataFile->open( bodyFilePath.c_str(), std::ios::binary | std::ios::trunc );
		}
	}
	catch( const std::exception& e )
	{
		throw std::invalid_argument( "Bad request: Parsing start line failed" );
	}
}

void	HTTPRequestParser::validateUriAndExtractQueries()
{
	validateUri();

	size_t queryPos = _s_.uri.find('?');

    if ( queryPos != std::string::npos )
	{
        std::string query = _s_.uri.substr( queryPos + 1 );
        _s_.uri = _s_.uri.substr( 0, queryPos );
        decomposeQueryParameters( query );
    }
}

void	HTTPRequestParser::validateUri()
{
    if ( _s_.uri.empty() )
        throw std::invalid_argument("Empty URI");

    if ( _s_.uri.front() != '/' )
        throw std::invalid_argument("Bad request: missing / at the start of URI");

    if ( _s_.uri.find(' ') != std::string::npos )
        throw std::invalid_argument("Bad request: URI contains space");

    if ( _s_.uri.length() > 1024 )
        throw std::invalid_argument("Bad request: URI length exceeded the limit");
}

void	HTTPRequestParser::decomposeQueryParameters( const std::string& query )
{
    std::string param;
    std::stringstream ss( query );

    while ( std::getline(ss, param, '&') )
	{
        if ( param.empty() ) continue;

        size_t equalPos		=	param.find( '=' );
        std::string key		=	param.substr( 0, equalPos );
        std::string value	=	equalPos != std::string::npos ? param.substr( equalPos + 1 ) : "";

        _s_.queries[key] = value;
    }
}

void 	HTTPRequestParser::extractHttpHeaders()
{
	std::string									line, first, second;
	std::vector<std::string>					header_lines;
	std::vector<std::string>::const_iterator	it;
	std::stringstream							headerStream ( _s_.fullClientRequest );

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
			
			if ( !examinHeaders( first, second ) )
				throw std::invalid_argument( "bad request: Invalid header" );

			_s_.headers[ first ] = second;
		}

		_s_.initialProcessingDone = FALSE;
		_s_.fullClientRequest.clear();
	}
	catch(const std::exception& e)
	{
		throw std::invalid_argument( "Bad request: misconfiguration in header lines" );
	}
}

void	HTTPRequestParser::processChunkedRequestBody()
{
	if ( !_s_.remainingRequestBody.empty() )
	{
		if ( chunkedComplete( _s_.remainingRequestBody ) )
		{
			// send( _s_.clientConnectionSocket, RES_HEADER, strlen( RES_HEADER ), NO_FLAG );
			throw std::invalid_argument( "CHUNK LI DAZ 3LA WJEH HEADER WAS PROCESSED" );
		}
		_s_.remainingRequestBody.clear();
	}
	else
	{
		char buffer[ BUFFER_SIZE ];

		int rc = recv( _s_.clientConnectionSocket, buffer, BUFFER_SIZE - 1, NO_FLAG );
		if ( rc > 0)
		{
			buffer[ rc ] = 0;
			std::string receivedData( buffer, rc );
			if ( chunkedComplete( receivedData ) )
			{
				clientDataFile->close();
				send( _s_.clientConnectionSocket, "FUCK YOU AGAIN", 15, NO_FLAG );
				throw std::invalid_argument( "SARF LI KATSAL HANTA KHDITIH" );
			}
			receivedData.clear();
		}
		else
			throw std::invalid_argument( "END" );
	}
}

long	HTTPRequestParser::parseChunkHeader( std::string& buffer )
{
	std::string	chunkHead;

	chunkHead = buffer.substr( _s_.chunkHeaderStart, buffer.find( "\r\n" ) );
	if ( !chunkHead.empty() )
		_s_.currentChunkSize = std::strtol( chunkHead.c_str(), NULL, 16 );
	else
		throw std::invalid_argument( "Bad request: invalid chunk size header" );
	if ( _s_.currentChunkSize == LONG_MAX || _s_.currentChunkSize == LONG_MIN )
		throw std::invalid_argument( "Bad request: invalid chunk size header" );

	_s_.chunkHeaderEnd = chunkHead.length() + 2;
	buffer = buffer.substr( _s_.chunkHeaderEnd );
	return _s_.currentChunkSize;
}

bool	HTTPRequestParser::chunkedComplete( std::string& buffer )
{
	size_t	bufflen ( buffer.length() );
	long	chunkHeaderStatus;

	while ( bufflen != 0 )
	{
		if ( _s_.isChunkHeader == true )
		{
			_s_.currentChunkSize = parseChunkHeader( buffer );
			if ( _s_.currentChunkSize == 0 )
				return true;
			bufflen = buffer.length();
		}
		if ( _s_.currentChunkSize > buffer.length() )
		{
			clientDataFile->write( buffer.c_str(),  buffer.length() );
			_s_.currentChunkSize -= buffer.length();
			_s_.isChunkHeader = false;
			return ( false );
		}
		else if ( _s_.currentChunkSize <= buffer.length() ) // add equal check it again
		{
			clientDataFile->write( buffer.c_str(),  _s_.currentChunkSize );
			bufflen -= _s_.currentChunkSize + 2;
			buffer = buffer.substr( _s_.currentChunkSize + 2 );
			_s_.isChunkHeader = true;
		}
	}
	return true;
}

void 	HTTPRequestParser::processRegularRequestBody()
{
	int rc;
	char buffer[ BUFFER_SIZE ];

	if ( !_s_.remainingRequestBody.empty() )
	{
		_s_.remainingRequestBody.clear();
	}
	rc = recv( _s_.clientConnectionSocket, buffer, BUFFER_SIZE - 1, NO_FLAG );
	if ( rc > 0 )
	{
		buffer[ rc ] = 0;
	}
	else if ( rc <= 0 )
		throw std::invalid_argument( "End writing to body file or connection closed" );
}

bool	HTTPRequestParser::examinHeaders( std::string first, std::string second )
{
	if ( toUpperCase( first ) == "CONTENT-LENGTH" )
		_s_.requestBodyLength = std::atoi( second.c_str() );
	// std::cout << "first: " << toUpperCase( first ) << " | " << "second: " << toUpperCase( second ) << std::endl;
	if ( toUpperCase( first ) == "TRANSFER-ENCODING" && toUpperCase( second ) == "CHUNKED")
		_s_.chunkedEncoding = TRUE;
	if ( toUpperCase( first ) == "EXPECT" && toUpperCase( second ) == "100-CONTINUE")
		_s_.expectContinueResponse = TRUE;
	if ( toUpperCase( first ) == "CONTENT-TYPE" &&  toUpperCase( second ).find( "MULTIPART/" ) != std::string::npos )
		_s_.multipartEncoding = TRUE;
	if ( ( _s_.requestBodyLength > 0 && _s_.chunkedEncoding ) || ( _s_.chunkedEncoding && _s_.multipartEncoding ) )
		return false;
	
	return true;
}

std::string& HTTPRequestParser::toUpperCase( std::string& str )
{
	for ( int i = 0; i < str.length(); i++  )
	{
		if ( str[ i ] >= 97 && str[ i ] <= 122 )
			str[ i ] -= 32;
	}
	return str;
}

std::string HTTPRequestParser::generateRandomFileName()
{
	const std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::srand( std::time( NULL ) );
	std::string	filename;

	for ( int i = 0; i < 25; i++ )
		filename += CHARACTERS[ rand() % CHARACTERS.length() ];
	return ( filename );
}

void 		HTTPRequestParser::output()
{
	COUT( " ---------------------- Request ---------------------- " );
	std::cout  << _s_.fullClientRequest << std::endl;
	COUT( " -------------------- start line --------------------- " );
	std::cout << "Method: " << _s_.method << " ---- " << "uri: " << _s_.uri << " ---- " << "version: " << _s_.version << std::endl;

	COUT( " ---------------------- Queries ---------------------- " );
	for ( auto &kv : _s_.queries )
		std::cout << "key: " << kv.first << " value: " << kv.second << std::endl;

	COUT( " ---------------------- Headers ---------------------- " );
	for ( auto &kv : _s_.headers )
		std::cout << "key: " << kv.first << " value: " << kv.second << std::endl;
	COUT( " ---------------------- ------- ---------------------- " );
}
