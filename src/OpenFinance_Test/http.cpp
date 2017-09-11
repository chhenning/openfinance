
#include <gtest\gtest.h>

#include <curl\curl.h>

std::string html;
std::string error;

int64_t http_code = 0;

std::string destination_ip;

double total_time;
double name_lookup_time;
double connect_time;
double app_connect_time;
double pre_transfer_time;
double start_transfer_time;
double redirect_time;
int redirect_count;

size_t write_callback_to_string(void *data, size_t size, size_t nmemb, void *userdata)
{
	html.append(reinterpret_cast<char*>(data), size*nmemb);

	return (size * nmemb);
}

TEST(MonitorTests, download_test)
{
    std::string url = "https://finance.yahoo.com/quote/BABA";

    char curl_errbuf[CURL_ERROR_SIZE];
    CURL *curl = curl_easy_init();
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
#ifdef _WIN32
        curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");
#endif
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_errbuf);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_to_string);
        
        CURLcode ret = curl_easy_perform(curl);

		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

		if(ret != CURLE_OK)
		{
			error.append(curl_easy_strerror(ret));
		}
		else
		{
			curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time);
			curl_easy_getinfo(curl, CURLINFO_NAMELOOKUP_TIME, &name_lookup_time);
			curl_easy_getinfo(curl, CURLINFO_CONNECT_TIME, &connect_time);
			curl_easy_getinfo(curl, CURLINFO_APPCONNECT_TIME, &app_connect_time);
			curl_easy_getinfo(curl, CURLINFO_PRETRANSFER_TIME, &pre_transfer_time);
			curl_easy_getinfo(curl, CURLINFO_STARTTRANSFER_TIME, &start_transfer_time);
			curl_easy_getinfo(curl, CURLINFO_REDIRECT_TIME, &redirect_time);
			curl_easy_getinfo(curl, CURLINFO_REDIRECT_COUNT, &redirect_count);

			// will be cleaned by curl_easy_cleanup
			char* ip;
            curl_easy_getinfo(curl, CURLINFO_PRIMARY_IP, &ip);
			destination_ip = std::string(ip);

		}
       
        curl_easy_cleanup(curl);
    }
}