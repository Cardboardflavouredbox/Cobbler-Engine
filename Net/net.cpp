#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>
#include <curl/curl.h>

#include <deque>
#include <sstream>
#include <vector>

#include "network.h"

struct NetworkStuffClass {
  std::vector<Uint16> PORT;
  std::vector<NET_Address*> Addresses;
  NET_DatagramSocket* Socket;

  CURL* curl;
  CURLcode res;
};

NetworkStuffClass* NetStuff;
PostField* curlpostfield;
std::string curlloginstring;
std::vector<Uint8> packetbuffer;

bool IsServer = false;

static size_t CobblerCurlCallback(char* data, size_t size, size_t nmemb,
                                  void* clientp) {
  size_t totalSize = size * nmemb;

  std::ostringstream* stream = (std::ostringstream*)clientp;

  stream->write(data, totalSize);

  return totalSize;
}

void CobblerAddIP(std::string IP, unsigned int Port) {
  NetStuff->Addresses.push_back(NET_ResolveHostname(IP.c_str()));
  NetStuff->PORT.push_back(Port);
  SDL_Log("%d", NetStuff->Addresses.size());
}

bool CobblerSetSocket(unsigned int port) {
  SDL_PropertiesID props = SDL_CreateProperties();
  SDL_SetBooleanProperty(props, NET_PROP_DATAGRAM_SOCKET_REUSEADDR_BOOLEAN,
                         true);
  NetStuff->Socket = NET_CreateDatagramSocket(NULL, port, 0);
  SDL_DestroyProperties(props);
  if (NetStuff->Socket == NULL) {
    SDL_Log("%s", SDL_GetError());
    return false;
  }
  return true;
}

void CobblerDestroySocket() { NET_DestroyDatagramSocket(NetStuff->Socket); }

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

bool CobblerQueueData(const char* name, std::vector<Uint8> buf) {
  int len = std::strlen(name);
  for (int i = 0; i < len; i++) {
    packetbuffer.push_back(Uint8(name[i]));
  }
  packetbuffer.push_back(Uint8('\0'));

  unsigned int buflen = buf.size();

  if (buflen > 255) {
    SDL_Log("buffer too long!");
    return false;
  }

  unsigned char templen = buflen;

  packetbuffer.push_back(static_cast<Uint8>(templen));

  packetbuffer.insert(packetbuffer.end(), buf.begin(), buf.begin() + templen);
  return true;
}

bool CobblerSendNet() {
  for (int i = 0; i < NetStuff->Addresses.size(); i++) {
    if (!NET_SendDatagram(NetStuff->Socket, NetStuff->Addresses[i],
                          NetStuff->PORT[i], packetbuffer.data(),
                          packetbuffer.size())) {
      SDL_Log("%s", SDL_GetError());
    }
  }
  packetbuffer.clear();
  return true;
}

std::vector<CobblerNetData>* CobblerRecvNet() {
  NET_Datagram* dgram = NULL;
  if (NET_ReceiveDatagram(NetStuff->Socket, &dgram)) {
    if (dgram != NULL) {
      std::vector<CobblerNetData>* tempvec = new std::vector<CobblerNetData>();

      // SDL_Log("SERVER: got %d-byte datagram from %s:%d", (int)dgram->buflen,
      //         NET_GetAddressString(dgram->addr), (int)dgram->port);

      std::deque<Uint8> datavec(dgram->buf, dgram->buf + dgram->buflen);

      while (!datavec.empty()) {
        CobblerNetData temp;
        temp.IP = NET_GetAddressString(dgram->addr);
        temp.PORT = dgram->port;

        while (datavec.front() != Uint8(0) && !datavec.empty()) {
          temp.name.push_back(char(datavec.front()));
          datavec.pop_front();
        }
        datavec.pop_front();

        unsigned char len = (unsigned char)datavec.front();
        datavec.pop_front();

        temp.buffer.insert(temp.buffer.end(), datavec.begin(),
                           datavec.begin() + len);
        datavec.erase(datavec.begin(), datavec.begin() + len);

        tempvec->push_back(temp);
      }
      NET_DestroyDatagram(dgram);
      return tempvec;
    }
  }
  return NULL;
}

void CobblerQuitNet() {
  if (NetStuff->Socket != nullptr) NET_DestroyDatagramSocket(NetStuff->Socket);
  for (int i = 0; i < NetStuff->Addresses.size(); i++)
    NET_UnrefAddress(NetStuff->Addresses[i]);

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