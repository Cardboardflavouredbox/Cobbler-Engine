#include "luathing.h"

#include "entity.h"

void LuaSet(sol::state* luathing) {
  luathing->new_usertype<glm::vec3>(
      "vec3", sol::constructors<glm::vec3()>(), "x", &glm::vec3::x, "y",
      &glm::vec3::y, "z", &glm::vec3::z,

      sol::meta_function::index, [](const glm::vec3& v, int idx) -> float {
        if (idx == 1) return v.x;
        if (idx == 2) return v.y;
        if (idx == 3) return v.z;
        throw std::out_of_range("vec3 index out of range");
      });

  luathing->new_usertype<glm::vec2>(
      "vec2", sol::constructors<glm::vec2()>(), "x", &glm::vec2::x, "y",
      &glm::vec2::y,

      sol::meta_function::index, [](const glm::vec2& v, int idx) -> float {
        if (idx == 1) return v.x;
        if (idx == 2) return v.y;
        throw std::out_of_range("vec2 index out of range");
      });

  luathing->new_usertype<glm::quat>(
      "vec2", sol::constructors<glm::vec2()>(), "w", &glm::quat::w, "x",
      &glm::quat::x, "y", &glm::quat::y, "z", &glm::quat::z,

      sol::meta_function::index, [](const glm::quat& v, int idx) -> float {
        if (idx == 1) return v.x;
        if (idx == 2) return v.y;
        if (idx == 3) return v.z;
        if (idx == 4) return v.w;
        throw std::out_of_range("quat index out of range");
      });

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

  luathing->new_usertype<Modeltransform>(
      "Entity", sol::constructors<Modeltransform()>(), "visible",
      &Modeltransform::visible, "name", &Modeltransform::name, "position",
      &Modeltransform::position, "size", &Modeltransform::size, "lookdir",
      &Modeltransform::lookdir, "rot", &Modeltransform::rot);

  luathing->new_usertype<Entity>(
      "Entity", sol::constructors<Entity()>(), "respawn", &Entity::respawn,
      "movespeed", &Entity::movespeed, "jumpheight", &Entity::jumpheight,
      "cameraoffset", &Entity::cameraoffset, "hitboxradius",
      &Entity::hitboxradius, "hitboxlength", &Entity::hitboxlength, "teamindex",
      &Entity::teamindex, "maxhp", &Entity::maxhp, "hp", &Entity::hp,
      "position", &Entity::position, "dir", &Entity::dir, "Modelthing",
      &Entity::Modelthing);
}