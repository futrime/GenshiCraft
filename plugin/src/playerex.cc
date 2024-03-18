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
 * @file playerex.cc
 * @author Futrime (futrime@outlook.com)
 * @brief Definition of the PlayerEx class
 * @version 1.0.0
 * @date 2022-07-23
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#include "playerex.h"

#include <EventAPI.h>
#include <GlobalServiceAPI.h>
#include <KVDBAPI.h>
#include <ScheduleAPI.h>

#include <MC/ActorUniqueID.hpp>
#include <MC/Block.hpp>
#include <MC/Container.hpp>
#include <MC/ItemStack.hpp>
#include <MC/Level.hpp>
#include <MC/MobEffect.hpp>
#include <MC/Player.hpp>
#include <MC/SimpleContainer.hpp>
#include <MC/Types.hpp>
#include <algorithm>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <third-party/Nlohmann/json.hpp>
#include <vector>

#include "artifact.h"
#include "character.h"
#include "damage.h"
#include "exceptions.h"
#include "menu.h"
#include "mobex.h"
#include "plugin.h"
#include "sidebar.h"
#include "stats.h"
#include "weapon.h"
#include "world.h"

namespace genshicraft {

PlayerEx::PlayerEx(Player* player)
    : MobEx(player),
      is_opening_container_(false),
      last_world_level_(0),
      menu_(Menu(this)),
      sidebar_(Sidebar(this)),
      stamina_(0),
      stamina_max_(0),
      xuid_(player->getXuid()) {
  // Empty
}

PlayerEx::~PlayerEx() { this->SaveData(); }

void PlayerEx::ApplyDamage(const Damage& damage) {
  this->latest_damage_ = damage;

  this->latest_damage_.SetVictimAttachedElement(world::ElementType::kPhysical);
  this->latest_damage_.SetVictimLevel(this->GetLevel());
  this->latest_damage_.SetVictimStats(this->GetStats());

  this->GetCharacter()->IncreaseHP(
      static_cast<int>(-std::ceil(this->latest_damage_.Get())));
}

void PlayerEx::ConsumeItem(std::string identifier, int value) {
  // Check if the items are enough for consumption
  if (this->GetItemCount(identifier) < value) {
    throw ExceptionItemsNotEnough();
  }

  // This function is only for item consumption
  if (value <= 0) {
    return;
  }

  auto& inventory = this->GetPlayer()->getInventory();
  for (int i = 0; i < inventory.getSize(); ++i) {
    if (inventory.getSlot(i)->getTypeName() == identifier) {
      int consumed_value = std::min(value, inventory.getSlot(i)->getCount());
      inventory.removeItem_s(i, consumed_value);
      value -= consumed_value;
      if (value <= 0) {
        break;
      }
    }
  }

  this->RefreshItems();
}

void PlayerEx::ConsumeMora(int value) {
  int mora_count = this->GetMoraCount();

  // Check if the mora is enough
  if (mora_count < value) {
    throw ExceptionMoraNotEnough();
  }

  mora_count -= value;

  // Clear all mora
  this->GetPlayer()->clearItem("genshicraft:mora_10000");
  this->GetPlayer()->clearItem("genshicraft:mora_5000");
  this->GetPlayer()->clearItem("genshicraft:mora_1000");
  this->GetPlayer()->clearItem("genshicraft:mora_500");
  this->GetPlayer()->clearItem("genshicraft:mora_100");
  this->GetPlayer()->clearItem("genshicraft:mora_50");
  this->GetPlayer()->clearItem("genshicraft:mora_10");
  this->GetPlayer()->clearItem("genshicraft:mora_5");
  this->GetPlayer()->clearItem("genshicraft:mora_1");

  // Give the mora left back
  this->GiveItem("genshicraft:mora_1", mora_count % 10);
  mora_count /= 10;
  this->GiveItem("genshicraft:mora_10", mora_count % 10);
  mora_count /= 10;
  this->GiveItem("genshicraft:mora_100", mora_count % 10);
  mora_count /= 10;
  this->GiveItem("genshicraft:mora_1000", mora_count % 10);
  mora_count /= 10;
  this->GiveItem("genshicraft:mora_10000", mora_count % 10);
}

std::vector<std::shared_ptr<Character>> PlayerEx::GetAllCharacters() const {
  return this->character_owned_;
}

std::map<Artifact::Type, std::shared_ptr<Artifact>> PlayerEx::GetArtifactDict()
    const {
  std::vector<ItemStack*> item_list;
  item_list.push_back(this->GetPlayer()->getArmorContainer().getSlot(0));
  item_list.push_back(this->GetPlayer()->getArmorContainer().getSlot(1));
  item_list.push_back(this->GetPlayer()->getArmorContainer().getSlot(2));
  item_list.push_back(this->GetPlayer()->getArmorContainer().getSlot(3));
  item_list.push_back(
      const_cast<ItemStack*>(&(this->GetPlayer()->getOffhandSlot())));

  std::map<Artifact::Type, std::shared_ptr<Artifact>> artifact_dict;

  for (auto&& item : item_list) {
    if (Artifact::CheckIsArtifact(item)) {
      auto artifact = Artifact::Make(item, const_cast<PlayerEx*>(this));
      artifact_dict[artifact->GetType()] = artifact;
    }
  }

  return artifact_dict;
}

Damage PlayerEx::GetAttackDamage() const {
  if (this->GetWeapon()) {  // if the player attacks with a GenshiCraft weapon
    return this->GetCharacter()->GetDamageNormalAttack();
  } else {
    static double last_attack_clock =
        GetNowClock() - 999999.;  // the clock of the last attack

    // Prevent too frequent attack
    if (GetNowClock() - last_attack_clock < 0.5) {
      return Damage();
    }
    last_attack_clock = GetNowClock();

    Stats stats;
    stats.ATK_base = static_cast<int>(this->GetStats().GetATK() * 0.1974);

    Damage damage;
    damage.SetAttackElementType(world::ElementType::kPhysical);
    damage.SetAttackerAmplifier(1.);
    damage.SetAttackerLevel(this->GetLevel());
    damage.SetAttackerStats(stats);
    damage.SetSourceType(Damage::SourceType::kMob);

    return damage;
  }
}

std::shared_ptr<Character> PlayerEx::GetCharacter() const {
  return this->character_;
}

int PlayerEx::GetHP() const { return this->GetCharacter()->GetHP(); }

int PlayerEx::GetItemCount(std::string identifier) const {
  auto& inventory = this->GetPlayer()->getInventory();

  int item_count = 0;
  for (auto&& item : inventory.getAllSlots()) {
    if (identifier == item->getTypeName()) {
      item_count += item->getCount();
    }
  }

  return item_count;
}

int PlayerEx::GetLastNativeHealth() const { throw ExceptionMethodNotAllowed(); }

int PlayerEx::GetLevel() const { return this->GetCharacter()->GetLevel(); }

Stats PlayerEx::GetStats() const { return this->GetCharacter()->GetStats(); }

Menu& PlayerEx::GetMenu() { return this->menu_; }

int PlayerEx::GetMoraCount() const {
  int mora_count = 0;
  mora_count += this->GetItemCount("genshicraft:mora_10000") * 10000;
  mora_count += this->GetItemCount("genshicraft:mora_5000") * 5000;
  mora_count += this->GetItemCount("genshicraft:mora_1000") * 1000;
  mora_count += this->GetItemCount("genshicraft:mora_500") * 500;
  mora_count += this->GetItemCount("genshicraft:mora_100") * 100;
  mora_count += this->GetItemCount("genshicraft:mora_50") * 50;
  mora_count += this->GetItemCount("genshicraft:mora_10") * 10;
  mora_count += this->GetItemCount("genshicraft:mora_5") * 5;
  mora_count += this->GetItemCount("genshicraft:mora_1") * 1;

  return mora_count;
}

Player* PlayerEx::GetPlayer() const {
  return Global<Level>->getPlayer(this->xuid_);
}

int PlayerEx::GetStamina() const { return this->stamina_; }

int PlayerEx::GetStaminaMax() const { return this->stamina_max_; }

std::shared_ptr<Weapon> PlayerEx::GetWeapon() const {
  auto mainhand_item = this->GetPlayer()->getHandSlot();
  if (Weapon::CheckIsWeapon(mainhand_item)) {
    return Weapon::Make(mainhand_item, const_cast<PlayerEx*>(this));
  } else {
    return std::shared_ptr<Weapon>();  // empty pointer
  }
}

const std::string& PlayerEx::GetXUID() const { return this->xuid_; }

void PlayerEx::GiveItem(const std::string& identifier, int value) {
  // This function is only for giving items
  if (value <= 0) {
    return;
  }

  auto item = ItemStack::create(identifier, value);
  this->GetPlayer()->giveItem(item);
}

void PlayerEx::IncreaseHP(int value) {
  this->GetCharacter()->IncreaseHP(value);
}

void PlayerEx::IncreaseStamina(int value) {
  this->stamina_ += value;
  this->stamina_ = std::max(this->stamina_, 0);
  this->stamina_ = std::min(this->stamina_, this->stamina_max_);
}

bool PlayerEx::IsOpeningContainer() const {
  return this->is_opening_container_;
}

bool PlayerEx::IsPlayer() const { return true; }

void PlayerEx::RefreshItems() const {
  auto xuid = this->xuid_;
  // Refresh next tick to ensure that the data of the items are updated
  Schedule::nextTick([xuid]() {
    if (PlayerEx::Get(xuid)) {  // ensure that the player is online
      PlayerEx::Get(xuid)->GetPlayer()->refreshInventory();
    }
  });
}

void PlayerEx::SelectCharacter(int no) {
  if (no >= this->GetAllCharacters().size()) {
    throw ExceptionCharacterNumberOutOfRange();
  }

  this->character_ = this->GetAllCharacters().at(no);
}

void PlayerEx::SetATKByNativeDamage(double native_damage) {
  throw ExceptionMethodNotAllowed();
}

void PlayerEx::SetIsOpeningContainer(bool is_opening_container) {
  this->is_opening_container_ = is_opening_container;
}

void PlayerEx::SetLastNativeHealth(int health) {
  throw ExceptionMethodNotAllowed();
}

std::shared_ptr<PlayerEx> PlayerEx::Get(long long unique_id) {
  auto player = Global<Level>->getPlayer(ActorUniqueID(unique_id));

  if (player == nullptr) {
    return std::shared_ptr<PlayerEx>();
  }

  return PlayerEx::Get(player->getXuid());
}

std::shared_ptr<PlayerEx> PlayerEx::Get(const std::string& xuid) {
  auto pred = [xuid](const std::shared_ptr<PlayerEx>& playerex) {
    return (playerex->GetXUID() == xuid);
  };  // the predicate
  auto iter = std::find_if(PlayerEx::all_playerex_.begin(),
                           PlayerEx::all_playerex_.end(), pred);

  if (iter == PlayerEx::all_playerex_.end()) {  // if not found
    return std::shared_ptr<PlayerEx>();
  }

  return *iter;
}

std::vector<std::shared_ptr<PlayerEx>>& PlayerEx::GetAll() {
  return PlayerEx::all_playerex_;
}

void PlayerEx::LoadPlayer(Player* player) {
  if (!PlayerEx::Get(player->getXuid())) {  // to prevent duplicated load
    auto playerex = std::make_shared<PlayerEx>(player);
    playerex->LoadData();
    PlayerEx::all_playerex_.push_back(playerex);
  }
}

void PlayerEx::OnPlayerRespawn(Player* player) {
  auto playerex = PlayerEx::Get(player->getXuid());
  for (auto&& character : playerex->character_owned_) {
    character->Revive();
    character->IncreaseHP(999999);  // increase to full HP
  }
}

void PlayerEx::OnTick() {
  static std::default_random_engine random_engine;
  static std::uniform_real_distribution<> dist(0, 1);

  for (auto&& playerex : PlayerEx::all_playerex_) {
    // Maintain the world level notice
    int world_level =
        world::GetWorldLevel(playerex->GetPlayer()->getPosition(),
                             playerex->GetPlayer()->getDimension());
    if (world_level != playerex->last_world_level_) {
      if (world_level * 11 - 10 > playerex->GetLevel() + 10) {
        playerex->GetPlayer()->sendTitlePacket("§cHighly Dangerous",
                                               TitleType::SetSubtitle, 0, 1, 0);
      } else if (world_level * 11 - 10 > playerex->GetLevel()) {
        playerex->GetPlayer()->sendTitlePacket("§6Dangerous",
                                               TitleType::SetSubtitle, 0, 1, 0);
      }

      playerex->GetPlayer()->sendTitlePacket(
          "World Level " + std::to_string(world_level), TitleType::SetTitle, 0,
          1, 0);

      playerex->last_world_level_ = world_level;
    }

    // Maintain the stamina
    if (playerex->GetPlayer()->isSprinting() &&
        !playerex->GetPlayer()->isSwimming()) {
      // Prevent sprinting if the stamina is used up
      if (playerex->stamina_ == 0) {
        playerex->GetPlayer()->setSprinting(false);
      }

      // Reduce 18 stamina per second when sprinting
      if (dist(random_engine) < 0.9) {
        playerex->IncreaseStamina(-1);
      }

    } else if (playerex->GetPlayer()->isSwimming()) {
      // The player drowns if the stamina is used up
      if (playerex->stamina_ == 0) {
        world::HurtActor(playerex->GetPlayer(), 999999., ActorDamageCause::Override);
      }

      // Reduce 10.2 stamina per second when swimming dash
      if (dist(random_engine) < 0.51) {
        playerex->IncreaseStamina(-1);
      }

    } else if (playerex->GetPlayer()->isInWater() &&
               !playerex->GetPlayer()->isOnGround() &&
               playerex->GetPlayer()->isMoving()) {
      // The player drowns if the stamina is used up
      if (playerex->stamina_ == 0) {
        world::HurtActor(playerex->GetPlayer(), 999999., ActorDamageCause::Override);
      }

      // Reduce 4 stamina per second when swimming
      if (dist(random_engine) < 0.51) {
        playerex->IncreaseStamina(-1);
      }

    } else if (playerex->GetPlayer()->isInWater() &&
               !playerex->GetPlayer()->isOnGround()) {
      // The player drowns if the stamina is used up
      if (playerex->stamina_ == 0) {
        world::HurtActor(playerex->GetPlayer(), 999999., ActorDamageCause::Override);
      }
    } else {
      // Regenerate stamina when idle
      // Regenerate 25 stamina per second when idle and on ground
      if (dist(random_engine) < 0.25) {
        playerex->IncreaseStamina(2);
      } else {
        playerex->IncreaseStamina(1);
      }
    }

    // Maintain the character survival
    if (playerex->character_->GetHP() ==
        0) {  // if the current character is dead
      playerex->character_->IncreaseEnergy(-999999);  // Clear the energy

      bool is_switched = false;
      for (auto&& character : playerex->character_owned_) {
        // Switch to the first character alive
        if (character->GetHP() != 0) {
          playerex->character_ = character;
        }
      }
      if (!is_switched) {  // if every character is dead
        world::HurtActor(playerex->GetPlayer(), 999999., ActorDamageCause::Override);
      }
    }

    // Maintain the character fullness
    for (auto character : playerex->character_owned_) {
      character->IncreaseFullness(-0.015);
    }

    // Maintain the native health
    if (playerex->GetPlayer()->getHealth() <
        playerex->GetPlayer()
            ->getMaxHealth()) {  // if the player loses health abnormally
      playerex->GetPlayer()->heal(20);
    }

    // Maintain the wither effect (temporary, for this is just a bug of
    // Minecraft)
    if (playerex->GetPlayer()->hasEffect(*MobEffect::WITHER)) {
      world::HurtActor(playerex->GetPlayer(), 1., ActorDamageCause::Wither);
    }

    // Refresh the sidebar
    playerex->sidebar_.Refresh();
  }
}

void PlayerEx::UnloadPlayer(Player* player) {
  if (PlayerEx::Get(player->getXuid())) {  // to prevent duplicated unload
    auto& all_playerex = PlayerEx::GetAll();
    for (auto it = all_playerex.begin(); it != all_playerex.end(); ++it) {
      if ((*it)->GetXUID() == player->getXuid()) {
        all_playerex.erase(it);
        break;
      }
    }
  }
}

const nlohmann::json PlayerEx::kPlayerDataTemplate = R"(
  {
    "character": "Kuki Shinobu",
    "character_owned": [
      {
        "name": "Kuki Shinobu",
        "ascension_phase": 0,
        "character_EXP": 0,
        "constellation": 0,
        "energy": 0,
        "HP": 1030,
        "talent_elemental_burst_level": 1,
        "talent_elemental_skill_level": 1,
        "talent_normal_attack_level": 1
      }
    ],
    "stamina_max": 100
  }
)"_json;

void PlayerEx::LoadData() {
  auto players_db = KVDB::open("plugins/GenshiCraft/db/players");

  nlohmann::json data;

  // Attempt to get the data from the database
  std::string data_str;
  players_db->get(this->xuid_, data_str);
  try {
    data = nlohmann::json::parse(data_str);
  } catch (const nlohmann::json::parse_error&) {
    // Empty
  }

  // Migrate the data and check its validity
  data = PlayerEx::MigrateData(data);

  if (data.empty()) {
    data = PlayerEx::kPlayerDataTemplate;

    // Save the data
    players_db->set(this->xuid_, data.dump());
  }

  // Load the data
  this->character_owned_.clear();
  for (const auto& character_data : data["character_owned"]) {
    auto name = character_data["name"].get<std::string>();
    auto ascension_phase = character_data["ascension_phase"].get<int>();
    auto character_EXP = character_data["character_EXP"].get<int>();
    auto constellation = character_data["constellation"].get<int>();
    auto energy = character_data["energy"].get<int>();
    auto HP = character_data["HP"].get<int>();
    auto talent_elemental_burst_level =
        character_data["talent_elemental_burst_level"].get<int>();
    auto talent_elemental_skill_level =
        character_data["talent_elemental_skill_level"].get<int>();
    auto talent_normal_attack_level =
        character_data["talent_normal_attack_level"].get<int>();

    auto character = Character::Make(
        this, name, ascension_phase, character_EXP, constellation, energy, HP,
        talent_elemental_burst_level, talent_elemental_skill_level,
        talent_normal_attack_level);

    this->character_owned_.push_back(character);

    if (name == data["character"].get<std::string>()) {
      this->character_ = character;
    }
  }
  this->stamina_max_ = data["stamina_max"].get<int>();
  this->stamina_ = this->stamina_max_;
}

void PlayerEx::SaveData() {
  if (this->is_data_saved_) {
    return;
  }

  this->is_data_saved_ = true;

  auto players_db = KVDB::open("plugins/GenshiCraft/db/players");

  nlohmann::json data;
  data["version"] = PlayerEx::kPlayerExDataFormatVersion;
  data["character"] = this->GetCharacter()->GetName();
  data["character_owned"] = nlohmann::json::array();
  for (const auto& character : this->GetAllCharacters()) {
    nlohmann::json character_data;
    character_data["name"] = character->GetName();
    character_data["ascension_phase"] = character->GetAscensionPhase();
    character_data["character_EXP"] = character->GetCharacterEXP();
    character_data["constellation"] = character->GetConstellation();
    character_data["energy"] = character->GetEnergy();
    character_data["HP"] = character->GetHP();
    character_data["talent_elemental_burst_level"] =
        character->GetTalentElementalBurstLevel();
    character_data["talent_elemental_skill_level"] =
        character->GetTalentElementalSkillLevel();
    character_data["talent_normal_attack_level"] =
        character->GetTalentNormalAttackLevel();

    data["character_owned"].push_back(character_data);
  }

  data["stamina_max"] = this->stamina_max_;

  players_db->set(this->xuid_, data.dump());
}

nlohmann::json PlayerEx::MigrateData(const nlohmann::json& old_data) {
  nlohmann::json data = old_data;

  try {
    // Migrate to version 1
    if (!old_data.contains("version")) {
      data["version"] = 1;
    }

    // Check the accessibility
    data["version"].get<int>();
    data["character"].get<std::string>();
    data["character"][0];  // the array should contain at least one element
    for (auto&& character_data : data["character_owned"]) {
      character_data["name"].get<std::string>();
      character_data["ascension_phase"].get<int>();
      character_data["character_EXP"].get<int>();
      character_data["constellation"].get<int>();
      character_data["energy"].get<int>();
      character_data["HP"].get<int>();
      character_data["talent_elemental_burst_level"].get<int>();
      character_data["talent_elemental_skill_level"].get<int>();
      character_data["talent_normal_attack_level"].get<int>();
    }

  } catch (const nlohmann::json::exception&) {
    // Return an empty JSON object if the data is invalid
    return nlohmann::json();
  }

  return data;
}

std::vector<std::shared_ptr<PlayerEx>> PlayerEx::all_playerex_ = {};

}  // namespace genshicraft