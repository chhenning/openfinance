// OpenFinance.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <string>

#include <curl\curl.h>
#include <cpr\cpr.h>

std::string html;

size_t write_callback(void *data, size_t size, size_t nmemb, void *userdata)
{
	html.append(reinterpret_cast<char*>(data), size*nmemb);

	return (size * nmemb);
}


int main()
{
    std::string url = "https://finance.yahoo.com/quote/BABA";

    char curl_errbuf[CURL_ERROR_SIZE];
    CURL *curl = curl_easy_init();
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_errbuf);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        
        CURLcode ret = curl_easy_perform(curl);
        
        curl_easy_cleanup(curl);
    }
    


    return 0;
}

