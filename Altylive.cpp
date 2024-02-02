#include <iostream>
#include <curl/curl.h>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <nlohmann/json.hpp>
#include <cstdio>

using json = nlohmann::json;

#define MAX_RETRIES 10

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::ofstream* file = static_cast<std::ofstream*>(userp);
    size_t totalSize = size * nmemb;

    if (file) {
        file->write(static_cast<char*>(contents), totalSize);
        return totalSize;
    }
    return 0;
}

int FetchInfo(struct curl_slist *headers){
    CURL *getinfo = curl_easy_init();
    if (getinfo){
        curl_easy_setopt(getinfo, CURLOPT_URL, "https://app.ofppt-langues.ma/gw//api/account");
        curl_easy_setopt(getinfo, CURLOPT_HTTPHEADER, headers);
        std::ofstream outputFile("info.json", std::ofstream::binary);
        curl_easy_setopt(getinfo, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(getinfo, CURLOPT_WRITEDATA, &outputFile);
        curl_easy_setopt(getinfo, CURLOPT_ACCEPT_ENCODING, "gzip, deflate");
        CURLcode res = curl_easy_perform(getinfo);
        if (res != CURLE_OK){
            std::cerr << "Error nit" << curl_easy_strerror(res) << std::endl;
        }
        outputFile.close();
    }
    std::ifstream lifile("info.json");
    if (!lifile.is_open()){
        std::cerr <<  "Error opening lifile" << std::endl;
    }
    std::string jsonString((std::istreambuf_iterator<char>(lifile)), std::istreambuf_iterator<char>());
    auto parsedJson = json::parse(jsonString);
    std::string firstName = parsedJson["firstName"];
    std::string lastName = parsedJson["lastName"];
    std::string email = parsedJson["email"];
    std::cout << firstName << std::endl;
    std::cout << lastName << std::endl;
    std::cout << email << std::endl;
    lifile.close();
    return 0;
}
int FetchID(struct curl_slist* headers){
    CURL *getid = curl_easy_init();
    if (getid){
        curl_easy_setopt(getid, CURLOPT_URL, "https://app.ofppt-langues.ma/gw//lcapi/main/api/lc/user-configurations/full");
        curl_easy_setopt(getid, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(getid, CURLOPT_ACCEPT_ENCODING, "gzip, deflate");
        std::ofstream outputFile("licence.json", std::ofstream::binary);
        curl_easy_setopt(getid, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(getid, CURLOPT_WRITEDATA, &outputFile);
        CURLcode res = curl_easy_perform(getid);
        if (res != CURLE_OK){
            std::cerr << "Error nit" << curl_easy_strerror(res) << std::endl;
        }
        outputFile.close();
    }
    std::ifstream lifile("licence.json");
    if (!lifile.is_open()){
        std::cerr <<  "Error opening lifile" << std::endl;
    }
    std::string jsonString((std::istreambuf_iterator<char>(lifile)), std::istreambuf_iterator<char>());
    auto parsedJson = json::parse(jsonString);
    int courseLicenseId = parsedJson["studyLanguages"][0]["courseLicenseId"];
    lifile.close();
    std::remove("licence.json");
    return courseLicenseId;
    return 0;
}
int KeepAlive(CURL *alty,struct curl_slist *headers, const char* Authorization, int licenceid){     
    if (alty){
        curl_easy_setopt(alty, CURLOPT_URL, "https://app.ofppt-langues.ma/gw//eventapi/main/api/event/internal/events");
        headers = curl_slist_append(headers, Authorization);
        curl_easy_setopt(alty, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(alty, CURLOPT_POST, 1);
        const char* postfieldtmp = "{\"licenseId\":%d,\"studyLg\":\"fr_FR\",\"action\":\"lc.application.alive\"}";
        char postData[100];
        snprintf(postData, sizeof(postData), postfieldtmp, licenceid);
        curl_easy_setopt(alty, CURLOPT_POSTFIELDS, postData);
        CURLcode res = curl_easy_perform(alty);
        if (res != CURLE_OK){
            std::cerr << "Faild to send" << std::endl;
        }
    }
    sleep(5);
    return 0;
}

int main(){
    CURL *ez = curl_easy_init();
    if (!ez){
        std::cerr << "Unable to start handle !" << std::endl;
        return -2;
    }
    size_t uuid_len = std::strlen("x-device-uuid: ")  + std::strlen("0b5624e4-51dc-415f-832a-786754f0079b") + 1;
    char * device_uuid = new char[uuid_len];
    std::snprintf(device_uuid, uuid_len, "x-device-uuid: %s", "0b5624e4-51dc-415f-832a-786754f0079b");
        
    size_t token_len = std::strlen("x-altissia-token: ") + std::strlen("efbbc36df97097f6d4df5bb19e03507cf967e0537177a79bd621ccaab3f2b5b7") + 1;
    char * xalrissiatoken = new char[token_len];
    std::snprintf(xalrissiatoken, token_len, "x-altissia-token: %s", "efbbc36df97097f6d4df5bb19e03507cf967e0537177a79bd621ccaab3f2b5b7");

    size_t cookie_len = std::strlen("Cookie: ") + std::strlen(device_uuid) + 1;
    char * cookie = new char[cookie_len];
    std::snprintf(cookie, cookie_len, "Cookie: %s", device_uuid);
    
    size_t auth_len = std::strlen("Authorization: ") + std::strlen("efbbc36df97097f6d4df5bb19e03507cf967e0537177a79bd621ccaab3f2b5b7") + 1;
    char * Authorization = new char[auth_len];
    std::snprintf(Authorization, auth_len, "Authorization:  %s", "efbbc36df97097f6d4df5bb19e03507cf967e0537177a79bd621ccaab3f2b5b7");

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Host: app.ofppt-langues.ma");
    headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:109.0) Gecko/20100101 Firefox/111.0");
    headers = curl_slist_append(headers, "Accept: application/json, text/plain, */*");
    headers = curl_slist_append(headers, "Accept-Language: en-US,en;q=0.5");
    headers = curl_slist_append(headers, "Accept-Encoding: gzip, deflate, br");
    headers = curl_slist_append(headers, "Referer: https://app.ofppt-langues.ma/platform/");
    headers = curl_slist_append(headers, device_uuid);
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, xalrissiatoken);
    headers = curl_slist_append(headers, "Connection: keep-alive");
    headers = curl_slist_append(headers, cookie);
    headers = curl_slist_append(headers, "Sec-Fetch-Dest: empty");
    headers = curl_slist_append(headers, "Sec-Fetch-Mode: cors");
    headers = curl_slist_append(headers, "Sec-Fetch-Site: same-origin");
    headers = curl_slist_append(headers, "TE: trailers");
    FetchInfo(headers);
    int licenceID = FetchID(headers);
    for (int i = 1; i <= 10; i++){
        KeepAlive(ez, headers, Authorization, licenceID);
        std::cout << "Attempt number " << i << std::endl;
    }
    delete[] device_uuid;
    delete[] Authorization;
    delete[] xalrissiatoken;
    delete[] cookie;
    curl_easy_cleanup(ez);
    curl_global_cleanup();
    return 0;
}
