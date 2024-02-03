#include "../include/include.hpp"

class HTTPRequest 
{
public:
    HTTPRequest( int clientSocket );
    // std::string getMethod() const;
    // std::string getURI() const;
    // std::string getHeader( const std::string& header_name ) const;
    // std::string getBody() const;
	// int			getConnectionStatus();
	~HTTPRequest();

private:
    std::string uri;
    std::string body;
    std::string method;
	std::string version;
	std::string	clientRequest;

	bool		connClosed;
	int			contentLength;

    std::map<std::string, std::string> headers;

	bool			appandClientRequest( int );
	bool			startParsingRequest( int );
    void 			parseMethodAndURI( std::string& );
    bool 			parseHeaders( const std::vector<std::string>& );
    bool 			parseBody( int , size_t );
};
