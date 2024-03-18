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
 * @file actorex.cc
 * @author Futrime (futrime@outlook.com)
 * @brief Definition of the ActorEx class
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#include "actorex.h"

#include <MC/Actor.hpp>
#include <MC/ActorUniqueID.hpp>
#include <MC/Level.hpp>
#include <memory>
#include <random>
#include <third-party/Base64/Base64.hpp>
#include <third-party/Nlohmann/json.hpp>

#include "damage.h"
#include "exceptions.h"
#include "mobex.h"
#include "plugin.h"
#include "world.h"

namespace genshicraft {

ActorEx::ActorEx(Actor* actor)
    : level_(0), unique_id_(actor->getUniqueID().get()) {
  // Empty
}

ActorEx::~ActorEx() { this->SaveData(); }

int ActorEx::GetLevel() const { return this->level_; }

Actor* ActorEx::GetActor() const {
  return Level::getEntity(ActorUniqueID(this->GetUniqueID()));
}

Damage ActorEx::GetAttackDamage() const {
  Damage damage;
  damage.SetAttackElementType(world::ElementType::kPhysical);
  damage.SetAttackerAmplifier(1.);
  damage.SetAttackerLevel(this->GetLevel());
  damage.SetAttackerStats(this->GetStats());
  damage.SetSourceType(Damage::SourceType::kMob);
  return damage;
}

Stats ActorEx::GetStats() const { return this->stats_; }

long long ActorEx::GetUniqueID() const { return this->unique_id_; }

bool ActorEx::IsMob() const { return false; }

bool ActorEx::IsPlayer() const { return false; }

void ActorEx::SetATKByNativeDamage(double native_damage) {
  this->stats_.ATK_base = static_cast<int>(
      native_damage * world::GetEnemyATKMultiplier(this->GetLevel()) * 18.);
}

std::shared_ptr<ActorEx> ActorEx::Get(long long unique_id) {
  auto actor = Level::getEntity(ActorUniqueID(unique_id));

  return ActorEx::Make(actor);
}

std::shared_ptr<ActorEx> ActorEx::Make(Actor* actor) {
  if (actor == nullptr) {
    return std::shared_ptr<ActorEx>();
  }

  auto mobex = MobEx::Get(actor->getUniqueID().get());
  if (mobex) {
    return mobex;
  }

  auto actorex = std::make_shared<ActorEx>(actor);

  actorex->LoadData();

  return actorex;
}

void ActorEx::LoadData() {
  static std::default_random_engine random_engine;

  auto actor = this->GetActor();

  if (actor == nullptr) {
    throw ExceptionActorNotFound();
  }

  nlohmann::json data;  // the actor data

  // Attempt to get the data from tags of the Actor object
  for (const auto& tag : actor->getAllTags()) {
    if (tag.substr(0, 12) == "genshicraft_") {
      try {
        data = nlohmann::json::parse(Base64::Decode(tag.substr(12)));

      } catch (const nlohmann::json::parse_error&) {
        actor->removeTag(tag);  // remove the malformed tag
        continue;
      }

      actor->removeTag(
          tag);  // the tag will be written back at the end of this method
      break;
    }
  }

  // Remove all GenshiCraft data tags
  for (const auto& tag : actor->getAllTags()) {
    if (tag.substr(0, 12) == "genshicraft_") {
      actor->removeTag(tag);
    }
  }

  // Migrate the data and check its validity
  data = ActorEx::MigrateData(data);

  // Initialize the actor with template data if the actor is not initialized or
  // the data is invalid
  if (data.empty()) {
    std::uniform_int_distribution dist(-10, 1);

    data["version"] = ActorEx::kActorExDataFormatVersion;

    data["level"] =
        world::GetWorldLevel(actor->getPosition(), actor->getDimension()) * 11 +
        dist(random_engine);

    // Write the data to a tag of the Actor object
    auto tag = Base64::Encode(nlohmann::to_string(data));
    tag = tag.substr(
        0, tag.find(
               '='));  // remove the "=" characters which are forbidden in tags
    actor->addTag("genshicraft_" + tag);
  }

  // Load the data
  this->level_ = data["level"];
}

void ActorEx::SaveData() {
  if (this->is_data_saved_) {
    return;
  }

  this->is_data_saved_ = true;

  auto actor = this->GetActor();

  // Nothing to do if the mob no longer exists
  if (actor == nullptr) {
    return;
  }

  // Remove all GenshiCraft data tags
  for (const auto& tag : actor->getAllTags()) {
    if (tag.substr(0, 12) == "genshicraft_") {
      actor->removeTag(tag);
    }
  }

  nlohmann::json data;
  data["version"] = MobEx::kActorExDataFormatVersion;
  data["level"] = this->level_;  // the data value should be the most original
                                 // value so do not use this->GetLevel()

  // Write the data to a tag of the Mob object
  auto tag = Base64::Encode(nlohmann::to_string(data));
  tag = tag.substr(
      0,
      tag.find('='));  // remove the "=" characters which are forbidden in tags
  actor->addTag("genshicraft_" + tag);
}

nlohmann::json ActorEx::MigrateData(const nlohmann::json& old_data) {
  nlohmann::json data = old_data;

  try {
    // Migrate to version 1
    if (!old_data.contains("version")) {
      data["version"] = 1;
    }

    // Check the accessibility
    data["version"].get<int>();
    data["level"].get<int>();

  } catch (const nlohmann::json::exception&) {
    // Return an empty JSON object if the data is invalid
    return nlohmann::json();
  }

  return data;
}

}  // namespace genshicraft