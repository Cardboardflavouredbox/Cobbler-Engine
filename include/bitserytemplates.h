#pragma once

#include <bitsery/adapter/buffer.h>
#include <bitsery/bitsery.h>
#include <bitsery/brief_syntax.h>
#include <bitsery/brief_syntax/map.h>
#include <bitsery/brief_syntax/set.h>
#include <bitsery/traits/array.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>

#include "entity.h"
#include "particles.h"
#include "player.h"

template <typename S>
void serialize(S& s, ParticleSpawnInfo& o) {
  s.value4b(o.ParticleCode);
  s.container4b(o.position);
  s.text1b(o.name, 32);
}

template <typename S>
void serialize(S& s, EntityDamageInfo& o) {
  s.value1b(o.IsPlayer);
  s.value8b(o.EntityIndex);
  s.value4b(o.damage);
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

template <typename S>
void serialize(S& s, S2CPlayerInfo& o) {
  s.value8b(o.ID);
  s.value4b(o.hp);
}