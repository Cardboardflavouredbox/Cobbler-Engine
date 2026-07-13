#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>
#include <curl/curl.h>

#include <array>
#include <bit>
#include <cstring>
#include <deque>
#include <sstream>
#include <vector>

#include "network.h"

struct NetworkStuffClass {
  struct Clientthing {
    Uint16 PORT;
    std::string Address;
    NET_Address* RealAddress = NULL;
    Uint64 ID;
  };
  std::vector<Clientthing> Clients;
  NET_DatagramSocket* Socket;

  CURL* curl;
  CURLcode res;
};

NetworkStuffClass* NetStuff;
PostField* curlpostfield;
std::string curlloginstring;
Uint64 UserID = 0;
std::vector<Uint8> packetbuffer;

bool IsServer = false;

static size_t CobblerCurlCallback(char* data, size_t size, size_t nmemb,
                                  void* clientp) {
  size_t totalSize = size * nmemb;

  std::ostringstream* stream = (std::ostringstream*)clientp;

  stream->write(data, totalSize);

  return totalSize;
}

void CobblerAddIP(std::string IP, unsigned int Port, Uint64 ID) {
  NetworkStuffClass::Clientthing client;
  client.Address = IP;
  client.PORT = Port;
  client.ID = ID;
  client.RealAddress = NET_ResolveHostname(IP.c_str());
  NetStuff->Clients.push_back(client);
  SDL_Log("%d %s %u", (int)NetStuff->Clients.size(), IP.c_str(), Port);
}

bool CobblerCheckHasIP(std::string IP, unsigned int Port) {
  for (int i = 0; i < NetStuff->Clients.size(); i++) {
    if (NetStuff->Clients[i].Address == IP && NetStuff->Clients[i].PORT == Port)
      return true;
  }
  return false;
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

bool CobblerSendNet() {  // from: ID, to: ID
  Uint64 tempID = UserID;
  if constexpr (std::endian::native == std::endian::little) {
    tempID = std::byteswap(tempID);
  }
  auto localID = std::bit_cast<std::array<Uint8, 8>>(tempID);

  for (int i = 0; i < NetStuff->Clients.size(); i++) {
    Uint64 ID = NetStuff->Clients[i].ID;

    if constexpr (std::endian::native == std::endian::little) {
      ID = std::byteswap(ID);
    }

    // 2. Cast directly into a fixed-size byte array safely
    auto byte_array = std::bit_cast<std::array<Uint8, 8>>(ID);

    std::vector<Uint8> temppacket(localID.begin(), localID.end());

    temppacket.insert(temppacket.end(), byte_array.begin(), byte_array.end());

    temppacket.insert(temppacket.end(), packetbuffer.begin(),
                      packetbuffer.end());

    if (!NET_SendDatagram(NetStuff->Socket, NetStuff->Clients[i].RealAddress,
                          NetStuff->Clients[i].PORT, temppacket.data(),
                          temppacket.size())) {
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

      std::array<Uint8, 8> tempbytes;

      for (int i = 0; i < 8; i++) {
        tempbytes[i] = datavec.front();
        datavec.pop_front();
      }

      Uint64 ID = std::bit_cast<Uint64>(tempbytes);

      if constexpr (std::endian::native == std::endian::little) {
        ID = std::byteswap(ID);
      }

      for (int i = 0; i < 8; i++) {
        tempbytes[i] = datavec.front();
        datavec.pop_front();
      }

      UserID = std::bit_cast<Uint64>(tempbytes);

      if constexpr (std::endian::native == std::endian::little) {
        UserID = std::byteswap(UserID);
      }

      while (!datavec.empty()) {
        CobblerNetData temp;
        temp.IP = NET_GetAddressString(dgram->addr);
        temp.PORT = dgram->port;
        temp.ID = ID;

        if (!datavec.empty()) {
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
        }

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
  for (int i = 0; i < NetStuff->Clients.size(); i++) {
    if (NetStuff->Clients[i].RealAddress != NULL)
      NET_UnrefAddress(NetStuff->Clients[i].RealAddress);
  }

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