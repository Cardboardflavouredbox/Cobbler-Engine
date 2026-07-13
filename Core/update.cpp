#include "update.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>

#include <cmath>
#include <glaze/beve.hpp>
#include <glm/glm.hpp>

#include "components.h"
#include "deltaTime.h"
#include "entity.h"
#include "extern.h"
#include "global.h"
#include "network.h"
#include "player.h"

playerinputs Loadinputdata(playerdatapacket input) {
  playerinputs temp;
  for (int i = 0; i < 3; i++) {
    temp.movevec3[i] = input.movevec3[i];
  }
  for (int i = 0; i < 2; i++) {
    temp.lookdir[i] = input.lookdir[i];
  }
  temp.jump = input.jump;
  temp.attack = input.attack;
  temp.altattack = input.altattack;
  return temp;
}

void processinputs() {
  P1PlayerInputs->lookdir.x +=
      -1 * Settings->mousesensitivity.x * LocalInputs->MouseDelta.x;
  P1PlayerInputs->lookdir.y +=
      -1 * Settings->mousesensitivity.y * LocalInputs->MouseDelta.y;

  if (P1PlayerInputs->lookdir.x < 0) P1PlayerInputs->lookdir.x += 360;
  if (P1PlayerInputs->lookdir.x >= 360) P1PlayerInputs->lookdir.x -= 360;
  if (P1PlayerInputs->lookdir.y >= 89) P1PlayerInputs->lookdir.y = 89;
  if (P1PlayerInputs->lookdir.y <= -89) P1PlayerInputs->lookdir.y = -89;

  LocalInputs->MouseDelta.x = 0;
  LocalInputs->MouseDelta.y = 0;
  float ps = std::sin(P1PlayerInputs->lookdir.x * PI / 180.0);
  float pc = std::cos(P1PlayerInputs->lookdir.x * PI / 180.0);

  glm::vec3 tempmove = glm::vec3({0, 0, 0});
  if (LocalInputs->A > 0 && LocalInputs->D == 0) {
    tempmove.x -= std::sin((P1PlayerInputs->lookdir.x + 90) * PI / 180.0);
    tempmove.y += std::cos((P1PlayerInputs->lookdir.x + 90) * PI / 180.0);
  }
  if (LocalInputs->D > 0 && LocalInputs->A == 0) {
    tempmove.x -= std::sin((P1PlayerInputs->lookdir.x - 90) * PI / 180.0);
    tempmove.y += std::cos((P1PlayerInputs->lookdir.x - 90) * PI / 180.0);
  }
  if (LocalInputs->W > 0) {
    tempmove.x -= ps;
    tempmove.y += pc;
  }
  if (LocalInputs->S > 0) {
    tempmove.x += ps;
    tempmove.y -= pc;
  }

  if (tempmove != glm::vec3(0)) tempmove = glm::normalize(tempmove);
  P1PlayerInputs->movevec3 = tempmove;
  P1PlayerInputs->jump = LocalInputs->Space;
}

void update() {
  if (Global->IsOnline) {  // recieve net data
    std::vector<CobblerNetData>* tempvector = CobblerRecvNet();
    if (tempvector != NULL) {
      while (!tempvector->empty()) {
        CobblerNetData* tempdata = &tempvector->back();
        // SDL_Log("%s", tempdata->name.c_str());
        if (tempdata->name == "Player") {
          if (Global->PlayerEntity.contains(tempdata->ID)) {
            playerdatapacket temp;
            auto ec = glz::read_beve(temp, tempdata->buffer);
            if (!ec) {
              Global->PlayerInputList[tempdata->ID] = Loadinputdata(temp);
              for (int i = 0; i < 3; i++) {
                Global->Entities[Global->PlayerEntity[tempdata->ID]]
                    ->velocityvec3[i] = temp.velocityvec3[i];
                Global->Entities[Global->PlayerEntity[tempdata->ID]]
                    ->position[i] = temp.position[i];
              }
              Global->Entities[Global->PlayerEntity[tempdata->ID]]->IsGrounded =
                  temp.IsGrounded;
            } else {
              SDL_Log("what");
            }
          }
        } else if (tempdata->name == "PlayerAdd") {
          if (!CobblerCheckHasIP(tempdata->IP, tempdata->PORT)) {
            Uint64 i = 1;
            while (Global->UserIDs.find(i) != Global->UserIDs.end()) {
              i++;
            }

            Global->UserIDs.insert(i);
            CobblerAddIP(tempdata->IP, tempdata->PORT, i);

            Global->Entities.push_back(SpawnEntities["Policeguy"]());
            Global->PlayerEntity[i] = Global->Entities.size() - 1;
          }
        } else if (tempdata->name == "SendTick") {
          CobblerQueueData("ReturnTick", tempdata->buffer);
        } else if (tempdata->name == "ReturnTick") {
          if (Global->PlayerEntity.contains(tempdata->ID)) {
            Uint64 temp;
            auto ec = glz::read_beve(temp, tempdata->buffer);
            if (!ec) {
              temp = SDL_GetPerformanceCounter() - temp;
              temp /= 2;
              Global->Entities[Global->PlayerEntity[tempdata->ID]]
                  ->deltatimelocal =
                  temp / (double)SDL_GetPerformanceFrequency();
              SDL_Log("%f", Global->Entities[Global->PlayerEntity[tempdata->ID]]
                                ->deltatimelocal);
            }
          }
        }
        tempvector->pop_back();
      }
      delete tempvector;
    }
  }

  lastTime = currentTime;
  currentTime = SDL_GetPerformanceCounter();
  deltaTime = ((double)(currentTime - lastTime)) /
              (double)SDL_GetPerformanceFrequency();
  if (LocalInputs->ESC == 2) {
    Global->pause = !Global->pause;
    SDL_SetWindowRelativeMouseMode(Global->window, !Global->pause);
  }

  if (!Global->pause) {
    processinputs();
    inputtoentity(*P1PlayerInputs, Camera);
    for (const auto& [ID, entity] : Global->PlayerEntity) {
      inputtoentity(Global->PlayerInputList[ID], Global->Entities[entity]);
    }
    PlayerClassUpdate[Global->playerclass]();
    componentsupdate();
    for (auto& [key, value] : Global->UImap3D) {
      for (auto& i : value) {
        i->update();
      }
    }
  }
  componentsupdatelate();

  // SDL_Log("%f %f %f", Global->Entities[1]->position[0],
  //         Global->Entities[1]->position[1],
  //         Global->Entities[1]->position[2]);

  if (Global->IsOnline) {  // send net data
    // if (IsServer) {
    //   std::vector<Uint8> buffer{};
    //   auto ec = glz::write_beve(Global->UserIDs, buffer);
    //   if (!ec) {
    //     CobblerQueueData("PlayerList", buffer);
    //   }
    // }
    std::vector<Uint8> buffer{};
    playerdatapacket temp;
    temp.Set(P1PlayerInputs);
    temp.IsGrounded = Camera->IsGrounded;
    for (int i = 0; i < 3; i++) {
      temp.position[i] = Camera->position[i];
      temp.velocityvec3[i] = Camera->velocityvec3[i];
    }
    auto ec = glz::write_beve(temp, buffer);
    if (!ec) {
      CobblerQueueData("Player", buffer);
    }
    buffer.clear();
    ec = glz::write_beve(SDL_GetPerformanceCounter(), buffer);

    if (!ec) {
      CobblerQueueData("SendTick", buffer);
    }

    Global->Onlinesendwait -= deltaTime;
    while (Global->Onlinesendwait <= 0) {
      CobblerSendNet();
      Global->Onlinesendwait += 0.05f;
    }
  }

  if (curlpostfield->hasdata && Global->LoggedIn) CobblerSendCurlData();
}