#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <string>

#include "deltaTime.h"
#include "entity.h"
#include "font.h"
#include "inputs.h"
#include "pausemenu.h"
#include "player.h"
#include "settings.h"
#include "ui.h"

#ifdef _WIN32
#ifdef DLLEXPORT
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else
#define LIB_API
#endif

LIB_API glm::vec2 getrealpos(UIthing::anchorpos anchor, glm::vec2 pos,
                            glm::vec2 size);

// UIcomponents
struct LIB_API UITextChanger {
  std::u32string* string;
  virtual void update() = 0;
  virtual ~UITextChanger() {}
};

struct LIB_API TextandNumChangerInt : UITextChanger {
  std::u32string text;
  int* num;
  void update() {
    std::string s = std::to_string(*num);
    std::u32string s32(s.begin(), s.end());
    *string = text + s32;
  }
  ~TextandNumChangerInt() {}
};

struct LIB_API TextandNumChangerFloat : UITextChanger {
  std::u32string text;
  float* num;
  void update() {
    std::string s = std::to_string(*num);
    std::u32string s32(s.begin(), s.end());
    *string = text + s32;
  }
  ~TextandNumChangerFloat() {}
};

struct LIB_API HPTextChanger : UITextChanger {
  void update() {
    std::string s = std::to_string((int)std::ceilf(LocalPlayer->hp));
    std::u32string s32(s.begin(), s.end());
    *string = s32;
  }
  ~HPTextChanger() {}
};

struct LIB_API MenuOptionTextChanger : UITextChanger {
  int* index;
  std::u32string* optionname;
  void update() { *string = (*index == menuindex ? U">" : U" ") + *optionname; }
  MenuOptionTextChanger() {}
  ~MenuOptionTextChanger() {}
};

struct LIB_API MenuCheckboxTextChanger : UITextChanger {
  int* index;
  bool* enabled;
  std::u32string* optionname;
  void update() {
    *string = (*index == menuindex ? U">" : U" ") + *optionname +
              (*enabled ? U"▣" : U"□");
  }
  MenuCheckboxTextChanger() {}
  ~MenuCheckboxTextChanger() {}
};

struct LIB_API AmmoTextChanger : UITextChanger {
  void update() {
    float temp = playerluastuff->lua["state"];
    int state = int(temp);

    temp = playerluastuff->lua["enumsize"];
    int enumsize = int(temp);

    int weaponclipammo =
        playerluastuff->lua["weaponclipammo"][(state / enumsize) + 1];

    std::string s = std::to_string(weaponclipammo);
    std::u32string s32(s.begin(), s.end());
    *string = s32;
  }
  ~AmmoTextChanger() {}
};

struct LIB_API ReserveTextChanger : UITextChanger {
  void update() {
    float temp = playerluastuff->lua["state"];
    int state = int(temp);

    temp = playerluastuff->lua["enumsize"];
    int enumsize = int(temp);

    int weaponreserveammo =
        playerluastuff->lua["weaponreserveammo"][(state / enumsize) + 1];

    std::string s = std::to_string(int(weaponreserveammo));
    std::u32string s32(s.begin(), s.end());
    *string = s32;
  }
  ~ReserveTextChanger() {}
};

struct LIB_API UIImageUVIndexChanger {
  int* index;
  virtual void update() = 0;
  virtual ~UIImageUVIndexChanger() {}
};

struct LIB_API ImagePistolChanger : UIImageUVIndexChanger {
  float animlen[3] = {0.f, 0.5f, 2.f}, animprogress = 0.f;
  unsigned char anim = 0;
  void update() {
    if (anim == 0) {
      if (LocalInputs->leftclick > 1) {
        anim = 1;
        animprogress = animlen[1];
      }
    } else {
      animprogress -= renderdeltaTime;
      if (animprogress <= 0) {
        if (anim == 1) {
          anim = 2;
          animprogress = animlen[2] + animprogress;
        } else {
          anim = 0;
        }
      }
    }
    *index = anim;
  }
  ~ImagePistolChanger() {}
};

// UIthings
struct LIB_API UIbox : public UIthing {
  unsigned char color;
  glm::vec4 rgba;
  glm::vec2 size;
  void update() {}
  void render();
  ~UIbox() {}
};

struct LIB_API UITextInputBox : public UIthing {
  bool ismousefocusedonthis = false;
  unsigned char color;
  glm::vec4 rgba;
  glm::vec2 size;
  void update();
  void render();
  ~UITextInputBox() {}
};

struct LIB_API UIimage : public UIthing {  // turn stuff into shared pointers!!
  unsigned char color;
  glm::vec4 rgba;
  UIImageUVIndexChanger* UVIndexChanger = NULL;
  glm::vec2 size;
  std::pair<glm::vec2, glm::vec2>* uvlist;
  std::string texturename;
  int uvindex = 0;
  void update() {
    if (UVIndexChanger != NULL) {
      UVIndexChanger->update();
    }
  }
  void render();
  ~UIimage() {
    if (UVIndexChanger != NULL) delete (UVIndexChanger);
    delete (uvlist);
  }
};

struct LIB_API UItext : public UIthing {
  unsigned char color;
  glm::vec4 rgba;
  float size = 1;
  bool stringstartfromright = false;
  UITextChanger* TextChanger = NULL;
  std::u32string string;
  void update() {
    if (TextChanger != NULL) {
      TextChanger->update();
    }
  }
  void render();
  UItext(UITextChanger* newTextChanger) {
    TextChanger = newTextChanger;
    if (TextChanger != NULL) {
      TextChanger->string = &string;
    }
  }
  ~UItext() {
    if (TextChanger != NULL) {
      delete (TextChanger);
    }
  }
};

struct LIB_API UIDamageIndicator : public UIthing {
  UIDamageIndicator() { anchor = middlemiddle; }
  glm::vec2 size;
  std::string texturename;
  void update() {}
  void render();
  ~UIDamageIndicator() {}
};

struct LIB_API firstpersonUIthing : public UI3Dthing {
  int currentstate = 0;
  firstpersonUIthing() {
    Modeltransform* Arm = new Modeltransform();
    Arm->position = glm::vec3(0, 0, -3.375f);

    Arm->name = "PlayerMain";

    Modeltransform::action actionthing;

    actionthing.pose.posecode.push_back(getposeindex("1stpersonPistol"));

    Arm->actions.push_back(actionthing);
    modeltrans = Arm;
  }

  void update();
};