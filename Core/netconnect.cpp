#include "netconnect.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>

#include "extern.h"
#include "network.h"
#include "networkextern.h"
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
    return;
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
      if (!tempvector->empty()) {
        CobblerNetData* tempdata = &tempvector->back();
        SDL_Log("%s", tempdata->name.c_str());
        check = true;
      }
      delete tempvector;
    }
    CobblerSendNet();
    SDL_DelayPrecise(uint64_t(1 / (double)30));
  }
  // SDL_Log("What");
}

void NetDisconnect() {
  SDL_Log("NetDisconnect");
  // for (auto& [ID, NetStuff] : GlobalNetworkStuff->PlayerNetStuff) {
  //   SDL_Log("%llu", ID);
  //   if (NetStuff.PlayerEntity != NULL) {
  //     SDL_Log("%s", NetStuff.PlayerEntity->name.c_str());
  //     delete (NetStuff.PlayerEntity);
  //   }
  // }
  GlobalNetworkStuff->UserIDs.clear();
  GlobalNetworkStuff->PlayerNetStuff.clear();
  CobblerQuitNet();
  Global->IsOnline = false;
  IsServer = false;
}
