#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>
#include <curl/curl.h>
#include <stdlib.h>

#include "network.h"

struct NetworkStuffClass {
  Uint16 PORT = 12093;
  NET_Address* Address;
  NET_DatagramSocket* Socket;

  CURL* curl;
  CURLcode res;
};

NetworkStuffClass* NetStuff;

static size_t CobblerCurlCallback(char* data, size_t size, size_t nmemb,
                                  char* clientp) {
  size_t realsize = nmemb;

  size_t memsize;

  char* ptr = (char*)realloc(clientp, memsize + realsize + 1);
  if (!ptr) return 0; /* out of memory */

  clientp = ptr;
  memcpy(&(clientp[memsize]), data, realsize);
  memsize += realsize;
  clientp[memsize] = 0;

  return realsize;
}

bool CobblerSetSocket(char* ipaddress, Uint16 port) {
  NetStuff->Address = NET_ResolveHostname(ipaddress);
  NetStuff->PORT = port;
  NetStuff->Socket =
      NET_CreateDatagramSocket(NetStuff->Address, NetStuff->PORT, 0);
  return true;
}

void CobblerDestroySocket() {
  NET_DestroyDatagramSocket(NetStuff->Socket);
  NET_UnrefAddress(NetStuff->Address);
}

bool CobblerInitNet() {
  if (!NET_Init()) return false;
  NetStuff = new NetworkStuffClass();

  curl_global_init(CURL_GLOBAL_ALL);

  NetStuff->curl = curl_easy_init();

  curl_easy_setopt(NetStuff->curl, CURLOPT_COOKIEFILE, "");

  // curl_easy_setopt(NetStuff->curl, CURLOPT_COOKIEFILE, MyCookieFileName);
  // curl_easy_setopt(NetStuff->curl, CURLOPT_COOKIEJAR, MyCookieFileName);

  return true;
}

void CobblerQuitNet() {
  if (NetStuff->Socket != nullptr) NET_DestroyDatagramSocket(NetStuff->Socket);
  if (NetStuff->Address != nullptr) NET_UnrefAddress(NetStuff->Address);

  if (NetStuff->curl) curl_easy_cleanup(NetStuff->curl);
  curl_global_cleanup();

  delete (NetStuff);
  NET_Quit();
}

bool CobblerSendCurlData(char postfields[]) {
  curl_easy_setopt(NetStuff->curl, CURLOPT_URL,
                   "http://127.0.0.1:5000/gamedata");
  curl_easy_setopt(NetStuff->curl, CURLOPT_POSTFIELDS, postfields);

  NetStuff->res = curl_easy_perform(NetStuff->curl);

  if (NetStuff->res != CURLE_OK) {
    SDL_Log("curl_easy_perform() failed: %s\n",
            curl_easy_strerror(NetStuff->res));
    return false;
  }
  return true;
}

bool CobblerCurlLogin(char postfields[]) {
  char* response;
  curl_easy_setopt(NetStuff->curl, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(NetStuff->curl, CURLOPT_WRITEFUNCTION, CobblerCurlCallback);
  curl_easy_setopt(NetStuff->curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(NetStuff->curl, CURLOPT_URL, "http://127.0.0.1:5000/login");
  curl_easy_setopt(NetStuff->curl, CURLOPT_POSTFIELDS, postfields);

  NetStuff->res = curl_easy_perform(NetStuff->curl);

  if (NetStuff->res != CURLE_OK) {
    SDL_Log("curl_easy_perform() failed: %s\n",
            curl_easy_strerror(NetStuff->res));
    free(response);
    return false;
  }

  bool result = (response[0] == 'S');
  free(response);
  return result;
}