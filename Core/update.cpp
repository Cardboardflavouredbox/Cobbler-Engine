#include "update.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/bitsery.h>
#include <bitsery/brief_syntax.h>
#include <bitsery/brief_syntax/map.h>
#include <bitsery/brief_syntax/set.h>
#include <bitsery/traits/array.h>
#include <bitsery/traits/string.h>
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
void serialize(S& s, ParticleSpawnInfo& o) {
  s.value4b(o.ParticleCode);
  s.container4b(o.position);
  s.text1b(o.name, 32);
}

template <typename S>
void serialize(S& s, EntitySpawnInfo& o) {
  s.value4b(o.teamindex);
  s.value4b(o.hp);
  s.text1b(o.name, 32);
  s.value4b(o.EntityCode);
  s.value4b(o.EntityIndex);
  s.container4b(o.direction);
  s.container4b(o.position);
  s.container4b(o.velocityvec3);
  s.value4b(o.State);
}

template <typename S>
void serialize(S& s, playerdatapacket& o) {
  s.value1b(o.altattack);
  s.value1b(o.attack);
  s.value8b(o.ID);
  s.value4b(o.teamindex);
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

// recieve net data
void RecieveNetData() {
  for (auto& i : GlobalNetworkStuff->PlayerNetStuff) {
    if (i.first != UserID) i.second.Timecounter += deltaTime;
  }
  std::queue<uint64_t> deleteplayerqueue;

  std::vector<CobblerNetData>* tempvector = CobblerRecvNet();
  if (tempvector != NULL) {
    while (!tempvector->empty()) {
      CobblerNetData* tempdata = &tempvector->back();
      // SDL_Log("%s", tempdata->name.c_str());
      if (GlobalNetworkStuff->PlayerNetStuff.contains(tempdata->ID))
        GlobalNetworkStuff->PlayerNetStuff[tempdata->ID].Timecounter = 0;
      if (tempdata->name == "Player") {
        playerdatapacket temp;
        auto state = bitsery::quickDeserialization<
            bitsery::InputBufferAdapter<std::vector<uint8_t>>>(
            {tempdata->buffer.begin(), tempdata->size}, temp);
        if (state.first == bitsery::ReaderError::NoError && state.second) {
          if (GlobalNetworkStuff->PlayerNetStuff.contains(temp.ID) &&
              temp.ID != UserID &&
              GlobalNetworkStuff->PlayerNetStuff[temp.ID].PlayerEntity !=
                  nullptr) {
            GlobalNetworkClass::PlayerNetClass* tempplayerthing =
                &GlobalNetworkStuff->PlayerNetStuff[temp.ID];
            tempplayerthing->PlayerInput = Loadinputdata(temp);
            for (int i = 0; i < 3; i++) {
              tempplayerthing->PlayerEntity->velocityvec3[i] =
                  temp.velocityvec3[i];
              tempplayerthing->PlayerEntity->position[i] = temp.position[i];
            }
            tempplayerthing->PlayerEntity->teamindex = temp.teamindex;
            tempplayerthing->PlayerEntity->IsGrounded = temp.IsGrounded;
            tempplayerthing->PlayerEntity->State = temp.State;
          } else {
            // SDL_Log("%llu %llu", temp.ID, UserID);
          }
        }
        // else {
        //   SDL_Log("%u Receive", tempdata->size);
        // }

      } else if (IsServer && tempdata->name == "PlayerAdd") {
        if (!CobblerCheckHasIP(tempdata->IP, tempdata->PORT)) {
          uint64_t i = 1;
          while (GlobalNetworkStuff->UserIDs.find(i) !=
                 GlobalNetworkStuff->UserIDs.end()) {
            i++;
          }

          GlobalNetworkStuff->UserIDs.insert(i);
          CobblerAddIP(tempdata->IP, tempdata->PORT, i);

          GlobalNetworkStuff->PlayerNetStuff[i].PlayerEntity =
              SpawnEntities["Gardner"](0, i);
        }
      } else if (tempdata->name == "PlayerList") {
        std::set<uint64_t> tempset;
        auto state = bitsery::quickDeserialization<
            bitsery::InputBufferAdapter<std::vector<uint8_t>>>(
            {tempdata->buffer.begin(), tempdata->size}, tempset);
        if (state.first == bitsery::ReaderError::NoError && state.second) {
          for (auto& key : tempset) {
            if (key != UserID && GlobalNetworkStuff->UserIDs.find(key) ==
                                     GlobalNetworkStuff->UserIDs.end()) {
              GlobalNetworkStuff->UserIDs.insert(key);
              GlobalNetworkStuff->PlayerNetStuff[key].PlayerEntity =
                  SpawnEntities["Gardner"](0, key);
            }
          }
        }
      } else if (tempdata->name == "LocalEntity") {
        EntitySpawnInfo tempinfo;
        auto state = bitsery::quickDeserialization<
            bitsery::InputBufferAdapter<std::vector<uint8_t>>>(
            {tempdata->buffer.begin(), tempdata->size}, tempinfo);
        if (state.first == bitsery::ReaderError::NoError && state.second) {
          uint32_t entityindex = 0;
          if (IsServer) {
            entityindex = EntitySpawn(tempinfo, false);
          } else {
            if (Entities.contains(tempinfo.EntityIndex)) {
              entityindex = tempinfo.EntityIndex;
              Entity* tempentity = Entities[tempinfo.EntityIndex];
              if (tempentity->name != tempinfo.name ||
                  tempentity->EntityCode != tempinfo.EntityCode) {
                delete (tempentity);
                tempentity = SpawnEntities[tempinfo.name](tempinfo.EntityCode,
                                                          tempinfo.EntityIndex);
              }
              if (tempinfo.hp != -1) tempentity->hp = tempinfo.hp;
              tempentity->name = tempinfo.name;
              tempentity->EntityCode = tempinfo.EntityCode;

              tempentity->EntityIndex = tempinfo.EntityIndex;
              for (int i = 0; i < 3; i++) {
                tempentity->position[i] = tempinfo.position[i];
                tempentity->velocityvec3[i] = tempinfo.velocityvec3[i];
              }
              tempentity->State = tempinfo.State;
              tempentity->teamindex = tempinfo.teamindex;
              for (int i = 0; i < 2; i++) {
                tempentity->dir[i] = tempinfo.direction[i];
              }
            } else {
              entityindex = EntitySpawn(tempinfo, false);
            }
          }
          Entities[entityindex]->deltatimelocal =
              GlobalNetworkStuff->PlayerNetStuff[tempdata->ID].deltatimelocal;
        }
      } else if (tempdata->name == "ParticleSpawn") {
        ParticleSpawnInfo tempinfo;
        auto state = bitsery::quickDeserialization<
            bitsery::InputBufferAdapter<std::vector<uint8_t>>>(
            {tempdata->buffer.begin(), tempdata->size}, tempinfo);
        if (state.first == bitsery::ReaderError::NoError && state.second) {
          if (IsServer) {
            CobblerQueueData("ParticleSpawn", tempdata->buffer, tempdata->size);
          }
          ParticleSpawn(tempinfo, false);
        }
      } else if (tempdata->name == "SendTick") {
        CobblerQueueData("ReturnTick", tempdata->buffer, tempdata->size);
      } else if (tempdata->name == "ReturnTick") {
        if (GlobalNetworkStuff->PlayerNetStuff.contains(tempdata->ID)) {
          uint64_t temp;
          auto state = bitsery::quickDeserialization<
              bitsery::InputBufferAdapter<std::vector<uint8_t>>>(
              {tempdata->buffer.begin(), tempdata->size}, temp);
          if (state.first == bitsery::ReaderError::NoError && state.second) {
            temp = SDL_GetPerformanceCounter() - temp;
            temp /= 2;
            double result = temp / (double)SDL_GetPerformanceFrequency();
            if (result > 0.03125f) result = 0.03125f;

            GlobalNetworkStuff->PlayerNetStuff[tempdata->ID]
                .PlayerEntity->deltatimelocal = result;
            GlobalNetworkStuff->PlayerNetStuff[tempdata->ID].deltatimelocal =
                result;
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

  for (auto i : GlobalNetworkStuff->PlayerNetStuff) {
    if (i.second.Timecounter > 5) {  // timeout player
      SDL_Log("player%llu timed out", i.first);

      deleteplayerqueue.push(i.first);
    }
  }

  while (!deleteplayerqueue.empty()) {
    GlobalNetworkStuff->UserIDs.erase(deleteplayerqueue.front());

    delete (GlobalNetworkStuff->PlayerNetStuff[deleteplayerqueue.front()]
                .PlayerEntity);

    GlobalNetworkStuff->PlayerNetStuff.erase(deleteplayerqueue.front());
    deleteplayerqueue.pop();
  }
}

void update() {
  if (Global->IsOnline) {
    RecieveNetData();
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
    for (const auto& [ID, player] : GlobalNetworkStuff->PlayerNetStuff) {
      if (player.PlayerEntity != NULL)
        inputtoentity(player.PlayerInput, player.PlayerEntity);
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
    uint32_t index = EntitydeleteQueue.front();
    EntitydeleteQueue.pop();
    delete (Entities[index]);
    Entities.erase(index);
  }

  while (!ParticledeleteQueue.empty()) {
    uint32_t index = ParticledeleteQueue.front();
    ParticledeleteQueue.pop();
    delete (Particles[index]);
    Particles.erase(index);
  }

  CameraUpdate();

  // SDL_Log("%f %f %f", Entities[1]->position[0],
  //         Entities[1]->position[1],
  //         Entities[1]->position[2]);

  if (Global->IsOnline) {  // send net data
    if (IsServer) {
      std::vector<uint8_t> buffer{};

      auto writtenSize = bitsery::quickSerialization<
          bitsery::OutputBufferAdapter<std::vector<uint8_t>>>(
          {buffer}, GlobalNetworkStuff->UserIDs);

      CobblerQueueData("PlayerList", buffer, writtenSize);

      buffer.clear();

      for (const auto& [ID, player] : GlobalNetworkStuff->PlayerNetStuff) {
        if (ID != UserID) {
          std::vector<uint8_t> buffer{};
          playerdatapacket temp;
          Entity* entity = player.PlayerEntity;
          temp.State = entity->State;
          temp.teamindex = entity->teamindex;
          temp.ID = ID;
          temp.Set(&player.PlayerInput);
          temp.IsGrounded = entity->IsGrounded;
          for (int i = 0; i < 3; i++) {
            temp.position[i] = entity->position[i];
            temp.velocityvec3[i] = entity->velocityvec3[i];
          }
          auto writtenSize = bitsery::quickSerialization<
              bitsery::OutputBufferAdapter<std::vector<uint8_t>>>({buffer},
                                                                  temp);

          CobblerQueueData("Player", buffer, writtenSize);
        }
      }

      for (const auto& [ID, entity] : Entities) {
        if (ID > 0) {
          std::vector<uint8_t> buffer{};
          EntitySpawnInfo temp;
          for (int i = 0; i < 2; i++) temp.direction[i] = entity->dir[i];
          temp.EntityIndex = entity->EntityIndex;
          temp.EntityCode = entity->EntityCode;
          temp.hp = entity->hp;
          temp.name = entity->name;
          temp.State = entity->State;
          temp.teamindex = entity->teamindex;
          for (int i = 0; i < 3; i++) {
            temp.position[i] = entity->position[i];
            temp.velocityvec3[i] = entity->velocityvec3[i];
          }
          auto writtenSize = bitsery::quickSerialization<
              bitsery::OutputBufferAdapter<std::vector<uint8_t>>>({buffer},
                                                                  temp);

          CobblerQueueData("LocalEntity", buffer, writtenSize);
        }
      }
    }
    std::vector<uint8_t> buffer{};
    if (LocalPlayer != NULL) {
      playerdatapacket temp;
      temp.teamindex = LocalPlayer->teamindex;
      temp.State = LocalPlayer->State;
      temp.ID = UserID;
      temp.Set(P1PlayerInputs);
      temp.IsGrounded = LocalPlayer->IsGrounded;
      for (int i = 0; i < 3; i++) {
        temp.position[i] = LocalPlayer->position[i];
        temp.velocityvec3[i] = LocalPlayer->velocityvec3[i];
      }
      auto writtenSize = bitsery::quickSerialization<
          bitsery::OutputBufferAdapter<std::vector<uint8_t>>>({buffer}, temp);
      CobblerQueueData("Player", buffer, writtenSize);
      // SDL_Log("%u Send", writtenSize);

      buffer.clear();
    }
    auto writtenSize = bitsery::quickSerialization<
        bitsery::OutputBufferAdapter<std::vector<uint8_t>>>(
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
  std::vector<uint8_t> buffer{};

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
              bitsery::InputBufferAdapter<std::vector<uint8_t>>>(
              {tempdata->buffer.begin(), tempdata->size}, tempset);
          if (state.first == bitsery::ReaderError::NoError && state.second) {
            if (tempset.find(UserID) == tempset.end()) {
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