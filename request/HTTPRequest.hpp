#include "../include/include.hpp"

class HTTPRequest 
{
public:
    HTTPRequest( int );
	~HTTPRequest();

    std::string getURI() const;
    std::string getMethod() const;
	std::string getVersion() const;
    std::string getHeaders( const std::string& ) const;

	bool		getConnectionStatus() const;
	void		startParsingRequest();
private:
    std::string 	uri;
    std::string 	method;
	std::string 	version;
	std::string		bodYrest;
	std::string		clientRequest;
	int				bodyFile;
	int				clientSocket;
	bool 			headerEnd;
	bool			connStatus;
	bool			chunkedEncoding;
	bool			ignoreBody;
	size_t 			npos;
	size_t			contentLength;
    std::map<std::string, std::string> headers;
	std::map<std::string, std::string> queries;

    void 			parseHeaders();
	void			receiveHeader();
	void			validateUri();
    void 			parseMethodAndURI();
    void 			parseBody( size_t );
	void			validateUriAndParseQueries();
	void			splitAndStoreQueries( const std::string& );

	std::string		randomFileNameGen();
	void			output();
};