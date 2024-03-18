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
 * @file mobex.h
 * @author Futrime (futrime@outlook.com)
 * @brief Definition of the MobEx class
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#include "mobex.h"

#include <GlobalServiceAPI.h>

#include <MC/ActorUniqueID.hpp>
#include <MC/Level.hpp>
#include <MC/Mob.hpp>
#include <algorithm>
#include <memory>
#include <random>
#include <third-party/Base64/Base64.hpp>
#include <third-party/Nlohmann/json.hpp>

#include "actorex.h"
#include "damage.h"
#include "exceptions.h"
#include "playerex.h"
#include "plugin.h"
#include "stats.h"
#include "world.h"

namespace genshicraft {

MobEx::MobEx(Mob* mob)
    : ActorEx(mob), HP_(0), latest_damage_(Damage()), last_native_health_(0) {
  // Empty
}

MobEx::~MobEx() { this->SaveData(); }

void MobEx::ApplyDamage(const Damage& damage) {
  this->latest_damage_ = damage;

  this->latest_damage_.SetVictimAttachedElement(world::ElementType::kPhysical);
  this->latest_damage_.SetVictimLevel(this->GetLevel());
  this->latest_damage_.SetVictimStats(this->GetStats());

  this->IncreaseHP(static_cast<int>(-damage.Get()));
}

int MobEx::GetHP() const { return this->HP_; }

Damage MobEx::GetLastDamage() const { return this->latest_damage_; }

int MobEx::GetLastNativeHealth() const { return this->last_native_health_; }

Mob* MobEx::GetMob() const {
  return Global<Level>->getMob(ActorUniqueID(this->GetUniqueID()));
}

void MobEx::IncreaseHP(int value) {
  this->HP_ -= static_cast<int>(std::ceil(this->latest_damage_.Get()));

  // Restrict the HP in a reasonable range
  this->HP_ = std::max(this->HP_, 0);
  this->HP_ = std::min(this->HP_, this->GetStats().GetMaxHP());
}

bool MobEx::IsMob() const { return true; }

void MobEx::SetLastNativeHealth(int health) {
  this->last_native_health_ = health;
}

std::shared_ptr<MobEx> MobEx::Get(long long unique_id) {
  auto mob = Global<Level>->getMob(ActorUniqueID(unique_id));

  return MobEx::Make(mob);
}

std::shared_ptr<MobEx> MobEx::Make(Mob* mob) {
  // Return an empty pointer if the pointer is nullptr
  if (mob == nullptr) {
    return std::shared_ptr<MobEx>();
  }

  auto playerex = PlayerEx::Get(mob->getUniqueID().get());
  if (playerex) {
    return playerex;
  }

  auto mobex = std::make_shared<MobEx>(mob);

  mobex->LoadData();

  return mobex;
}

void MobEx::LoadData() {
  static std::default_random_engine random_engine;

  auto mob = this->GetMob();

  if (mob == nullptr) {
    throw ExceptionMobNotFound();
  }

  nlohmann::json data;  // the mob data

  // Attempt to get the data from tags of the Mob object
  for (const auto& tag : mob->getAllTags()) {
    if (tag.substr(0, 12) == "genshicraft_") {
      try {
        data = nlohmann::json::parse(Base64::Decode(tag.substr(12)));

      } catch (const nlohmann::json::parse_error&) {
        mob->removeTag(tag);  // remove the malformed tag
        continue;
      }

      mob->removeTag(
          tag);  // the tag will be written back at the end of this method
      break;
    }
  }

  // Remove all GenshiCraft data tags
  for (const auto& tag : mob->getAllTags()) {
    if (tag.substr(0, 12) == "genshicraft_") {
      mob->removeTag(tag);
    }
  }

  // Migrate the data and check its validity
  data = MobEx::MigrateData(data);

  // Initialize the mob with template data if the mob is not initialized or the
  // data is invalid
  if (data.empty()) {
    std::uniform_int_distribution dist(-10, 1);

    data["version"] = MobEx::kMobExDataFormatVersion;

    data["level"] =
        world::GetWorldLevel(mob->getPosition(), mob->getDimension()) * 11 +
        dist(random_engine);

    data["max_HP"] = static_cast<int>(
        mob->getMaxHealth() *
        world::GetEnemyMaxHPMultiplier(data["level"].get<int>()) * 3.65);

    data["HP"] = static_cast<int>(data["max_HP"].get<double>() *
                                  mob->getHealth() / mob->getMaxHealth());

    data["ATK"] = 0;  // the ATK cannot be set until the mob performs an attack

    data["last_native_health"] = mob->getHealth();

    // Write the data to a tag of the Mob object
    auto tag = Base64::Encode(nlohmann::to_string(data));
    tag = tag.substr(
        0, tag.find(
               '='));  // remove the "=" characters which are forbidden in tags
    mob->addTag("genshicraft_" + tag);
  }

  this->HP_ = data["HP"].get<int>();
  this->last_native_health_ = data["last_native_health"].get<int>();
  this->level_ = data["level"];
  this->stats_.max_HP_base = data["max_HP"].get<int>();
  this->stats_.ATK_base = data["ATK"].get<int>();
  this->stats_.DEF_base = data["level"].get<int>() * 5 + 500;
}

void MobEx::SaveData() {
  if (this->is_data_saved_) {
    return;
  }

  this->is_data_saved_ = true;

  auto mob = this->GetMob();

  // Nothing to do if the mob no longer exists
  if (mob == nullptr) {
    return;
  }

  // Remove all GenshiCraft data tags
  for (const auto& tag : mob->getAllTags()) {
    if (tag.substr(0, 12) == "genshicraft_") {
      mob->removeTag(tag);
    }
  }

  nlohmann::json data;
  data["version"] = MobEx::kMobExDataFormatVersion;
  data["level"] = this->level_;  // the data value should be the most original
                                 // value so do not use this->GetLevel()
  data["max_HP"] = this->stats_.max_HP_base;
  data["HP"] = this->HP_;
  data["ATK"] = this->stats_.ATK_base;
  data["last_native_health"] = this->last_native_health_;

  // Write the data to a tag of the Mob object
  auto tag = Base64::Encode(nlohmann::to_string(data));
  tag = tag.substr(
      0,
      tag.find('='));  // remove the "=" characters which are forbidden in tags
  mob->addTag("genshicraft_" + tag);
}

nlohmann::json MobEx::MigrateData(const nlohmann::json& old_data) {
  nlohmann::json data = old_data;

  try {
    // Migrate to version 1
    if (!old_data.contains("version")) {
      data["version"] = 1;

      data["last_native_health"] = data["last_minecraft_health"].get<int>();

      data.erase("last_minecraft_health");
    }

    // Check the accessibility
    data["version"].get<int>();
    data["level"].get<int>();
    data["max_HP"].get<int>();
    data["HP"].get<int>();
    data["ATK"].get<int>();
    data["last_native_health"].get<int>();

  } catch (const nlohmann::json::exception&) {
    // Return an empty JSON object if the data is invalid
    return nlohmann::json();
  }

  return data;
}

}  // namespace genshicraft
