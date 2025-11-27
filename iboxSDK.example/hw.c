#include "hw.h"

/*
 *
 * SERIAL CONNECTION
 *
 */

char *sendData(char *buf, int size, uint16_t *retsize)
{
	serial_t serial;
	int ret;

	char *retbuf = 0;

	if (serial_open(&serial, "/dev/ttyACM0", 115200) < 0)
	{
		fprintf(stderr, "serial_open(): %s\n", serial_errmsg(&serial));
		retbuf = 0;
	}
	else
	{
		if (serial_write(&serial, buf, size) < 0)
		{
			fprintf(stderr, "serial_write(): %s\n", serial_errmsg(&serial));
			retbuf = 0;
		}
		else
		{
			char *res = calloc(1024, sizeof(char));
			if ((ret = serial_read(&serial, res, sizeof(char) * 1024, 1000)) < 0)	// 500
			{
				fprintf(stderr, "serial_read(): %s\n", serial_errmsg(&serial));
				retbuf = 0;
			}
			else
			{
				*retsize = ret;
				retbuf = calloc(ret, sizeof(char));
				memcpy(retbuf, res, ret);
			}
			free(res);
		}
	}

	serial_close(&serial);
	return retbuf;
}

/*
 *
 * NETWORK CONNECTION
 *
 */

CURL *curl;
CURLcode res;

typedef struct {
	char *memory;
	size_t size;
} MemoryStruct;

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	MemoryStruct *mem = (MemoryStruct*) userp;

	mem->memory = realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory == NULL)
	{
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

MemoryStruct *sendRequest(const char *url, const char *postdata)
{
	//printf("REQUEST: %s\n%s\n", url, postdata);
	CURL *curl_handle;
	CURLcode res;

	MemoryStruct *chunk = malloc(sizeof(MemoryStruct));

	chunk->memory = malloc(1);
	chunk->size = 0;

	curl_global_init(CURL_GLOBAL_ALL);
	curl_handle = curl_easy_init();

	//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata);
	//curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(postdata));

	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
	curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, chunk);
	curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 15);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 30);

	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
	curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);

	curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, postdata);

	res = curl_easy_perform(curl_handle);

	curl_slist_free_all(headers);
	curl_easy_cleanup(curl_handle);
	curl_global_cleanup();

	if (res != CURLE_OK)
	{
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                if (chunk->memory)
                   free (chunk->memory);
                free (chunk);
		return 0;
	}
	else
	{
		return chunk;
	}

}

Ibox_MemoryStruct *sendWebRequest(const char *request, const char *url)
{
	MemoryStruct *chunk = sendRequest(url, request);

	Ibox_MemoryStruct *response = calloc(1, sizeof(Ibox_MemoryStruct));
	response->length = chunk ? chunk->size : 0;
	response->data = chunk ? chunk->memory : NULL;

    if (chunk)
		free (chunk);

	return response;
}

Ibox_MemoryStruct *sendReaderRequest(char *request, int length)
{
	Ibox_MemoryStruct *response = NULL;

	uint16_t responseLength = 0;
	char *responseData = sendData(request, length, &responseLength);
	if (responseData)
	{
		response = calloc(1, sizeof(Ibox_MemoryStruct));
		response->length = responseLength;
		response->data = responseData;
	}

	return response;
}
