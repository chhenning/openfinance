#include <string>
#include <fstream>
#include <iostream>


#include <curl\curl.h>

#include <stdio.h>
#include <tidy.h>
#include <tidybuffio.h>

#include <pugixml.hpp>

#include <gtest\gtest.h>


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


TEST(xml_test, download_xml_test)
{
    std::string url = "https://finance.yahoo.com/quote/GM";

    TidyDoc tdoc;
    TidyBuffer docbuf = {0};
    TidyBuffer tidy_errbuf = {0};

    tdoc = tidyCreate();
    tidyOptSetBool(tdoc, TidyXmlOut, yes);
    tidyOptSetBool(tdoc, TidyQuiet, yes);
    tidyOptSetBool(tdoc, TidyNumEntities, yes);
    tidyOptSetBool(tdoc, TidyShowWarnings, no);
    tidyOptSetInt(tdoc, TidyWrapLen, no);
    tidyOptSetBool(tdoc, TidyForceOutput, yes); // try harder

    tidySetErrorBuffer(tdoc, &tidy_errbuf);
    tidyBufInit(&docbuf);



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
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &docbuf);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_to_tidy_buffer);
        
        CURLcode ret = curl_easy_perform(curl);

		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

		if(ret != CURLE_OK)
		{
			error.append(curl_easy_strerror(ret));
		}
		else
		{
            // parse the input
            int err = tidyParseBuffer(tdoc, &docbuf);
            if(err >= 0)
            {
                // fix any problems
                err = tidyCleanAndRepair(tdoc);

                if(err >= 0)
                {
                    tidySaveBuffer(tdoc, &docbuf);
                    if(docbuf.bp)
                    {
                        std::string tidyResult;
                        tidyResult = (char*)docbuf.bp;
                        tidyBufFree(&docbuf);
                    }
                }
            }
		}
       
        curl_easy_cleanup(curl);

        tidyBufFree(&docbuf);
        tidyRelease(tdoc);
    }
}

TEST(xml_test, convert_html_to_xml)
{
    std::ifstream in(".\\test\\BABA.html");
    std::string html;

    in.seekg(0, std::ios::end);   
    html.reserve(in.tellg());
    in.seekg(0, std::ios::beg);

    html.assign((std::istreambuf_iterator<char>(in)),std::istreambuf_iterator<char>());

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
                std::string xhtml;
                xhtml = (char*)docbuf.bp;
                tidyBufFree(&docbuf);

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

