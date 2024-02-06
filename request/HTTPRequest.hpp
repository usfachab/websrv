#include "../include/include.hpp"

class HTTPRequest 
{
public:
    HTTPRequest( int );
	~HTTPRequest();

    std::string getMethod() const;
    std::string getURI() const;
	std::string getVersion() const;
    std::string getHeader() const;
	bool		getConnectionStatus() const;
	void		startParsingRequest();

private:
    std::string 	method;
    std::string 	uri;
	std::string 	version;
	std::string		clientRequest;
	FILE*			body;

	bool			connStatus;
	bool 			headerEnd;
	size_t			contentLength;
	unsigned int	clientSocket;

    std::map<std::string, std::string> headers;

	void			startParsingHeaders();
    void 			parseMethodAndURI();
    void 			parseHeaders();
    void 			parseBody( size_t );
};
