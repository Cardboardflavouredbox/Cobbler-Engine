#include "luathing.h"

#include "entity.h"

void LuaSet(sol::state* luathing) {
  luathing->new_usertype<glm::vec3>(
      "vec3", sol::constructors<glm::vec3()>(), "x", &glm::vec3::x, "y",
      &glm::vec3::y, "z", &glm::vec3::z,

      sol::meta_function::index,
      sol::overload(
          [](const glm::vec3& v, int idx) -> float {
            if (idx == 1) return v.x;
            if (idx == 2) return v.y;
            if (idx == 3) return v.z;
            throw std::out_of_range("vec3 index out of range");
          },
          [](sol::this_state s, const glm::vec3& v,
             const std::string& key) -> sol::object {
            sol::state_view lua(s);
            if (key == "x") return sol::make_object(lua, v.x);
            if (key == "y") return sol::make_object(lua, v.y);
            if (key == "z") return sol::make_object(lua, v.z);
            return sol::lua_nil;
          }),
      sol::meta_function::new_index,
      sol::overload(
          [](glm::vec3& v, int idx, float value) {
            if (idx == 1) {
              v.x = value;
              return;
            }
            if (idx == 2) {
              v.y = value;
              return;
            }
            if (idx == 3) {
              v.z = value;
              return;
            }
            throw std::out_of_range("vec3 index out of range");
          },
          [](glm::vec3& v, const std::string& key, float value) {
            if (key == "x") {
              v.x = value;
              return;
            }
            if (key == "y") {
              v.y = value;
              return;
            }
            if (key == "z") {
              v.z = value;
              return;
            }
            throw std::invalid_argument("Invalid key for vec3");
          }));

  luathing->new_usertype<glm::vec2>(
      "vec2", sol::constructors<glm::vec2()>(), "x", &glm::vec2::x, "y",
      &glm::vec2::y,

      sol::meta_function::index,
      sol::overload(
          [](const glm::vec2& v, int idx) -> float {
            if (idx == 1) return v.x;
            if (idx == 2) return v.y;
            throw std::out_of_range("vec2 index out of range");
          },
          [](sol::this_state s, const glm::vec2& v,
             const std::string& key) -> sol::object {
            sol::state_view lua(s);
            if (key == "x") return sol::make_object(lua, v.x);
            if (key == "y") return sol::make_object(lua, v.y);
            return sol::lua_nil;
          }),
      sol::meta_function::new_index,
      sol::overload(
          [](glm::vec2& v, int idx, float value) {
            if (idx == 1) {
              v.x = value;
              return;
            }
            if (idx == 2) {
              v.y = value;
              return;
            }
            throw std::out_of_range("vec2 index out of range");
          },
          [](glm::vec2& v, const std::string& key, float value) {
            if (key == "x") {
              v.x = value;
              return;
            }
            if (key == "y") {
              v.y = value;
              return;
            }
            throw std::invalid_argument("Invalid key for vec2");
          }));

  // 3. Fixed Quaternion registration
  luathing->new_usertype<glm::quat>(
      "quat", sol::constructors<glm::quat()>(), "w", &glm::quat::w, "x",
      &glm::quat::x, "y", &glm::quat::y, "z", &glm::quat::z,

      sol::meta_function::index,
      sol::overload(
          [](const glm::quat& v, int idx) -> float {
            if (idx == 1) return v.x;
            if (idx == 2) return v.y;
            if (idx == 3) return v.z;
            if (idx == 4) return v.w;
            throw std::out_of_range("quat index out of range");
          },
          [](sol::this_state s, const glm::quat& v,
             const std::string& key) -> sol::object {
            sol::state_view lua(s);
            if (key == "x") return sol::make_object(lua, v.x);
            if (key == "y") return sol::make_object(lua, v.y);
            if (key == "z") return sol::make_object(lua, v.z);
            if (key == "w") return sol::make_object(lua, v.w);
            return sol::lua_nil;
          }),
      sol::meta_function::new_index,
      sol::overload(
          [](glm::quat& v, int idx, float value) {
            if (idx == 1) {
              v.x = value;
              return;
            }
            if (idx == 2) {
              v.y = value;
              return;
            }
            if (idx == 3) {
              v.z = value;
              return;
            }
            if (idx == 4) {
              v.w = value;
              return;
            }
            throw std::out_of_range("quat index out of range");
          },
          [](glm::quat& v, const std::string& key, float value) {
            if (key == "x") {
              v.x = value;
              return;
            }
            if (key == "y") {
              v.y = value;
              return;
            }
            if (key == "z") {
              v.z = value;
              return;
            }
            if (key == "w") {
              v.w = value;
              return;
            }
            throw std::invalid_argument("Invalid key for quat");
          }));

  luathing->new_usertype<EntitySpawnInfo>(
      "EntitySpawnInfo", sol::constructors<EntitySpawnInfo()>(), "teamindex",
      &EntitySpawnInfo::teamindex, "hp", &EntitySpawnInfo::hp, "name",
      &EntitySpawnInfo::name, "EntityIndex", &EntitySpawnInfo::EntityIndex,
      "direction", &EntitySpawnInfo::direction, "position",
      &EntitySpawnInfo::position, "velocityvec3",
      &EntitySpawnInfo::velocityvec3, "State", &EntitySpawnInfo::State,
      "SourceExists", &EntitySpawnInfo::SourceExists, "SourcePlayer",
      &EntitySpawnInfo::SourcePlayer, "SourceEntityIndex",
      &EntitySpawnInfo::SourceEntityIndex);

  luathing->new_usertype<Modeltransform::action::actionpose>(
      "actionpose", sol::constructors<Modeltransform::action::actionpose()>(),
      "lerpamount", &Modeltransform::action::actionpose::lerpamount, "posecode",
      &Modeltransform::action::actionpose::posecode);

  luathing->new_usertype<Modeltransform::action>(
      "action", sol::constructors<Modeltransform::action()>(), "pose",
      &Modeltransform::action::pose, "frame", &Modeltransform::action::frame,
      "speed", &Modeltransform::action::speed);

  luathing->new_usertype<Modeltransform>(
      "Modeltransform", sol::constructors<Modeltransform()>(), "visible",
      &Modeltransform::visible, "name", &Modeltransform::name, "position",
      &Modeltransform::position, "size", &Modeltransform::size, "lookdir",
      &Modeltransform::lookdir, "rot", &Modeltransform::rot, "actions",
      &Modeltransform::actions);

  luathing->new_usertype<Entity>(
      "Entity", sol::constructors<Entity()>(), "respawn", &Entity::respawn,
      "Collided", &Entity::Collided, "gravity", &Entity::gravity, "invincible",
      &Entity::invincible, "movespeed", &Entity::movespeed, "jumpheight",
      &Entity::jumpheight, "cameraoffset", &Entity::cameraoffset, "EntityIndex",
      &Entity::EntityIndex, "hitboxradius", &Entity::hitboxradius,
      "hitboxlength", &Entity::hitboxlength, "teamindex", &Entity::teamindex,
      "maxhp", &Entity::maxhp, "hp", &Entity::hp, "position", &Entity::position,
      "dir", &Entity::dir, "velocityvec3", &Entity::velocityvec3, "State",
      &Entity::State, "movevec2", &Entity::movevec2);
}