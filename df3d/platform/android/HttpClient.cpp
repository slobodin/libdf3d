#include <df3d/engine/network/HttpClient.h>

namespace df3d {

class HttpClientImpl
{

};

HttpClient::HttpClient()
{

}

HttpClient::~HttpClient()
{

}

void HttpClient::sendRequest(const std::string &url, HttpResponseCompletitionHandler &&completitionHandler, int timeout)
{
    completitionHandler({});
}

void HttpClient::poll()
{

}

}
