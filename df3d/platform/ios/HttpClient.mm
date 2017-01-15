#import <Foundation/Foundation.h>
#import <df3d/Common.h>
#import <df3d/engine/network/HttpClient.h>
#import <df3d/lib/containers/ConcurrentQueue.h>

namespace df3d {

class HttpClientImpl
{
public:
    struct Request
    {
        std::string url;
        int timeout;
        HttpResponseCompletitionHandler completitionHandler;

        HttpResponse response;
    };

private:
    ConcurrentQueue<shared_ptr<Request>> m_processedRequests;

public:
    void sendRequest(shared_ptr<Request> request)
    {
        NSString *urlStr = [NSString stringWithUTF8String:request->url.c_str()];
        NSURLRequest *nsReq = [NSURLRequest requestWithURL:[NSURL URLWithString:urlStr]
                                               cachePolicy:NSURLRequestReloadIgnoringLocalCacheData
                                           timeoutInterval:request->timeout];

        auto completitionHandler = [this, request](NSURLResponse *nsResp, NSData *data, NSError *error) {
            HttpResponse response;

            if (error == nil)
            {
                response.success = true;
                response.responseData.assign(reinterpret_cast<const char*>(data.bytes), data.length);
            }
            else
            {
                NSLog(@"Network error: failed to load %@", [[nsResp URL] absoluteString]);
                NSLog(@"%@", [error localizedDescription]);

                response.success = false;
            }

            request->response = std::move(response);
            m_processedRequests.push(request);
        };

        [NSURLConnection sendAsynchronousRequest:nsReq queue:[NSOperationQueue mainQueue] completionHandler:completitionHandler];
    }

    void poll()
    {
        while (!m_processedRequests.empty())
        {
            auto req = m_processedRequests.pop();
            req->completitionHandler(req->response);
        }
    }
};

HttpClient::HttpClient()
    : m_pImpl(make_unique<HttpClientImpl>())
{

}

HttpClient::~HttpClient()
{

}

void HttpClient::sendRequest(const std::string &url, HttpResponseCompletitionHandler &&completitionHandler, int timeout)
{
    auto request = make_shared<HttpClientImpl::Request>();
    request->timeout = timeout;
    request->url = url;
    request->completitionHandler = std::move(completitionHandler);

    m_pImpl->sendRequest(request);
}

void HttpClient::poll()
{
    m_pImpl->poll();
}

}
