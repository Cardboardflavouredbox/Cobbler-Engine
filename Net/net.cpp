#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>

#ifdef _WIN32
#ifdef DLLEXPORT
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else
#define LIB_API
#endif

struct NetworkStuffClass {
  Uint16 PORT = 12093;
  NET_Address* Address;
  NET_DatagramSocket* Socket;
};

NetworkStuffClass* NetStuff;

LIB_API bool CobblerSetSocket(char* ipaddress, Uint16 port) {
  NetStuff->Address = NET_ResolveHostname(ipaddress);
  NetStuff->PORT = port;
  NetStuff->Socket =
      NET_CreateDatagramSocket(NetStuff->Address, NetStuff->PORT, 0);
  return true;
}

LIB_API void CobblerDestroySocket() {
  NET_DestroyDatagramSocket(NetStuff->Socket);
  NET_UnrefAddress(NetStuff->Address);
}

LIB_API bool CobblerInitNet() {
  if (!NET_Init()) return false;
  NetStuff = new NetworkStuffClass();
  return true;
}

LIB_API void CobblerQuitNet() {
  if (NetStuff->Socket != nullptr) NET_DestroyDatagramSocket(NetStuff->Socket);
  if (NetStuff->Address != nullptr) NET_UnrefAddress(NetStuff->Address);
  delete (NetStuff);
  NET_Quit();
}