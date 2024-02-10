#include "../include/include.hpp"

class HTTPRequest 
{
public:
    HTTPRequest( int );
	~HTTPRequest();

	bool			getConnectionStatus() const;
	void			startParsingRequest();
    std::string 	getURI() const;
    std::string 	getMethod() const;
	std::string 	getVersion() const;
    std::string 	getHeaders( const std::string& ) const;
private:
	HTTPRequestStruct crs;

    void 			regularBody();
	void			chunkedBody();
	void			validateUri();
    void 			parseHeaders();
	void			receiveHeader();
    void 			parseMethodAndURI();
	void			validateUriAndParseQueries();
	void			splitAndStoreQueries( const std::string& );
	void			startParsingBodies();
	std::string		randomFileNameGen();
	void			output();

	void			handleBodYrest();
};