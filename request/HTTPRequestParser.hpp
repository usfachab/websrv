#include "../include/include.hpp"
#include <fstream>

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
	void			processChunkedRequestBody();
	bool			chunkedComplete( std::string& );
    void 			processRegularRequestBody();
	void			validateUri();
	// void			output();

	std::string		generateRandomFileName();
	long			parseChunkHeader( std::string& );
};