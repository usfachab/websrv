#include "../include/include.hpp"

class HTTPRequest 
{
public:
    HTTPRequest( int );
	~HTTPRequest();

    std::string getMethod() const;
    std::string getURI() const;
	std::string getVersion() const;
	std::string	fileNameGen() const;
    std::string getHeaders( const std::string& ) const;
	bool		getConnectionStatus() const;
	void		startParsingRequest();
private:
    std::string 	method;
    std::string 	uri;
	std::string 	version;
	std::string		clientRequest;
	int				bodyFile;


	bool			connStatus;
	bool 			headerEnd;
	unsigned int	clientSocket;

	size_t			contentLength;
	size_t 			npos;
    std::map<std::string, std::string> headers;
	std::map<std::string, std::string> queries;

    void 			parseMethodAndURI();
    void 			parseHeaders();
	void			validateUriAndParseQueries();
    void 			parseBody( size_t );
};