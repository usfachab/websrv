#include "../include/include.hpp"
#include <string>

class HTTPRequestParser
{
public:
    HTTPRequestParser( int );
	~HTTPRequestParser();

	void			processIncomingRequest();

private:
	HTTPRequestParserStruct _s_;

	std::ofstream	*clientDataFile;
	void			fetchRequestHeader();
    void 			extractMethodAndUri();
	void			validateUriAndExtractQueries();
	void			decomposeQueryParameters( const std::string& query );
    void 			extractHttpHeaders();
	bool			chunkedComplete( std::string& );
	void			processChunkedRequestBody();
	void			processMultipartRequestBody();
    void 			processRegularRequestBody();
	void			validateUri();
	void			output();

	std::string		generateRandomFileName();
	std::string&	toUpperCase( std::string& );
	long			parseChunkHeader( std::string& );
	bool			examinHeaders( std::string , std::string );
};