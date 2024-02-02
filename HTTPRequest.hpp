#include "include.hpp"

class HTTPRequest 
{
public:
    HTTPRequest( int clientSocket );
    // std::string getMethod() const;
    // std::string getURI() const;
    // std::string getHeader( const std::string& header_name ) const;
    // std::string getBody() const;
	// int			getConnectionStatus();

private:
	int connClosed;
    std::string method;
    std::string uri;
	std::string version;
    std::map<std::string, std::string> headers;
    std::string body;

    void parseRequest( int clientSocket );
    void parseMethodAndURI( const std::string& request_line );
    void parseHeaders( const std::vector<std::string>& header_lines );
    // void parseBody(int clientSocket, size_t content_length);
};
