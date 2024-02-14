#include "../include/include.hpp"

class HTTPRequestParser 
{
public:
    HTTPRequestParser( int );
	~HTTPRequestParser();

	void			processIncomingRequest();

private:
	HTTPRequestParserStruct _s_;

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
};