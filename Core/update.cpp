#include "update.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>

#include <cmath>
#include <glm/glm.hpp>
#include <queue>

#include "bitserytemplates.h"
#include "camera.h"
#include "components.h"
#include "deltaTime.h"
#include "entity.h"
#include "extern.h"
#include "global.h"
#include "inputs.h"
#include "lights.h"
#include "network.h"
#include "networkextern.h"
#include "physics.h"
#include "pi.h"
#include "player.h"
#include "render.h"
#include "settings.h"

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
    if (i.first != UserID) i.second.Timecounter += updatedeltaTime;
    i.second.PlayerInput.jump = 0;
    i.second.PlayerInput.altattack = 0;
    i.second.PlayerInput.attack = 0;
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
              GlobalNetworkStuff->PlayerNetStuff[temp.ID].PlayerEntity !=
                  nullptr) {
            GlobalNetworkClass::PlayerNetClass* tempplayerthing =
                &GlobalNetworkStuff->PlayerNetStuff[temp.ID];

            unsigned char jump = tempplayerthing->PlayerInput.jump,
                          attack = tempplayerthing->PlayerInput.attack,
                          altattack = tempplayerthing->PlayerInput.altattack;

            tempplayerthing->PlayerInput = Loadinputdata(temp);

            if (jump > tempplayerthing->PlayerInput.jump)
              tempplayerthing->PlayerInput.jump = 2;
            if (attack > tempplayerthing->PlayerInput.attack)
              tempplayerthing->PlayerInput.attack = 2;
            if (altattack > tempplayerthing->PlayerInput.altattack)
              tempplayerthing->PlayerInput.altattack = 2;

            if (!IsServer) {
              for (int i = 0; i < 3; i++) {
                tempplayerthing->PlayerEntity->velocityvec3[i] =
                    temp.velocityvec3[i];
                tempplayerthing->PlayerEntity->position[i] = temp.position[i];
              }
              tempplayerthing->PlayerEntity->IsGrounded = temp.IsGrounded;
            }
            tempplayerthing->PlayerEntity->teamindex = temp.teamindex;
            tempplayerthing->PlayerEntity->State = temp.State;
            // EntityMove(tempplayerthing->PlayerEntity);
            // tempplayerthing->PlayerEntity->deltatimelocal = 0;
          } else if (temp.ID == UserID) {
            glm::vec3 tempvec3;
            for (int i = 0; i < 3; i++) {
              tempvec3[i] = temp.position[i];
            }

            GlobalNetworkStuff->RecvEntity->position = tempvec3;

            for (int i = 0; i < 3; i++) {
              tempvec3[i] = temp.velocityvec3[i];
            }
            GlobalNetworkStuff->RecvEntity->velocityvec3 = tempvec3;

            GlobalNetworkStuff->RecvEntity->IsGrounded = temp.IsGrounded;

            GlobalNetworkStuff->RecvEntity->teamindex = temp.ID;
            LocalPlayer->teamindex = temp.ID;

            // inputtoentity(Loadinputdata(temp),
            // GlobalNetworkStuff->RecvEntity);
            // GlobalNetworkStuff->RecvEntity->update();
            // EntityMove(GlobalNetworkStuff->RecvEntity);
            // GlobalNetworkStuff->RecvEntity->deltatimelocal = 0;
          }
        }

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
      } else if (tempdata->name == "DamageEntity") {
        EntityDamageInfo tempinfo;
        auto state = bitsery::quickDeserialization<
            bitsery::InputBufferAdapter<std::vector<uint8_t>>>(
            {tempdata->buffer.begin(), tempdata->size}, tempinfo);
        if (state.first == bitsery::ReaderError::NoError && state.second) {
          if (!tempinfo.IsPlayer && tempinfo.EntityIndex == 0) {
            tempinfo.IsPlayer = true;
            tempinfo.EntityIndex = tempdata->ID;
          } else if (tempinfo.IsPlayer && tempinfo.EntityIndex == UserID) {
            tempinfo.IsPlayer = false;
            tempinfo.EntityIndex = 0;
          }
          DamageEntity(tempinfo, false);
        }
      } else if (tempdata->name == "S2CPlayerData") {
        // SDL_Log("WHAT");
        S2CPlayerInfo tempinfo;

        auto state = bitsery::quickDeserialization<
            bitsery::InputBufferAdapter<std::vector<uint8_t>>>(
            {tempdata->buffer.begin(), tempdata->size}, tempinfo);
        if (state.first == bitsery::ReaderError::NoError && state.second) {
          if (GlobalNetworkStuff->UserIDs.contains(tempinfo.ID))
            GlobalNetworkStuff->PlayerNetStuff[tempinfo.ID].PlayerEntity->hp =
                tempinfo.hp;
          else if (UserID == tempinfo.ID)
            LocalPlayer->hp = tempinfo.hp;
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

            // GlobalNetworkStuff->PlayerNetStuff[tempdata->ID]
            //     .PlayerEntity->deltatimelocal = result;
            GlobalNetworkStuff->PlayerNetStuff[tempdata->ID].deltatimelocal =
                result;
            // if (tempdata->ID == 0) {
            //   LocalPlayer->deltatimelocal =
            //       GlobalNetworkStuff->PlayerNetStuff[0].deltatimelocal;
            // }
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

// send net data
void SendNetData() {
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
        temp.teamindex = ID;
        // temp.teamindex = entity->teamindex;
        temp.ID = ID;
        temp.Set(&player.PlayerInput);
        temp.IsGrounded = entity->IsGrounded;
        for (int i = 0; i < 3; i++) {
          temp.position[i] = entity->position[i];
          temp.velocityvec3[i] = entity->velocityvec3[i];
        }
        auto writtenSize = bitsery::quickSerialization<
            bitsery::OutputBufferAdapter<std::vector<uint8_t>>>({buffer}, temp);

        CobblerQueueData("Player", buffer, writtenSize);
      }
    }

    for (const auto& [ID, player] : GlobalNetworkStuff->PlayerNetStuff) {
      if (ID != UserID) {
        S2CPlayerInfo tempinfo;
        tempinfo.ID = ID;
        tempinfo.hp = player.PlayerEntity->hp;

        std::vector<uint8_t> buffer{};
        auto writtenSize = bitsery::quickSerialization<
            bitsery::OutputBufferAdapter<std::vector<uint8_t>>>({buffer},
                                                                tempinfo);

        CobblerQueueData("S2CPlayerData", buffer, writtenSize);
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
            bitsery::OutputBufferAdapter<std::vector<uint8_t>>>({buffer}, temp);

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

    if (IsServer)
      CobblerQueueData("Player", buffer, writtenSize);
    else
      CobblerQueueConfirmedData(0, "Player", buffer, writtenSize);
    // SDL_Log("%u Send", writtenSize);

    buffer.clear();
  }
  auto writtenSize = bitsery::quickSerialization<
      bitsery::OutputBufferAdapter<std::vector<uint8_t>>>(
      {buffer}, SDL_GetPerformanceCounter());
  CobblerQueueData("SendTick", buffer, writtenSize);

  GlobalNetworkStuff->Onlinesendwait -= updatedeltaTime;
  while (GlobalNetworkStuff->Onlinesendwait <= 0) {
    CobblerSendNet();
    GlobalNetworkStuff->Onlinesendwait += 0.05f;
  }
}

void fixedupdate() {
  if (Global->IsOnline) {
    RecieveNetData();
  }

  if (LocalInputs->Keys[SDL_SCANCODE_ESCAPE] == 2) {
    Global->pause = !Global->pause;
    SDL_SetWindowRelativeMouseMode(RendererGlobal->window, !Global->pause);
  }

  if (!Global->pause) {
    processinputs();
    if (LocalPlayer != NULL) inputtoentity(*P1PlayerInputs, LocalPlayer);

    if (Global->IsOnline && !IsServer) {
      if (GlobalNetworkStuff->RecvEntity != NULL)
        inputtoentity(*P1PlayerInputs, GlobalNetworkStuff->RecvEntity);
    }

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
  while (!EntitydeleteQueue.empty()) {
    uint32_t index = EntitydeleteQueue.front();
    EntitydeleteQueue.pop();
    delete (Entities[index]);
    Entities.erase(index);
  }

  if (Global->IsOnline && !IsServer) {
    if (GlobalNetworkStuff->RecvEntity != NULL) {
      if (glm::distance(LocalPlayer->position,
                        GlobalNetworkStuff->RecvEntity->position) > 1.0f)
        LocalPlayer->position = GlobalNetworkStuff->RecvEntity->position;
      else
        LocalPlayer->position =
            glm::mix(LocalPlayer->position,
                     GlobalNetworkStuff->RecvEntity->position, 0.0625f);

      // if (glm::distance(LocalPlayer->velocityvec3,
      //                   GlobalNetworkStuff->RecvEntity->velocityvec3) > 1.0f)
      //   LocalPlayer->velocityvec3 =
      //   GlobalNetworkStuff->RecvEntity->velocityvec3;
      // else
      //   LocalPlayer->velocityvec3 =
      //       glm::mix(LocalPlayer->velocityvec3,
      //                GlobalNetworkStuff->RecvEntity->velocityvec3, 0.0625f);

      if (64.f > LocalPlayer->velocityvec3.z &&
          LocalPlayer->velocityvec3.z > 1.25f &&
          GlobalNetworkStuff->RecvEntity->velocityvec3.z < 0.125f) {
        GlobalNetworkStuff->RecvEntity->velocityvec3.z =
            LocalPlayer->velocityvec3.z;
        LocalPlayer->velocityvec3 =
            GlobalNetworkStuff->RecvEntity->velocityvec3;
      } else {
        LocalPlayer->velocityvec3 =
            GlobalNetworkStuff->RecvEntity->velocityvec3;
      }
    }
  }

  if (Global->IsOnline) SendNetData();
}

void update() {
  componentsupdatelate();

  while (!ParticledeleteQueue.empty()) {
    uint32_t index = ParticledeleteQueue.front();
    ParticledeleteQueue.pop();
    delete (Particles[index]);
    Particles.erase(index);
  }

  while (!LightdeleteQueue.empty()) {
    uint32_t index = LightdeleteQueue.front();
    LightdeleteQueue.pop();
    delete (Lights[index]);
    Lights.erase(index);
  }

  CameraUpdate();

  // SDL_Log("%f %f %f", Entities[1]->position[0],
  //         Entities[1]->position[1],
  //         Entities[1]->position[2]);

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