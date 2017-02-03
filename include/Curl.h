#include <curl/curl.h>
#include <sstream>
#include <stdexcept>

#ifndef CURL_H
#define CURL_H

class CURLplusplus {
  
  private:
    CURL* curl;
    std::stringstream ss;
    long http_code;
  
  public:
    CURLplusplus();   
    ~CURLplusplus();
    std::string get(const std::string& url);
    long getHttpCode();

  private:
    static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);    
    size_t write(void *buffer, size_t size, size_t nmemb);

};

#endif
