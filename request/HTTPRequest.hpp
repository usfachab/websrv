#include "../include/include.hpp"

class HTTPRequest 
{
public:
    HTTPRequest( int );
    // std::string getMethod() const;
    // std::string getURI() const;
    // std::string getHeader( const std::string& header_name ) const;
    // std::string getBody() const;
	// int			getConnectionStatus();
	~HTTPRequest();
	int getClientSocket()
	{
		return ( clientSocket );
	}

private:
	int clientSocket;
    // std::string uri;
    // std::string body;
    // std::string method;
	// std::string version;
	// std::string	clientRequest;

	// bool		connClosed;
	// int			contentLength;

    // std::map<std::string, std::string> headers;

	// void			appandClientRequest( int );
	// void			startParsingRequest( int );
    // void 			parseMethodAndURI( std::string& );
    // void 			parseHeaders( const std::vector<std::string>& );
    // void 			parseBody( int , size_t );
};
