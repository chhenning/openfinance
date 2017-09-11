// OpenFinance.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <string>
#include <fstream>
#include <iostream>
#include <curl\curl.h>
#include <pugixml.hpp>

#include <stdio.h>
#include <tidy.h>
#include <tidybuffio.h>

std::string html;

size_t write_callback_to_string(void *data, size_t size, size_t nmemb, void *userdata)
{
	html.append(reinterpret_cast<char*>(data), size*nmemb);

	return (size * nmemb);
}

uint write_callback_to_tidy_buffer(char *in, uint size, uint nmemb, TidyBuffer *out)
{
    uint r;
    r = size * nmemb;
    tidyBufAppend(out, in, r);
    return r;
}

void a()
{
    TidyDoc tdoc;
    tdoc = tidyCreate();
    tidyOptSetBool(tdoc, TidyXhtmlOut, yes);
    tidyOptSetInt(tdoc, TidyDoctype, TidyDoctypeHtml5);
    tidyOptSetBool(tdoc, TidyDropEmptyParas, yes);

    tidyOptSetBool(tdoc, TidyFixBackslash, yes);
    tidyOptSetBool(tdoc, TidyFixComments, yes);
    tidyOptSetBool(tdoc, TidyFixUri, yes);
    tidyOptSetBool(tdoc, TidyJoinStyles, yes);
    tidyOptSetBool(tdoc, TidyLowerLiterals, yes);
    tidyOptSetBool(tdoc, TidyNCR, yes);
    tidyOptSetBool(tdoc, TidyQuoteAmpersand, yes);
    tidyOptSetBool(tdoc, TidyQuoteNbsp, yes);

    tidyOptSetInt(tdoc, TidyCharEncoding, TidyEncUtf8);

    tidyOptSetBool(tdoc, TidyQuiet, yes);
    tidyOptSetBool(tdoc, TidyNumEntities, yes);
    tidyOptSetBool(tdoc, TidyShowWarnings, no);
    tidyOptSetInt(tdoc, TidyWrapLen, no);
    tidyOptSetBool(tdoc, TidyForceOutput, yes); // try harder

    TidyBuffer input_buffer = {0};
    tidyBufInit(&input_buffer);

    TidyBuffer output_buffer = {0};
    tidyBufInit(&output_buffer);



    char curl_errbuf[CURL_ERROR_SIZE];
    CURL *curl = curl_easy_init();
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "https://finance.yahoo.com/quote/GM");
#ifdef _WIN32
        curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");
#endif
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_errbuf);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &input_buffer);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_to_tidy_buffer);
        //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_to_string);
        
        CURLcode ret = curl_easy_perform(curl);

        int http_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

		if(ret != CURLE_OK)
		{
			std::cerr << curl_easy_strerror(ret) << std::endl;
		}
		else
		{
            // parse the input
            //int err = tidyParseString(tdoc, html.c_str());
            int err = tidyParseBuffer(tdoc, &input_buffer);
            if(err >= 0)
            {
                // fix any problems
                err = tidyCleanAndRepair(tdoc);

                if(err >= 0)
                {
                    tidySaveBuffer(tdoc, &output_buffer);
                    if(output_buffer.bp)
                    {
                        std::string xhtml((char*)output_buffer.bp);
                        size_t len = xhtml.length();


                        pugi::xml_document xmlDoc;
                        pugi::xml_parse_result result = xmlDoc.load_buffer(xhtml.c_str(), xhtml.size());

                        if (result)
                        {
                            std::cout << "XML [" << xhtml << "] parsed without errors, attr value: [" << xmlDoc.child("node").attribute("attr").value() << "]\n\n";
                        }
                        else
                        {
                            //std::cout << "parsed with errors, attr value: [" << xmlDoc.child("node").attribute("attr").value() << "]\n";
                            //std::cout << "Error description: " << result.description() << "\n";
        
                            auto off = (xhtml.substr(result.offset));
                            std::cout << "Error offset: " << result.offset << " (error at [..." << off << "]\n\n";
                        }
                    }
                }
            }
		}
       
        curl_easy_cleanup(curl);

        tidyBufFree(&input_buffer);
        tidyBufFree(&output_buffer);
        tidyRelease(tdoc);
    }
}

void b()
{
    TidyDoc tdoc;
    tdoc = tidyCreate();
    tidyOptSetBool(tdoc, TidyXhtmlOut, yes);
    tidyOptSetInt(tdoc, TidyDoctype, TidyDoctypeHtml5);
    tidyOptSetBool(tdoc, TidyDropEmptyParas, yes);

    tidyOptSetBool(tdoc, TidyFixBackslash, yes);
    tidyOptSetBool(tdoc, TidyFixComments, yes);
    tidyOptSetBool(tdoc, TidyFixUri, yes);
    tidyOptSetBool(tdoc, TidyJoinStyles, yes);
    tidyOptSetBool(tdoc, TidyLowerLiterals, yes);
    tidyOptSetBool(tdoc, TidyNCR, yes);
    tidyOptSetBool(tdoc, TidyQuoteAmpersand, yes);
    tidyOptSetBool(tdoc, TidyQuoteNbsp, yes);

    tidyOptSetInt(tdoc, TidyCharEncoding, TidyEncUtf8);

    tidyOptSetBool(tdoc, TidyQuiet, yes);
    tidyOptSetBool(tdoc, TidyNumEntities, yes);
    tidyOptSetBool(tdoc, TidyShowWarnings, no);
    tidyOptSetInt(tdoc, TidyWrapLen, no);
    tidyOptSetBool(tdoc, TidyForceOutput, yes); // try harder

    TidyBuffer docbuf = {0};
    tidyBufInit(&docbuf);


    char curl_errbuf[CURL_ERROR_SIZE];
    CURL *curl = curl_easy_init();
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "https://finance.yahoo.com/quote/GM");
#ifdef _WIN32
        curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");
#endif
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_errbuf);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_to_string);
        
        CURLcode ret = curl_easy_perform(curl);

        int http_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

		if(ret != CURLE_OK)
		{
			std::cerr << curl_easy_strerror(ret) << std::endl;
		}
		else
		{
            int err = tidyParseString(tdoc, html.c_str());
            if(err >= 0)
            {
                // fix any problems
                err = tidyCleanAndRepair(tdoc);

                if(err >= 0)
                {
                    tidySaveBuffer(tdoc, &docbuf);
                    if(docbuf.bp)
                    {
                        std::string xhtml((char*)docbuf.bp);
                        size_t len = xhtml.length();


                        pugi::xml_document xmlDoc;
                        pugi::xml_parse_result result = xmlDoc.load_buffer(xhtml.c_str(), xhtml.size());

                        if (result)
                        {
                            std::cout << "XML [" << xhtml << "] parsed without errors, attr value: [" << xmlDoc.child("node").attribute("attr").value() << "]\n\n";
                        }
                        else
                        {
                            std::cout << "parsed with errors, attr value: [" << xmlDoc.child("node").attribute("attr").value() << "]\n";
                            std::cout << "Error description: " << result.description() << "\n";
        
                            auto off = (xhtml.substr(result.offset));
                            std::cout << "Error offset: " << result.offset << " (error at [..." << off << "]\n\n";
                        }
                    }
                }
            }
		}
       
        curl_easy_cleanup(curl);

        tidyBufFree(&docbuf);
        tidyRelease(tdoc);
    }
}

int main()
{
    a();
    b();




    
    return 0; 
}
