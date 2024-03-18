/**
 *    GenshiCraft. Play Genshin Impact in Minecraft!
 *    Copyright (C) 2022  Futrime <futrime@outlook.com>
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Affero General Public License as published
 *    by the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * @file plugin.cc
 * @author Futrime (futrime@outlook.com)
 * @brief Definition of global interfaces and objects
 * @version 1.0.0
 * @date 2022-07-22
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 * @copyright copyright description
 *
 */

#include "plugin.h"

#include <EventAPI.h>
#include <LoggerAPI.h>
#include <ScheduleAPI.h>
#include <ServerAPI.h>

#include <MC/AbstractArrow.hpp>
#include <MC/Actor.hpp>
#include <MC/ActorDamageSource.hpp>
#include <MC/Container.hpp>
#include <MC/ItemStack.hpp>
#include <MC/Player.hpp>
#include <MC/Types.hpp>
#include <chrono>
#include <cmath>
#include <map>
#include <memory>
#include <random>
#include <third-party/Base64/Base64.hpp>
#include <third-party/Nlohmann/json.hpp>

#include "actorex.h"
#include "artifact.h"
#include "character.h"
#include "command.h"
#include "damage.h"
#include "exceptions.h"
#include "food.h"
#include "mobex.h"
#include "playerex.h"
#include "stats.h"
#include "version.h"
#include "weapon.h"
#include "world.h"

namespace genshicraft {

Logger logger(PLUGIN_NAME);

void CheckProtocolVersion() {
#ifdef TARGET_BDS_PROTOCOL_VERSION
  auto currentProtocol = LL::getServerProtocolVersion();
  if (TARGET_BDS_PROTOCOL_VERSION != currentProtocol) {
    logger.warn(
        "Protocol version not match, target version: {}, current version: {}.",
        TARGET_BDS_PROTOCOL_VERSION, currentProtocol);
    logger.warn(
        "This will most likely crash the server, please use the Plugin that "
        "matches the BDS version!");
  }
#endif  // TARGET_BDS_PROTOCOL_VERSION
}

double GetNowClock() {
  return static_cast<double>(
             std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::steady_clock::now().time_since_epoch())
                 .count()) /
         1000;
}

void Init() {
  CheckProtocolVersion();

  Command::Init();

  Event::MobHurtEvent::subscribe_ref(OnMobHurt);
  Event::PlayerDropItemEvent::subscribe_ref(OnPlayerDropItem);
  Event::PlayerExperienceAddEvent::subscribe_ref(OnPlayerExperienceAdd);
  Event::PlayerInventoryChangeEvent::subscribe_ref(OnPlayerInventoryChange);
  Event::PlayerJoinEvent::subscribe_ref(OnPlayerJoin);
  Event::PlayerLeftEvent::subscribe_ref(OnPlayerLeft);
  Event::PlayerOpenContainerEvent::subscribe_ref(OnPlayerOpenContainer);
  Event::PlayerOpenContainerScreenEvent::subscribe_ref(
      OnPlayerOpenContainerScreen);
  Event::PlayerRespawnEvent::subscribe_ref(OnPlayerRespawn);
  Event::PlayerUseItemEvent::subscribe_ref(OnPlayerUseItem);

  Schedule::repeat(OnTick, 1);
}

bool OnMobHurt(Event::MobHurtEvent& event) {
  static const std::map<world::ElementType, std::string> kElementTypeColor = {
      {world::ElementType::kAnemo, "§3"},
      {world::ElementType::kCryo, "§b"},
      {world::ElementType::kDendro, "§a"},
      {world::ElementType::kElectro, "§d"},
      {world::ElementType::kGeo, "§g"},
      {world::ElementType::kHydro, "§9"},
      {world::ElementType::kPhysical, "§f"},
      {world::ElementType::kPyro, "§c"},
  };

  static std::default_random_engine random_engine;
  static std::uniform_int_distribution dist(-10, 1);

  // Override damage directly affects the native health
  if (event.mDamageSource->getCause() == ActorDamageCause::Override) {
    return true;
  }

  int world_level = world::GetWorldLevel(event.mMob->getPosition(),
                                         event.mMob->getDimension());

  Damage damage;

  // Process the damage source
  std::shared_ptr<PlayerEx> attacker_playerex;

  if ((event.mDamageSource->isEntitySource() &&
       event.mDamageSource->getEntity()->getTypeName() == "minecraft:player") ||
      (event.mDamageSource->isEntitySource() &&
       event.mDamageSource->getEntity()->getPlayerOwner() !=
           nullptr)) {  // if the damage is caused by a player

    // Get the player
    if (event.mDamageSource->getEntity()->getPlayerOwner() !=
        nullptr) {  // if the damage is indirectly caused by a player

      attacker_playerex = PlayerEx::Get(
          event.mDamageSource->getEntity()->getPlayerOwner()->getXuid());
    } else {
      attacker_playerex = PlayerEx::Get(
          static_cast<Player*>(event.mDamageSource->getEntity())->getXuid());
    }

    // Check if the PlayerEx object exists
    if (!attacker_playerex) {
      return false;  // the damage should not take effect if the player is not
                     // loaded
    }

    damage = attacker_playerex->GetAttackDamage();

    if (!attacker_playerex->GetWeapon()) {  // if the player does not attack
                                            // with a GenshiCraft weapon
      damage.SetAttackerAmplifier(
          event.mDamage);  // the ratio to attacking unarmed
    }

  } else if (event.mDamageSource->isEntitySource() &&
             event.mDamageSource->getEntity()->getTypeName().substr(0, 10) ==
                 "minecraft:" &&
             !event.mDamageSource->getEntity()
                  ->isPlayer()) {  // if the damage is caused by a native entity
                                   // that is not a player

    std::shared_ptr<ActorEx> actor;

    if (event.mDamageSource->getEntity()->getOwner() !=
        nullptr) {  // if the actor belongs to another actor
      actor = ActorEx::Get(
          event.mDamageSource->getEntity()->getOwner()->getUniqueID());
    } else {
      actor = ActorEx::Get(event.mDamageSource->getEntity()->getUniqueID());
    }

    actor->SetATKByNativeDamage(event.mDamage);

    damage = actor->GetAttackDamage();

  } else {  // if the damage is caused by the environment

    damage.SetSourceType(Damage::SourceType::kEnvironment);

    if (event.mDamageSource->getCause() == ActorDamageCause::Contact ||
        event.mDamageSource->getCause() == ActorDamageCause::Fire ||
        event.mDamageSource->getCause() == ActorDamageCause::FireTick ||
        event.mDamageSource->getCause() == ActorDamageCause::Lava ||
        event.mDamageSource->getCause() == ActorDamageCause::Suffocation ||
        event.mDamageSource->getCause() == ActorDamageCause::Wither) {
      // Reduce the damage of those triggered per tick
      damage.SetTrueDamageProportion(
          0.0025 * event.mDamage);  // damage 5% of the max HP per second
    } else {
      damage.SetTrueDamageProportion(0.05 * event.mDamage);  // damage 5% of the max HP
    }
  }

  // The victim
  int victim_HP = 0;
  int victim_max_HP = 0;

  if (event.mMob->isPlayer()) {  // if the victim is a player

    event.mDamage = 0;  // not to reduce the native Minecraft health

    auto playerex = PlayerEx::Get(static_cast<Player*>(event.mMob)->getXuid());

    if (!playerex) {
      return false;  // the damage should not take effect if the player is not
                     // loaded
    }

    playerex->ApplyDamage(damage);
    damage = playerex->GetLastDamage();

    victim_HP = playerex->GetHP();
    victim_max_HP = playerex->GetStats().GetMaxHP();

  } else if (event.mMob->getTypeName().substr(0, 10) ==
             "minecraft:") {  // if the victim is a Minecraft mob

    auto mobex = MobEx::Get(event.mMob->getUniqueID());

    // Avoid SEH exception when dealing with a dead mob
    if (!mobex) {
      return false;
    }

    // Maintain the native healing
    if (mobex->GetLastNativeHealth() < event.mMob->getHealth()) {
      mobex->IncreaseHP(static_cast<int>(
          (event.mMob->getHealth() - mobex->GetLastNativeHealth()) *
          (1. * mobex->GetStats().GetMaxHP() / event.mMob->getMaxHealth())));
    }
    mobex->SetLastNativeHealth(event.mMob->getHealth());

    mobex->ApplyDamage(damage);
    damage = mobex->GetLastDamage();

    // Calculate the corresponding damage for native health
    if (damage.Get() >
        0.0001) {  // only apply the native damage if the damage succeeded
      event.mDamage = static_cast<float>(
          event.mMob->getHealth() -
          (1. * mobex->GetHP() / mobex->GetStats().GetMaxHP() *
           event.mMob->getMaxHealth()));
    } else {
      event.mDamage = 0;
    }

    if (mobex->GetHP() == 0) {
      event.mDamage = 999999;  // kill the mob instantly
    }

    victim_HP = mobex->GetHP();
    victim_max_HP = mobex->GetStats().GetMaxHP();
  }

  // Zero-damage hurt indicates failed hurt
  if (damage.Get() < 0.0001 && event.mDamage == 0) {
    return false;
  }

  // Show the damage value at the action bar
  if (attacker_playerex) {
    attacker_playerex->GetPlayer()->sendTitlePacket(
        kElementTypeColor.at(damage.GetElementType()) +
            std::to_string(static_cast<int>(damage.Get())) + " §f(" +
            std::to_string(std::max(victim_HP, 0)) + "§7/" +
            std::to_string(victim_max_HP) + "§f)",
        TitleType::SetActionBar, 0, 1, 0);
  }

  return true;
}

bool OnPlayerDropItem(Event::PlayerDropItemEvent& event) {
  // Prevent keeping inventory
  if (event.mPlayer->getHealth() == 0) {
    return true;
  }

  auto playerex = PlayerEx::Get(event.mPlayer->getXuid());
  if (!playerex->GetPlayer()
           ->isSneaking()) {  // if the player is pressing Q without Shift
    if (playerex->GetCharacter()->HasWeapon()) {
      // Perform elemental burst

      return false;
    }
  } else {  // if the player is pressing Shift + Q
    playerex->GetMenu().OpenMain();

    return false;
  }

  return true;
}

bool OnPlayerExperienceAdd(Event::PlayerExperienceAddEvent& event) {
  auto playerex = PlayerEx::Get(event.mPlayer->getXuid());
  playerex->GetCharacter()->IncreaseEnergy(event.mExp);

  return true;
}

bool OnPlayerRespawn(Event::PlayerRespawnEvent& event) {
  PlayerEx::OnPlayerRespawn(event.mPlayer);

  return true;
}

bool OnPlayerInventoryChange(Event::PlayerInventoryChangeEvent& event) {
  auto playerex = PlayerEx::Get(event.mPlayer->getXuid());
  if (!playerex) {  // if the player is not loaded
    return true;
  }

  if (Weapon::CheckIsWeapon(event.mNewItemStack)) {
    Weapon::Make(event.mNewItemStack, playerex.get());
  }

  if (Artifact::CheckIsArtifact(event.mNewItemStack)) {
    Artifact::Make(event.mNewItemStack, playerex.get());
  }

  if (food::CheckIsFood(event.mNewItemStack)) {
    food::RegisterFood(playerex.get(), event.mNewItemStack);
  }

  return true;
}

bool OnPlayerJoin(Event::PlayerJoinEvent& event) {
  PlayerEx::LoadPlayer(event.mPlayer);

  return true;
}

bool OnPlayerLeft(Event::PlayerLeftEvent& event) {
  PlayerEx::UnloadPlayer(event.mPlayer);

  return true;
}

bool OnPlayerOpenContainer(Event::PlayerOpenContainerEvent& event) {
  auto playerex = PlayerEx::Get(event.mPlayer->getXuid());
  playerex->SetIsOpeningContainer(true);

  return true;
}

bool OnPlayerOpenContainerScreen(Event::PlayerOpenContainerScreenEvent& event) {
  auto playerex = PlayerEx::Get(event.mPlayer->getXuid());
  if (!playerex->IsOpeningContainer() &&
      !playerex->GetPlayer()->isSneaking()) {  // if the player is pressing E
                                               // without pressing Shift
    if (playerex->GetCharacter()->HasWeapon()) {
      // Perform elemental skill

      return false;
    }
  }

  playerex->SetIsOpeningContainer(false);
  return true;
}

bool OnPlayerUseItem(Event::PlayerUseItemEvent& event) {
  if (food::CheckIsFood(event.mItemStack)) {
    auto playerex = PlayerEx::Get(event.mPlayer->getXuid());

    return food::EatFood(playerex.get(), event.mItemStack);
  }

  return true;
}

void OnTick() { PlayerEx::OnTick(); }

}  // namespace genshicraft