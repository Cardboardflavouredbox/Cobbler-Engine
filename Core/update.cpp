#include "update.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/bitsery.h>
#include <bitsery/brief_syntax.h>
#include <bitsery/brief_syntax/set.h>
#include <bitsery/traits/array.h>
#include <bitsery/traits/vector.h>

#include <cmath>
#include <glm/glm.hpp>
#include <queue>

#include "camera.h"
#include "components.h"
#include "deltaTime.h"
#include "entity.h"
#include "extern.h"
#include "global.h"
#include "inputs.h"
#include "network.h"
#include "networkextern.h"
#include "pi.h"
#include "player.h"
#include "render.h"
#include "settings.h"

template <typename S>
void serialize(S& s, playerdatapacket& o) {
  s.value1b(o.altattack);
  s.value1b(o.attack);
  s.value8b(o.ID);
  s.value1b(o.IsGrounded);
  s.value1b(o.jump);
  s.container4b(o.lookdir);
  s.container4b(o.movevec2);
  s.container4b(o.position);
  s.value4b(o.State);
  s.container4b(o.velocityvec3);
}

playerinputs Loadinputdata(playerdatapacket input) {
  playerinputs temp;
  for (int i = 0; i < 2; i++) {
    temp.movevec2[i] = input.movevec2[i];
    temp.lookdir[i] = input.lookdir[i];
  }
  temp.jump = input.jump;
  temp.attack = input.attack;
  temp.altattack = input.altattack;
  return temp;
}

void processinputs() {
  P1PlayerInputs->lookdir.x += -0.5f * Settings->mousesensitivity.x *
                               LocalInputs->MouseDelta.x *
                               RendererGlobal->windowscale;
  P1PlayerInputs->lookdir.y += -0.5f * Settings->mousesensitivity.y *
                               LocalInputs->MouseDelta.y *
                               RendererGlobal->windowscale;

  if (P1PlayerInputs->lookdir.x < 0) P1PlayerInputs->lookdir.x += 360;
  if (P1PlayerInputs->lookdir.x >= 360) P1PlayerInputs->lookdir.x -= 360;
  if (P1PlayerInputs->lookdir.y >= 89) P1PlayerInputs->lookdir.y = 89;
  if (P1PlayerInputs->lookdir.y <= -89) P1PlayerInputs->lookdir.y = -89;

  LocalInputs->MouseDelta.x = 0;
  LocalInputs->MouseDelta.y = 0;
  float ps = std::sin(P1PlayerInputs->lookdir.x * PI / 180.0);
  float pc = std::cos(P1PlayerInputs->lookdir.x * PI / 180.0);

  glm::vec2 tempmove = glm::vec3(0);
  if (LocalInputs->Keys[SDL_SCANCODE_A] > 0 &&
      LocalInputs->Keys[SDL_SCANCODE_D] == 0) {
    tempmove.x -= std::sin((P1PlayerInputs->lookdir.x + 90) * PI / 180.0);
    tempmove.y += std::cos((P1PlayerInputs->lookdir.x + 90) * PI / 180.0);
  }
  if (LocalInputs->Keys[SDL_SCANCODE_D] > 0 &&
      LocalInputs->Keys[SDL_SCANCODE_A] == 0) {
    tempmove.x -= std::sin((P1PlayerInputs->lookdir.x - 90) * PI / 180.0);
    tempmove.y += std::cos((P1PlayerInputs->lookdir.x - 90) * PI / 180.0);
  }
  if (LocalInputs->Keys[SDL_SCANCODE_W] > 0) {
    tempmove.x -= ps;
    tempmove.y += pc;
  }
  if (LocalInputs->Keys[SDL_SCANCODE_S] > 0) {
    tempmove.x += ps;
    tempmove.y -= pc;
  }

  if (tempmove != glm::vec2(0)) tempmove = glm::normalize(tempmove);
  P1PlayerInputs->movevec2 = tempmove;
  P1PlayerInputs->jump = LocalInputs->Keys[SDL_SCANCODE_SPACE];
}

void CameraUpdate() {
  if (LocalPlayer != NULL) {
    Camera->pos = LocalPlayer->position;
    Camera->pos.z += LocalPlayer->cameraoffset;
    Camera->lookat.x = std::cos(glm::radians(LocalPlayer->dir.x + 90.f)) *
                       std::cos(glm::radians(LocalPlayer->dir.y));
    Camera->lookat.z = std::sin(glm::radians(LocalPlayer->dir.y));
    Camera->lookat.y = std::sin(glm::radians(LocalPlayer->dir.x + 90.f)) *
                       std::cos(glm::radians(LocalPlayer->dir.y));
    Camera->lookat += Camera->pos;
  }
}

void update() {
  if (Global->IsOnline) {  // recieve net data
    for (auto& i : GlobalNetworkStuff->PlayerTimecounter) {
      if (i.first != GlobalNetworkStuff->UserID) i.second += deltaTime;
    }
    std::queue<uint64_t> deleteplayerqueue;

    std::vector<CobblerNetData>* tempvector = CobblerRecvNet();
    if (tempvector != NULL) {
      while (!tempvector->empty()) {
        CobblerNetData* tempdata = &tempvector->back();
        // SDL_Log("%s", tempdata->name.c_str());
        GlobalNetworkStuff->PlayerTimecounter[tempdata->ID] = 0;
        if (tempdata->name == "Player") {
          playerdatapacket temp;
          auto state = bitsery::quickDeserialization<
              bitsery::InputBufferAdapter<std::vector<Uint8>>>(
              {tempdata->buffer.begin(), tempdata->size}, temp);
          if (state.first == bitsery::ReaderError::NoError && state.second) {
            if (GlobalNetworkStuff->PlayerEntity.contains(temp.ID) &&
                temp.ID != GlobalNetworkStuff->UserID) {
              GlobalNetworkStuff->PlayerInputList[temp.ID] =
                  Loadinputdata(temp);
              for (int i = 0; i < 3; i++) {
                GlobalNetworkStuff->PlayerEntity[temp.ID]->velocityvec3[i] =
                    temp.velocityvec3[i];
                GlobalNetworkStuff->PlayerEntity[temp.ID]->position[i] =
                    temp.position[i];
              }
              GlobalNetworkStuff->PlayerEntity[temp.ID]->IsGrounded =
                  temp.IsGrounded;
              GlobalNetworkStuff->PlayerEntity[temp.ID]->State = temp.State;
            }
          }
          // else {
          //   SDL_Log("%u Receive", tempdata->size);
          // }

        } else if (GlobalNetworkStuff->IsServer &&
                   tempdata->name == "PlayerAdd") {
          if (!CobblerCheckHasIP(tempdata->IP, tempdata->PORT)) {
            uint64_t i = 1;
            while (GlobalNetworkStuff->UserIDs.find(i) !=
                   GlobalNetworkStuff->UserIDs.end()) {
              i++;
            }

            GlobalNetworkStuff->UserIDs.insert(i);
            CobblerAddIP(tempdata->IP, tempdata->PORT, i);

            GlobalNetworkStuff->PlayerEntity[i] =
                SpawnEntities["Gardner"](0, i);
          }
        } else if (tempdata->name == "PlayerList") {
          std::set<uint64_t> tempset;
          auto state = bitsery::quickDeserialization<
              bitsery::InputBufferAdapter<std::vector<Uint8>>>(
              {tempdata->buffer.begin(), tempdata->size}, tempset);
          if (state.first == bitsery::ReaderError::NoError && state.second) {
            for (auto& key : tempset) {
              if (key != GlobalNetworkStuff->UserID &&
                  GlobalNetworkStuff->UserIDs.find(key) ==
                      GlobalNetworkStuff->UserIDs.end()) {
                GlobalNetworkStuff->UserIDs.insert(key);
                GlobalNetworkStuff->PlayerEntity[key] =
                    SpawnEntities["Gardner"](0, key);
              }
            }
          }
        } else if (tempdata->name == "SendTick") {
          CobblerQueueData("ReturnTick", tempdata->buffer, tempdata->size);
        } else if (tempdata->name == "ReturnTick") {
          if (GlobalNetworkStuff->PlayerEntity.contains(tempdata->ID)) {
            uint64_t temp;
            auto state = bitsery::quickDeserialization<
                bitsery::InputBufferAdapter<std::vector<Uint8>>>(
                {tempdata->buffer.begin(), tempdata->size}, temp);
            if (state.first == bitsery::ReaderError::NoError && state.second) {
              temp = SDL_GetPerformanceCounter() - temp;
              temp /= 2;
              GlobalNetworkStuff->PlayerEntity[tempdata->ID]->deltatimelocal =
                  temp / (double)SDL_GetPerformanceFrequency();
              if (GlobalNetworkStuff->PlayerEntity[tempdata->ID]
                      ->deltatimelocal > 0.03125f)
                GlobalNetworkStuff->PlayerEntity[tempdata->ID]->deltatimelocal =
                    0.03125f;
              // SDL_Log("%f",
              // Entities[GlobalNetworkStuff->PlayerEntity[tempdata->ID]]
              //                   ->deltatimelocal);
            }
          }
        } else if (tempdata->name == "PlayerQuit") {
          if (GlobalNetworkStuff->UserIDs.contains(tempdata->ID)) {
            SDL_Log("player%llu client disconnect", tempdata->ID);
            deleteplayerqueue.push(tempdata->ID);
          }
        }
        tempvector->pop_back();
      }
      delete tempvector;
    }

    for (auto i : GlobalNetworkStuff->PlayerTimecounter) {
      if (i.second > 5) {  // timeout player
        SDL_Log("player%llu timed out", i.first);

        deleteplayerqueue.push(i.first);
      }
    }

    while (!deleteplayerqueue.empty()) {
      GlobalNetworkStuff->UserIDs.erase(deleteplayerqueue.front());

      delete (GlobalNetworkStuff->PlayerEntity[deleteplayerqueue.front()]);

      GlobalNetworkStuff->PlayerEntity.erase(deleteplayerqueue.front());
      GlobalNetworkStuff->PlayerInputList.erase(deleteplayerqueue.front());
      GlobalNetworkStuff->PlayerTimecounter.erase(deleteplayerqueue.front());
      deleteplayerqueue.pop();
    }
  }

  lastTime = currentTime;
  currentTime = SDL_GetPerformanceCounter();
  deltaTime = ((double)(currentTime - lastTime)) /
              (double)SDL_GetPerformanceFrequency();
  if (LocalInputs->Keys[SDL_SCANCODE_ESCAPE] == 2) {
    Global->pause = !Global->pause;
    SDL_SetWindowRelativeMouseMode(RendererGlobal->window, !Global->pause);
  }

  if (!Global->pause) {
    processinputs();
    if (LocalPlayer != NULL) inputtoentity(*P1PlayerInputs, LocalPlayer);
    for (const auto& [ID, entity] : GlobalNetworkStuff->PlayerEntity) {
      inputtoentity(GlobalNetworkStuff->PlayerInputList[ID], entity);
    }
    if (LocalPlayer != NULL) PlayerClassUpdate[Global->playerclass]();
    componentsupdate();
    for (auto& [key, value] : UIGlobalStuff->UImap3D) {
      for (auto& i : value) {
        i->update();
      }
    }
  }

  componentsupdatelate();

  while (!EntitydeleteQueue.empty()) {
    unsigned int index = EntitydeleteQueue.front();
    EntitydeleteQueue.pop();
    delete (Entities[index]);
    Entities.erase(index);
  }

  while (!ParticledeleteQueue.empty()) {
    unsigned int index = ParticledeleteQueue.front();
    ParticledeleteQueue.pop();
    delete (Particles[index]);
    Particles.erase(index);
  }

  CameraUpdate();

  // SDL_Log("%f %f %f", Entities[1]->position[0],
  //         Entities[1]->position[1],
  //         Entities[1]->position[2]);

  if (Global->IsOnline) {  // send net data
    if (GlobalNetworkStuff->IsServer) {
      std::vector<Uint8> buffer{};

      auto writtenSize = bitsery::quickSerialization<
          bitsery::OutputBufferAdapter<std::vector<Uint8>>>(
          {buffer}, GlobalNetworkStuff->UserIDs);

      CobblerQueueData("PlayerList", buffer, writtenSize);

      buffer.clear();

      for (const auto& [ID, entity] : GlobalNetworkStuff->PlayerEntity) {
        if (ID != GlobalNetworkStuff->UserID) {
          std::vector<Uint8> buffer{};
          playerdatapacket temp;
          temp.State = GlobalNetworkStuff->PlayerEntity[ID]->State;
          temp.ID = ID;
          temp.Set(&GlobalNetworkStuff->PlayerInputList[ID]);
          temp.IsGrounded = GlobalNetworkStuff->PlayerEntity[ID]->IsGrounded;
          for (int i = 0; i < 3; i++) {
            temp.position[i] =
                GlobalNetworkStuff->PlayerEntity[ID]->position[i];
            temp.velocityvec3[i] =
                GlobalNetworkStuff->PlayerEntity[ID]->velocityvec3[i];
          }
          auto writtenSize = bitsery::quickSerialization<
              bitsery::OutputBufferAdapter<std::vector<Uint8>>>({buffer}, temp);

          CobblerQueueData("Player", buffer, writtenSize);
        }
      }
    }
    std::vector<Uint8> buffer{};
    if (LocalPlayer != NULL) {
      playerdatapacket temp;
      temp.State = LocalPlayer->State;
      temp.ID = GlobalNetworkStuff->UserID;
      temp.Set(P1PlayerInputs);
      temp.IsGrounded = LocalPlayer->IsGrounded;
      for (int i = 0; i < 3; i++) {
        temp.position[i] = LocalPlayer->position[i];
        temp.velocityvec3[i] = LocalPlayer->velocityvec3[i];
      }
      auto writtenSize = bitsery::quickSerialization<
          bitsery::OutputBufferAdapter<std::vector<Uint8>>>({buffer}, temp);
      CobblerQueueData("Player", buffer, writtenSize);
      // SDL_Log("%u Send", writtenSize);

      buffer.clear();
    }
    auto writtenSize = bitsery::quickSerialization<
        bitsery::OutputBufferAdapter<std::vector<Uint8>>>(
        {buffer}, SDL_GetPerformanceCounter());
    CobblerQueueData("SendTick", buffer, writtenSize);

    GlobalNetworkStuff->Onlinesendwait -= deltaTime;
    while (GlobalNetworkStuff->Onlinesendwait <= 0) {
      CobblerSendNet();
      GlobalNetworkStuff->Onlinesendwait += 0.05f;
    }
  }

  // if (curlpostfield->hasdata && Global->LoggedIn) CobblerSendCurlData();
}

void PlayerQuit() {
  std::vector<Uint8> buffer{};

  for (int i = 0; i < 30; i++) {
    CobblerQueueData("PlayerQuit", buffer, 0);
    std::vector<CobblerNetData>* tempvector = CobblerRecvNet();
    if (tempvector != NULL) {
      bool check = false;
      while (!tempvector->empty()) {
        CobblerNetData* tempdata = &tempvector->back();
        if (tempdata->name == "PlayerList") {
          std::set<uint64_t> tempset;
          auto state = bitsery::quickDeserialization<
              bitsery::InputBufferAdapter<std::vector<Uint8>>>(
              {tempdata->buffer.begin(), tempdata->size}, tempset);
          if (state.first == bitsery::ReaderError::NoError && state.second) {
            if (tempset.find(GlobalNetworkStuff->UserID) == tempset.end()) {
              check = true;
              break;
            }
          }
        }
        tempvector->pop_back();
        break;
      }
      delete tempvector;
      if (check) break;
    }

    CobblerSendNet();
    SDL_DelayNS(1000000000 / 30);
  }
}