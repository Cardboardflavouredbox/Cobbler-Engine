#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>
#include <curl/curl.h>

#include <sstream>
#include <vector>

#include "network.h"

struct NetworkStuffClass {
  Uint16 PORT = 12093;
  NET_Address* Address;
  NET_DatagramSocket* Socket;

  CURL* curl;
  CURLcode res;
};

NetworkStuffClass* NetStuff;
PostField* curlpostfield;
std::string curlloginstring;
std::string ServerIP = "";
unsigned int ServerPort;
bool IsServer = false;

static size_t CobblerCurlCallback(char* data, size_t size, size_t nmemb,
                                  void* clientp) {
  size_t totalSize = size * nmemb;

  std::ostringstream* stream = (std::ostringstream*)clientp;

  stream->write(data, totalSize);

  return totalSize;
}

bool CobblerSetSocket() {
  NetStuff->Address = NET_ResolveHostname(ServerIP.c_str());
  NetStuff->PORT = ServerPort;
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

bool CobblerClientSendNet(char* name, std::vector<std::byte> buf) {
  std::vector<std::byte> buffer;
  buffer.reserve(256);
  int len = std::strlen(name);
  for (int i = 0; i < len; i++) {
    buffer.push_back(std::byte(name[i]));
  }
  buffer.push_back(std::byte('\0'));
  buffer.insert(buffer.end(), buf.begin(), buf.end());
  return NET_SendDatagram(NetStuff->Socket, NetStuff->Address, NetStuff->PORT,
                          buffer.data(), buffer.size());
}

CobblerNetData* CobblerRecvNet() {
  CobblerNetData* temp = new CobblerNetData();
  NET_Datagram* dgram = NULL;
  if (NET_ReceiveDatagram(NetStuff->Socket, &dgram) && (dgram != NULL)) {
    SDL_Log("SERVER: got %d-byte datagram from %s:%d", (int)dgram->buflen,
            NET_GetAddressString(dgram->addr), (int)dgram->port);
    temp->IP = NET_GetAddressString(dgram->addr);
    temp->PORT = dgram->port;

    int i = 0;
    while (dgram->buf[i] != '\0' && i < dgram->buflen) {
      temp->name.push_back(char(dgram->buf[i]));
      i++;
    }
    i++;
    while (i < dgram->buflen) {
      temp->buffer.push_back(std::byte(dgram->buf[i]));
      i++;
    }
    NET_DestroyDatagram(dgram);
    return temp;
  }
  delete (temp);
  return NULL;
}

void CobblerQuitNet() {
  if (NetStuff->Socket != nullptr) NET_DestroyDatagramSocket(NetStuff->Socket);
  if (NetStuff->Address != nullptr) NET_UnrefAddress(NetStuff->Address);

  if (NetStuff->curl) curl_easy_cleanup(NetStuff->curl);
  curl_global_cleanup();

  if (curlpostfield != nullptr) delete (curlpostfield);

  delete (NetStuff);
  NET_Quit();
}

bool CobblerSendCurlData() {
  curl_easy_setopt(
      NetStuff->curl, CURLOPT_URL,
      ("http://" + curlpostfield->websiteaddr + "/gamedata").c_str());
  std::ostringstream stream;
  curl_easy_setopt(NetStuff->curl, CURLOPT_WRITEDATA, &stream);
  std::string tempstr = curlpostfield->ToField();
  curl_easy_setopt(NetStuff->curl, CURLOPT_POSTFIELDS, tempstr.c_str());

  NetStuff->res = curl_easy_perform(NetStuff->curl);

  if (NetStuff->res != CURLE_OK) {
    SDL_Log("curl_easy_perform() failed: %s\n",
            curl_easy_strerror(NetStuff->res));
    curlpostfield->hasdata = false;
    return false;
  }
  curlpostfield->reset();
  bool result = (stream.str() == "Success");
  if (result)
    SDL_Log("CurlData Sent Successfully");
  else
    SDL_Log("CurlData Send Failed");
  return result;
}

bool CobblerCurlLogin() {
  SDL_Log("logging in...");
  std::ostringstream stream;
  curl_easy_setopt(NetStuff->curl, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(NetStuff->curl, CURLOPT_WRITEFUNCTION, CobblerCurlCallback);
  curl_easy_setopt(NetStuff->curl, CURLOPT_WRITEDATA, &stream);
  curl_easy_setopt(NetStuff->curl, CURLOPT_URL,
                   ("http://" + curlpostfield->websiteaddr + "/login").c_str());
  curl_easy_setopt(NetStuff->curl, CURLOPT_POSTFIELDS, curlloginstring.c_str());

  NetStuff->res = curl_easy_perform(NetStuff->curl);

  if (NetStuff->res != CURLE_OK) {
    SDL_Log("curl_easy_perform() failed: %s\n",
            curl_easy_strerror(NetStuff->res));
    return false;
  }

  bool result = (stream.str() == "Success");
  if (result) {
    curlloginstring.clear();
  }
  return result;
}