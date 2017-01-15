#pragma once

namespace df3d {

struct HttpResponse
{
    std::string responseData;
    std::string error;
    bool success = false;
};

using HttpResponseCompletitionHandler = std::function<void(const HttpResponse&)>;

class HttpClientImpl;

class HttpClient
{
    unique_ptr<HttpClientImpl> m_pImpl;

public:
    HttpClient();
    ~HttpClient();

    void sendRequest(const std::string &url, HttpResponseCompletitionHandler &&completitionHandler, int timeout);
    void poll();
};

}
