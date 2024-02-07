#include "../include/include.hpp"

class HTTPResponse {
public:
    HTTPResponse( int clientSocket );
    void setStatusCode( int code );
    // void setReasonPhrase(const std::string& phrase);
    void addHeader( const std::string& key, const std::string& value );
    void setBody( const std::string& body );

    void sendResponse();

private:
    int clientSocket;  // Socket to send the response to
    int statusCode;    // HTTP status code (e.g., 200, 404)
    std::string reasonPhrase;  // Reason phrase corresponding to the status code
    std::map<std::string, std::string> headers;  // HTTP headers
    std::string body;  // Response body

    std::string formatResponse();
};