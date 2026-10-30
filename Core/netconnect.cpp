#include "netconnect.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>

#include "extern.h"
#include "update.h"

// Server Setup.
void ServerSetup(uint32_t ServerPort) {
  if (!CobblerInitNet()) return;

  if (!CobblerSetSocket(ServerPort)) {
    SDL_Log("Server Setup failed");
  }
}

// Client Setup.
void ClientConnect(const std::string& ServerIP, uint32_t ServerPort) {
  if (!CobblerInitNet()) return;

  if (!CobblerSetSocket(0)) {
    SDL_Log("Server connection failed");
  }
  CobblerAddIP(ServerIP, ServerPort, 0);
  Global->IsOnline = true;
  std::vector<uint8_t> buffer{};
  bool check = false;

  // check if the server acknowledged you
  while (IsRunning && !check) {
    events();
    CobblerQueueData("PlayerAdd", buffer, 0);
    std::vector<CobblerNetData>* tempvector = CobblerRecvNet();
    if (tempvector != NULL) {
      while (!tempvector->empty()) {
        CobblerNetData* tempdata = &tempvector->back();
        SDL_Log("%s", tempdata->name.c_str());
        check = true;
        tempvector->pop_back();
        break;
      }
      delete tempvector;
    }
    CobblerSendNet();
    SDL_DelayNS(1000000000 / 30);
  }
  // SDL_Log("What");
}