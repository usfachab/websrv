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
	int				bodyOutFile;
	int				clientSocket;
	bool 			headerEnd;
	bool			connStatus;
	bool			chunkedEncoding;
	bool			ignoreBody;
	size_t			contentLength;
	size_t 			npos;
    std::string 	uri, method, version;
	std::string		bodYrest;
	std::string		clientRequest;
    std::map<std::string, std::string> headers, queries;

    void 			parseHeaders();
	void			receiveHeader();
	void			validateUri();
    void 			parseMethodAndURI();
    void 			parseBody();
	void			validateUriAndParseQueries();
	void			splitAndStoreQueries( const std::string& );

	std::string		randomFileNameGen();
	void			output();
};