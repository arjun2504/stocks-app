// StockApp1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "iostream"
#include "string"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#define CURL_STATICLIB
#include "curl/curl.h"

using namespace rapidjson;

#ifdef _DEBUG
#	pragma comment (lib, "curl/libcurl_a_debug.lib")
#else
#	pragma comment (lib, "curl/libcurl_a.lib")
#endif

static int content_writer(char *data, size_t size, size_t nmemb, std::string *writerData)
{
	if (writerData == NULL)
		return 0;

	writerData->append(data, size*nmemb);

	return size * nmemb;
}

int main()
{
	std::string content;
	curl_global_init(CURL_GLOBAL_ALL);
	CURL *curl = nullptr;
	Document d;
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");

	curl = curl_easy_init();

	if (curl) {
		std::cout << "Fetching data...";
		curl_easy_setopt(curl, CURLOPT_URL, "http://quantprice.herokuapp.com/api/v1.1/scoop/period?tickers=MSFT&nrgin=2012-02-19&end=2012-02-20");
		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, content_writer);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);

		CURLcode res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);


		d.Parse(content.c_str());
		

		std::cout << "\n\n";
		int column_size = d["datatable"]["columns"].Size();
		for (int i = 0; i < column_size; i++) {
			StringBuffer buffer;
			Writer<StringBuffer> writer(buffer);
			d["datatable"]["columns"][i]["name"].Accept(writer);
			std::cout << buffer.GetString();
			std::cout << "\t";
		}
		std::cout << "\n---------------------------------------------------------------------------------------------------------------------------------------------\n";
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < column_size; j++) {
				StringBuffer buffer;
				Writer<StringBuffer> writer(buffer);
				d["datatable"]["data"][i][j].Accept(writer);
				std::cout << buffer.GetString();
				std::cout << "\t";
			}
			std::cout << "\n";
		}
	}

	curl_slist_free_all(headers);

	//std::cout << content;

	std::cout << "\n\nPress any key to exit";
	std::cin.get();
    return EXIT_SUCCESS;
}