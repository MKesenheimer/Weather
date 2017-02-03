#include "Curl.h"

//copied and modified from 
//http://stackoverflow.com/questions/1011339/how-do-you-make-a-http-request-with-c

CURLplusplus::CURLplusplus() : curl(curl_easy_init()), http_code(0) {}
   
CURLplusplus::~CURLplusplus() {
    if(curl) curl_easy_cleanup(curl);
}
    
std::string CURLplusplus::get(const std::string& url) {
    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

    ss.str("");
    http_code = 0;
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(res));
    }
        
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    return ss.str();
}
    
long CURLplusplus::getHttpCode() {
    return http_code;
}

size_t CURLplusplus::write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
    return static_cast<CURLplusplus*>(userp)->write(buffer,size,nmemb);
}
    
size_t CURLplusplus::write(void *buffer, size_t size, size_t nmemb) {
    ss.write((const char*)buffer,size*nmemb);
    return size*nmemb;
}