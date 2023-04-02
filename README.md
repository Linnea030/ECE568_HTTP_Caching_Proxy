# HTTP_Caching_Proxy

For this assignment you will be writing an http proxy – a server whose job it is to forward requests to the origin server on behalf of the client. Your proxy will cache responses, and, when appropriate, respond with the cached copy of a resource rather than re-fetching it.

While the HTTP specification is quite large (and includes many complex features), you should make an http proxy which functions with GET, POST, and CONNECT. (You MAY handle any of the other request methods if you want). Specifically, a user should be able to configure their browser to use your proxy, and browse typical webpages (e.g., perform a Google Search, view the results, etc). Note that many webpages only do HTTPS, so your browser will use CONNECT to communicate with them (and you won’t see the actual GET requests).
