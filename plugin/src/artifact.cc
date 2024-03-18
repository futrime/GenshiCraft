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
 * @file artifact.cc
 * @author Futrime (futrime@outlook.com)
 * @brief Definition of the Artifact class
 * @version 1.0.0
 * @date 2022-08-17
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#include "artifact.h"

#include <EventAPI.h>
#include <ScheduleAPI.h>

#include <MC/CompoundTag.hpp>
#include <MC/ItemStack.hpp>
#include <MC/ListTag.hpp>
#include <MC/Player.hpp>
#include <algorithm>
#include <iomanip>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "character.h"
#include "exceptions.h"
#include "playerex.h"
#include "plugin.h"
#include "stats.h"

namespace genshicraft {

Artifact::Artifact(ItemStack* item, PlayerEx* playerex)
    : item_(item), playerex_(playerex) {
  // Check if the item is a GenshiCraft item
  if (!Artifact::CheckIsArtifact(item)) {
    throw ExceptionNotAnArtifact();
  }

  this->identifier_ =
      item->getTypeName().substr(0, item->getTypeName().size() - 2);

  this->rarity_ =
      std::stoi(item->getTypeName().substr(item->getTypeName().size() - 1));
  this->rarity_ = std::max(this->rarity_, 1);
  this->rarity_ = std::min(this->rarity_, 5);

  auto nbt = item->getNbt();

  if (!nbt->contains("tag") || !nbt->getCompound("tag")->contains(
                                   "genshicraft")) {  // if not initialized
    // Initialize the artifact
    this->artifact_exp_ = 0;

    this->InitStats();

    // Write to the item NBT
    nbt->put("tag", CompoundTag());

    auto tag = nbt->getCompound("tag");
    tag->put("genshicraft", CompoundTag());

    auto data = tag->getCompound("genshicraft");
    data->putInt("artifact_exp", this->artifact_exp_);  // the Artifact EXP

    data->put("main_stat", CompoundTag());
    data->getCompound("main_stat")
        ->putInt("type", static_cast<int>(this->main_stat_.type));
    data->getCompound("main_stat")->putDouble("value", this->main_stat_.value);

    for (int i = 0; i < 4; ++i) {
      data->put("sub_stat_" + std::to_string(i), CompoundTag());
      data->getCompound("sub_stat_" + std::to_string(i))
          ->putInt("type", static_cast<int>(this->sub_stat_list_.at(i).type));
      data->getCompound("sub_stat_" + std::to_string(i))
          ->putDouble("value", this->sub_stat_list_.at(i).value);
    }

    item->setNbt(nbt.get());

    playerex->RefreshItems();
  }

  auto data = nbt->getCompound("tag")->getCompound("genshicraft");

  this->artifact_exp_ = data->getInt("artifact_exp");

  this->main_stat_.type = static_cast<Artifact::StatType>(
      data->getCompound("main_stat")->getInt("type"));
  this->main_stat_.value = data->getCompound("main_stat")->getDouble("value");

  this->sub_stat_list_.clear();
  for (int i = 0; i < 4; ++i) {
    Artifact::StatItem stat;

    stat.type = static_cast<Artifact::StatType>(
        data->getCompound("sub_stat_" + std::to_string(i))->getInt("type"));

    stat.value =
        data->getCompound("sub_stat_" + std::to_string(i))->getDouble("value");

    this->sub_stat_list_.push_back(stat);
  }

  this->ApplyLore();
}

Artifact::~Artifact() {
  auto nbt = this->item_->getNbt();
  auto data = nbt->getCompound("tag")->getCompound("genshicraft");

  bool is_modified = false;
  if (this->artifact_exp_ != data->getInt("artifact_exp")) {
    is_modified = true;
  }

  data->putInt("artifact_exp", this->artifact_exp_);  // the Artifact EXP

  data->put("main_stat", CompoundTag());
  data->getCompound("main_stat")
      ->putInt("type", static_cast<int>(this->main_stat_.type));
  data->getCompound("main_stat")->putDouble("value", this->main_stat_.value);

  for (int i = 0; i < 4; ++i) {
    data->put("sub_stat_" + std::to_string(i), CompoundTag());
    data->getCompound("sub_stat_" + std::to_string(i))
        ->putInt("type", static_cast<int>(this->sub_stat_list_.at(i).type));
    data->getCompound("sub_stat_" + std::to_string(i))
        ->putDouble("value", this->sub_stat_list_.at(i).value);
  }

  if (is_modified) {
    this->item_->setNbt(nbt.get());
    this->playerex_->RefreshItems();
  }
}

void Artifact::ApplyLore() {
  // Set the lore
  std::vector<std::string> lore;

  if (this->GetType() == Artifact::Type::kFlowerOfLife) {
    lore.push_back("§7Flower of Life");
  } else if (this->GetType() == Artifact::Type::kPlumeOfDeath) {
    lore.push_back("§7Plume of Death");
  } else if (this->GetType() == Artifact::Type::kSandsOfEon) {
    lore.push_back("§7Sands of Eon");
  } else if (this->GetType() == Artifact::Type::kGobletOfEonothem) {
    lore.push_back("§7Goblet of Eonothem");
  } else if (this->GetType() == Artifact::Type::kCircletOfLogos) {
    lore.push_back("§7Circle of Logos");
  }

  auto stats_description = this->GetBaseStatsDescription();

  lore.push_back("§f" + stats_description.at(0));

  std::string rarity_line = "§6";
  for (int i = 0; i < this->GetRarity(); ++i) {
    rarity_line += "★";
  }
  lore.push_back(rarity_line);

  lore.push_back("§f+" + std::to_string(this->GetLevel()));

  for (int i = 1; i < stats_description.size(); ++i) {
    lore.push_back("§f" + stats_description.at(i));
  }

  // Prevent duplicated refresh
  auto old_lore = this->item_->getCustomLore();
  bool is_updated = false;
  if (old_lore.size() != lore.size()) {
    is_updated = true;
  } else {
    for (int i = 0; i < lore.size(); ++i) {
      if (old_lore.at(i) != lore.at(i)) {
        is_updated = true;
      }
    }
  }
  if (is_updated) {
    this->item_->setCustomLore(lore);
    this->playerex_->RefreshItems();
  }
}

int Artifact::GetArtifactEXP() const { return this->artifact_exp_; }

int Artifact::GetBaseConsumableEXP() const {
  return Artifact::kRarityBaseConsumableEXPList[this->rarity_];
};

Stats Artifact::GetBaseStats() const {
  Stats stats;

  std::vector<Artifact::StatItem> stat_list = {
      this->main_stat_, this->sub_stat_list_[0], this->sub_stat_list_[1],
      this->sub_stat_list_[2], this->sub_stat_list_[3]};

  for (const auto& stat : stat_list) {
    switch (stat.type) {
      case Artifact::StatType::kHP:
        stats.max_HP_ext = static_cast<int>(stat.value);
        break;

      case Artifact::StatType::kHPPercent:
        stats.max_HP_percent = stat.value / 100;
        break;

      case Artifact::StatType::kATK:
        stats.ATK_ext = static_cast<int>(stat.value);
        break;

      case Artifact::StatType::kATKPercent:
        stats.ATK_percent = stat.value / 100;
        break;

      case Artifact::StatType::kDEF:
        stats.DEF_ext = static_cast<int>(stat.value);
        break;

      case Artifact::StatType::kDEFPercent:
        stats.DEF_percent = stat.value / 100;
        break;

      case Artifact::StatType::kElementalMastery:
        stats.elemental_mastery = static_cast<int>(stat.value);
        break;

      case Artifact::StatType::kCritRate:
        stats.CRIT_rate = stat.value / 100;
        break;

      case Artifact::StatType::kCritDMG:
        stats.CRIT_DMG = stat.value / 100;
        break;

      case Artifact::StatType::kHealingBonus:
        stats.healing_bonus = stat.value / 100;
        break;

      case Artifact::StatType::kEnergyRecharge:
        stats.energy_recharge = stat.value / 100;
        break;

      case Artifact::StatType::kPyroDMG:
        stats.pyro_DMG_bonus = stat.value / 100;
        break;

      case Artifact::StatType::kHydroDMG:
        stats.hydro_DMG_bonus = stat.value / 100;
        break;

      case Artifact::StatType::kDendroDMG:
        stats.dendro_DMG_bonus = stat.value / 100;
        break;

      case Artifact::StatType::kElectroDMG:
        stats.electro_DMG_bonus = stat.value / 100;
        break;

      case Artifact::StatType::kAnemoDMG:
        stats.anemo_DMG_bonus = stat.value / 100;
        break;

      case Artifact::StatType::kCryoDMG:
        stats.cryo_DMG_bonus = stat.value / 100;
        break;

      case Artifact::StatType::kGeoDMG:
        stats.geo_DMG_bonus = stat.value / 100;
        break;

      case Artifact::StatType::kPhysicalDMG:
        stats.physical_DMG_bonus = stat.value / 100;
        break;

      default:
        break;
    }
  }

  return stats;
}

std::vector<std::string> Artifact::GetBaseStatsDescription() const {
  std::vector<std::string> description;

  auto DoubleToString = [](double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << value;
    return oss.str();
  };

  switch (this->main_stat_.type) {
    case Artifact::StatType::kHP:
      description.push_back(
          "§fHP: " + std::to_string(static_cast<int>(this->main_stat_.value)));
      break;

    case Artifact::StatType::kHPPercent:
      description.push_back("§fHP: " + DoubleToString(this->main_stat_.value) +
                            "%");
      break;

    case Artifact::StatType::kATK:

      description.push_back(
          "§fATK: " + std::to_string(static_cast<int>(this->main_stat_.value)));
      break;

    case Artifact::StatType::kATKPercent:
      description.push_back("§fATK: " + DoubleToString(this->main_stat_.value) +
                            "%");
      break;

    case Artifact::StatType::kDEFPercent:
      description.push_back("§fDEF: " + DoubleToString(this->main_stat_.value) +
                            "%");
      break;

    case Artifact::StatType::kElementalMastery:

      description.push_back(
          "§fElemental Mastery: " +
          std::to_string(static_cast<int>(this->main_stat_.value)));
      break;

    case Artifact::StatType::kCritRate:
      description.push_back(
          "§fCRIT Rate: " + DoubleToString(this->main_stat_.value) + "%");
      break;

    case Artifact::StatType::kCritDMG:
      description.push_back(
          "§fCRIT DMG: " + DoubleToString(this->main_stat_.value) + "%");
      break;

    case Artifact::StatType::kHealingBonus:
      description.push_back(
          "§fHealing Bonus: " + DoubleToString(this->main_stat_.value) + "%");
      break;

    case Artifact::StatType::kEnergyRecharge:
      description.push_back(
          "§fEnergy Recharge: " + DoubleToString(this->main_stat_.value) + "%");
      break;

    case Artifact::StatType::kPyroDMG:
      description.push_back(
          "§fPyro DMG Bonus: " + DoubleToString(this->main_stat_.value) + "%");
      break;

    case Artifact::StatType::kHydroDMG:
      description.push_back(
          "§fHydro DMG Bonus: " + DoubleToString(this->main_stat_.value) + "%");
      break;

    case Artifact::StatType::kDendroDMG:
      description.push_back("§fDendro DMG Bonus: " +
                            DoubleToString(this->main_stat_.value) + "%");
      break;

    case Artifact::StatType::kElectroDMG:
      description.push_back("§fElectro DMG Bonus: " +
                            DoubleToString(this->main_stat_.value) + "%");
      break;

    case Artifact::StatType::kAnemoDMG:
      description.push_back(
          "§fAnemo DMG Bonus: " + DoubleToString(this->main_stat_.value) + "%");
      break;

    case Artifact::StatType::kCryoDMG:
      description.push_back(
          "§fCryo DMG Bonus: " + DoubleToString(this->main_stat_.value) + "%");
      break;

    case Artifact::StatType::kGeoDMG:
      description.push_back(
          "§fGeo DMG Bonus: " + DoubleToString(this->main_stat_.value) + "%");
      break;

    case Artifact::StatType::kPhysicalDMG:
      description.push_back("§fPhysical DMG Bonus: " +
                            DoubleToString(this->main_stat_.value) + "%");
      break;

    default:
      break;
  }

  for (const auto& stat : this->sub_stat_list_) {
    if (stat.value < 0.0001) {
      continue;
    }

    switch (stat.type) {
      case Artifact::StatType::kHP:
        description.push_back("§f·HP+" +
                              std::to_string(static_cast<int>(stat.value)));
        break;

      case Artifact::StatType::kHPPercent:
        description.push_back("§f·HP+" + DoubleToString(stat.value) + "%");
        break;

      case Artifact::StatType::kATK:

        description.push_back("§f·ATK+" +
                              std::to_string(static_cast<int>(stat.value)));
        break;

      case Artifact::StatType::kATKPercent:
        description.push_back("§f·ATK+" + DoubleToString(stat.value) + "%");
        break;

      case Artifact::StatType::kDEF:

        description.push_back("§f·DEF+" +
                              std::to_string(static_cast<int>(stat.value)));
        break;

      case Artifact::StatType::kDEFPercent:
        description.push_back("§f·DEF+" + DoubleToString(stat.value) + "%");
        break;

      case Artifact::StatType::kElementalMastery:

        description.push_back("§f·Elemental Mastery+" +
                              std::to_string(static_cast<int>(stat.value)));
        break;

      case Artifact::StatType::kCritRate:
        description.push_back("§f·CRIT Rate+" + DoubleToString(stat.value) +
                              "%");
        break;

      case Artifact::StatType::kCritDMG:
        description.push_back("§f·CRIT DMG+" + DoubleToString(stat.value) +
                              "%");
        break;

      case Artifact::StatType::kEnergyRecharge:
        description.push_back("§f·Energy Recharge+" +
                              DoubleToString(stat.value) + "%");
        break;

      default:
        break;
    }
  }

  return description;
}

int Artifact::GetLevel() const {
  return this->GetLevelByArtifactEXP(this->artifact_exp_);
}

int Artifact::GetLevelMax() const {
  return Artifact::kRarityMaxLevelList[this->rarity_];
}

int Artifact::GetLevelByArtifactEXP(int artifact_exp) const {
  auto rarity = this->GetRarity();

  // Get the level by the artifact EXP
  int level = 0;
  for (int i = 0; i <= this->GetLevelMax(); ++i) {
    if (Artifact::kLevelMinArtifactEXPList[rarity][i] <= artifact_exp) {
      level = i;
    } else {
      break;
    }
  }

  return level;
}

std::string Artifact::GetName() const {
  return Artifact::kArtifactInfoDict.at(this->identifier_).name;
}

int Artifact::GetRarity() const { return this->rarity_; }

std::string Artifact::GetSetName() const {
  return Artifact::kArtifactInfoDict.at(this->identifier_).set_name;
}

Artifact::Type Artifact::GetType() const {
  return Artifact::kArtifactInfoDict.at(this->identifier_).type;
}

void Artifact::IncreaseArtifactEXP(int value) {
  static std::default_random_engine random_engine;
  if (this->GetLevel() >= this->GetLevelMax()) {
    return;
  }

  auto previous_level = this->GetLevel();

  this->artifact_exp_ += std::max(value, 0);

  if (this->GetLevel() > previous_level) {
    this->main_stat_.value =
        Artifact::kMainStatBase[this->rarity_].at(this->main_stat_.type) +
        this->GetLevel() *
            Artifact::kMainStatDiff[this->rarity_].at(this->main_stat_.type);
  }

  if (this->GetLevel() % 4 == 0 && previous_level % 4 != 0) {
    // Enhance the zero-value stats first
    for (int i = 0; i < 4; ++i) {
      if (this->sub_stat_list_.at(i).value < 0.0001) {
        auto possible_diff_list =
            Artifact::kPossibleSubStatDiff[this->rarity_].at(
                this->sub_stat_list_.at(i).type);

        this->sub_stat_list_.at(i).value +=
            possible_diff_list.at(std::uniform_int_distribution(
                0, static_cast<int>(possible_diff_list.size() - 1))(
                random_engine));
        return;
      }
    }

    // Enhance a random stat
    auto stat_no = std::uniform_int_distribution(0, 3)(random_engine);
    auto possible_diff_list = Artifact::kPossibleSubStatDiff[this->rarity_].at(
        this->sub_stat_list_.at(stat_no).type);
    this->sub_stat_list_.at(stat_no).value +=
        possible_diff_list.at(std::uniform_int_distribution(
            0, static_cast<int>(possible_diff_list.size() - 1))(random_engine));
  }
}

bool Artifact::CheckIsArtifact(ItemStack* item) {
  auto identifier = item->getTypeName();

  identifier = identifier.substr(0, identifier.size() - 2);

  for (const auto& artifact_info : Artifact::kArtifactInfoDict) {
    if (identifier == artifact_info.first) {
      return true;
    }
  }

  return false;
}

int Artifact::GetSetCount(const std::string& set_name, PlayerEx* playerex) {
  int set_count = 0;
  for (auto artifact_pair : playerex->GetArtifactDict()) {
    auto artifact = artifact_pair.second;
    if (artifact->GetSetName() == set_name) {
      ++set_count;
    }
  }

  return set_count;
}

std::vector<std::string> Artifact::GetSetEffectDescription(
    const std::string& set_name) {
  return Artifact::kSetEffectDescriptionDict.at(set_name);
}

std::shared_ptr<Artifact> Artifact::Make(ItemStack* item, PlayerEx* playerex) {
  if (!Artifact::CheckIsArtifact(item)) {
    throw ExceptionNotAnArtifact();
  }

  return std::make_shared<Artifact>(item, playerex);
}

void Artifact::InitStats() {
  static std::default_random_engine random_engine;

  // Initialize the main stat
  if (this->GetType() == Artifact::Type::kFlowerOfLife) {
    this->main_stat_.type = Artifact::StatType::kHP;
    this->main_stat_.value =
        Artifact::kMainStatBase[this->rarity_].at(this->main_stat_.type);

  } else if (this->GetType() == Artifact::Type::kPlumeOfDeath) {
    this->main_stat_.type = Artifact::StatType::kATK;
    this->main_stat_.value =
        Artifact::kMainStatBase[this->rarity_].at(this->main_stat_.type);

  } else if (this->GetType() == Artifact::Type::kSandsOfEon) {
    std::vector<Artifact::StatType> possible_main_stat_list = {
        Artifact::StatType::kHPPercent, Artifact::StatType::kATKPercent,
        Artifact::StatType::kDEFPercent, Artifact::StatType::kElementalMastery,
        Artifact::StatType::kEnergyRecharge};

    this->main_stat_.type = possible_main_stat_list.at(
        std::uniform_int_distribution(0, 4)(random_engine));
    this->main_stat_.value =
        Artifact::kMainStatBase[this->rarity_].at(this->main_stat_.type);
  } else if (this->GetType() == Artifact::Type::kGobletOfEonothem) {
    std::vector<Artifact::StatType> possible_main_stat_list = {
        Artifact::StatType::kHPPercent,  Artifact::StatType::kATKPercent,
        Artifact::StatType::kDEFPercent, Artifact::StatType::kElementalMastery,
        Artifact::StatType::kPyroDMG,    Artifact::StatType::kHydroDMG,
        Artifact::StatType::kDendroDMG,  Artifact::StatType::kElectroDMG,
        Artifact::StatType::kAnemoDMG,   Artifact::StatType::kCryoDMG,
        Artifact::StatType::kGeoDMG,     Artifact::StatType::kPhysicalDMG};

    this->main_stat_.type = possible_main_stat_list.at(
        std::uniform_int_distribution(0, 11)(random_engine));
    this->main_stat_.value =
        Artifact::kMainStatBase[this->rarity_].at(this->main_stat_.type);
  } else if (this->GetType() == Artifact::Type::kCircletOfLogos) {
    std::vector<Artifact::StatType> possible_main_stat_list = {
        Artifact::StatType::kHPPercent,  Artifact::StatType::kATKPercent,
        Artifact::StatType::kDEFPercent, Artifact::StatType::kElementalMastery,
        Artifact::StatType::kCritRate,   Artifact::StatType::kCritDMG};

    this->main_stat_.type = possible_main_stat_list.at(
        std::uniform_int_distribution(0, 5)(random_engine));
    this->main_stat_.value =
        Artifact::kMainStatBase[this->rarity_].at(this->main_stat_.type);
  }

  // Initialize the sub stats
  std::vector<Artifact::StatType> possible_sub_stat_list = {
      Artifact::StatType::kHP,
      Artifact::StatType::kATK,
      Artifact::StatType::kDEF,
      Artifact::StatType::kHPPercent,
      Artifact::StatType::kATKPercent,
      Artifact::StatType::kDEFPercent,
      Artifact::StatType::kElementalMastery,
      Artifact::StatType::kEnergyRecharge,
      Artifact::StatType::kCritRate,
      Artifact::StatType::kCritDMG};

  // Prevent duplicated stats
  for (auto it = possible_sub_stat_list.begin();
       it != possible_sub_stat_list.end();) {
    if (*it == this->main_stat_.type) {
      possible_sub_stat_list.erase(it);
    } else {
      ++it;
    }
  }

  std::shuffle(possible_sub_stat_list.begin(), possible_sub_stat_list.end(),
               random_engine);

  auto sub_stat_count =
      this->GetRarity() - std::uniform_int_distribution(1, 2)(random_engine);
  sub_stat_count = std::max(sub_stat_count, 0);

  for (int i = 0; i < 4; ++i) {
    Artifact::StatItem stat_item;
    stat_item.type = possible_sub_stat_list.at(i);
    stat_item.value = 0.;
    if (i < sub_stat_count) {
      auto sub_stat_diff_list =
          Artifact::kPossibleSubStatDiff[this->rarity_].at(stat_item.type);
      stat_item.value = sub_stat_diff_list.at(std::uniform_int_distribution(
          0, static_cast<int>(sub_stat_diff_list.size() - 1))(random_engine));
    }
    this->sub_stat_list_.push_back(stat_item);
  }
}

const int Artifact::kRarityBaseConsumableEXPList[6] = {0,    420,  840,
                                                       1260, 2520, 3780};

const std::map<std::string, Artifact::ArtifactInfo>
    Artifact::kArtifactInfoDict = {
        {"genshicraft:adventurer_s_flower",
         {"Adventurer's Flower", "Adventurer", Artifact::Type::kFlowerOfLife}},
        {"genshicraft:adventurer_s_tail_feather",
         {"Adventurer's Tail Feather", "Adventurer",
          Artifact::Type::kPlumeOfDeath}},
        {"genshicraft:adventurer_s_pocket_watch",
         {"Adventurer's Pocket Watch", "Adventurer",
          Artifact::Type::kSandsOfEon}},
        {"genshicraft:adventurer_s_golden_goblet",
         {"Adventurer's Golden Goblet", "Adventurer",
          Artifact::Type::kGobletOfEonothem}},
        {"genshicraft:adventurer_s_bandana",
         {"Adventurer's Bandana", "Adventurer",
          Artifact::Type::kCircletOfLogos}},
};

const int Artifact::kLevelMinArtifactEXPList[6][21] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    {0,      600,    1350,   2225,   3250,   999999, 999999,
     999999, 999999, 999999, 999999, 999999, 999999, 999999,
     999999, 999999, 999999, 999999, 999999, 999999, 999999},

    {0,      1200,   2700,   4475,   6525,   999999, 999999,
     999999, 999999, 999999, 999999, 999999, 999999, 999999,
     999999, 999999, 999999, 999999, 999999, 999999, 999999},

    {0,      1800,   4025,   6675,   9775,   13325,  17325,
     21825,  26825,  32350,  38425,  45050,  52275,  999999,
     999999, 999999, 999999, 999999, 999999, 999999, 999999},

    {0,     2400,   5375,   8925,   13050,  17775,  23125,
     29125, 35800,  43175,  51275,  60125,  69750,  80175,
     92300, 106375, 122675, 999999, 999999, 999999, 999999},

    {0,      3000,   6725,   11150,  16300,  22200,  28875,
     36375,  44725,  53950,  64075,  75125,  87150,  100175,
     115325, 132925, 153300, 176800, 203850, 234900, 270475},
};

const std::map<Artifact::StatType, double> Artifact::kMainStatBase[6] = {
    {},

    {{Artifact::StatType::kHP, 129},
     {Artifact::StatType::kATK, 8},
     {Artifact::StatType::kHPPercent, 3.1},
     {Artifact::StatType::kATKPercent, 3.1},
     {Artifact::StatType::kDEFPercent, 3.9},
     {Artifact::StatType::kPhysicalDMG, 3.9},
     {Artifact::StatType::kPyroDMG, 3.1},
     {Artifact::StatType::kHydroDMG, 3.1},
     {Artifact::StatType::kDendroDMG, 3.1},
     {Artifact::StatType::kElectroDMG, 3.1},
     {Artifact::StatType::kAnemoDMG, 3.1},
     {Artifact::StatType::kCryoDMG, 3.1},
     {Artifact::StatType::kGeoDMG, 3.1},
     {Artifact::StatType::kElementalMastery, 12.6},
     {Artifact::StatType::kEnergyRecharge, 3.5},
     {Artifact::StatType::kCritRate, 2.1},
     {Artifact::StatType::kCritDMG, 4.2},
     {Artifact::StatType::kHealingBonus, 2.4}},

    {{Artifact::StatType::kHP, 258},
     {Artifact::StatType::kATK, 17},
     {Artifact::StatType::kHPPercent, 4.2},
     {Artifact::StatType::kATKPercent, 4.2},
     {Artifact::StatType::kDEFPercent, 5.2},
     {Artifact::StatType::kPhysicalDMG, 5.2},
     {Artifact::StatType::kPyroDMG, 4.2},
     {Artifact::StatType::kHydroDMG, 4.2},
     {Artifact::StatType::kDendroDMG, 4.2},
     {Artifact::StatType::kElectroDMG, 4.2},
     {Artifact::StatType::kAnemoDMG, 4.2},
     {Artifact::StatType::kCryoDMG, 4.2},
     {Artifact::StatType::kGeoDMG, 4.2},
     {Artifact::StatType::kElementalMastery, 16.8},
     {Artifact::StatType::kEnergyRecharge, 4.7},
     {Artifact::StatType::kCritRate, 2.8},
     {Artifact::StatType::kCritDMG, 5.6},
     {Artifact::StatType::kHealingBonus, 3.2}},

    {{Artifact::StatType::kHP, 430},
     {Artifact::StatType::kATK, 28},
     {Artifact::StatType::kHPPercent, 5.2},
     {Artifact::StatType::kATKPercent, 5.2},
     {Artifact::StatType::kDEFPercent, 6.6},
     {Artifact::StatType::kPhysicalDMG, 6.6},
     {Artifact::StatType::kPyroDMG, 5.2},
     {Artifact::StatType::kHydroDMG, 5.2},
     {Artifact::StatType::kDendroDMG, 5.2},
     {Artifact::StatType::kElectroDMG, 5.2},
     {Artifact::StatType::kAnemoDMG, 5.2},
     {Artifact::StatType::kCryoDMG, 5.2},
     {Artifact::StatType::kGeoDMG, 5.2},
     {Artifact::StatType::kElementalMastery, 21.},
     {Artifact::StatType::kEnergyRecharge, 5.8},
     {Artifact::StatType::kCritRate, 3.5},
     {Artifact::StatType::kCritDMG, 7.},
     {Artifact::StatType::kHealingBonus, 4.}},

    {{Artifact::StatType::kHP, 645},
     {Artifact::StatType::kATK, 42},
     {Artifact::StatType::kHPPercent, 6.3},
     {Artifact::StatType::kATKPercent, 6.3},
     {Artifact::StatType::kDEFPercent, 7.9},
     {Artifact::StatType::kPhysicalDMG, 7.9},
     {Artifact::StatType::kPyroDMG, 6.3},
     {Artifact::StatType::kHydroDMG, 6.3},
     {Artifact::StatType::kDendroDMG, 6.3},
     {Artifact::StatType::kElectroDMG, 6.3},
     {Artifact::StatType::kAnemoDMG, 6.3},
     {Artifact::StatType::kCryoDMG, 6.3},
     {Artifact::StatType::kGeoDMG, 6.3},
     {Artifact::StatType::kElementalMastery, 25.2},
     {Artifact::StatType::kEnergyRecharge, 7.},
     {Artifact::StatType::kCritRate, 4.2},
     {Artifact::StatType::kCritDMG, 8.4},
     {Artifact::StatType::kHealingBonus, 4.8}},

    {{Artifact::StatType::kHP, 717},
     {Artifact::StatType::kATK, 47},
     {Artifact::StatType::kHPPercent, 7.0},
     {Artifact::StatType::kATKPercent, 7.0},
     {Artifact::StatType::kDEFPercent, 8.7},
     {Artifact::StatType::kPhysicalDMG, 8.7},
     {Artifact::StatType::kPyroDMG, 7.0},
     {Artifact::StatType::kHydroDMG, 7.0},
     {Artifact::StatType::kDendroDMG, 7.0},
     {Artifact::StatType::kElectroDMG, 7.0},
     {Artifact::StatType::kAnemoDMG, 7.0},
     {Artifact::StatType::kCryoDMG, 7.0},
     {Artifact::StatType::kGeoDMG, 7.0},
     {Artifact::StatType::kElementalMastery, 28.},
     {Artifact::StatType::kEnergyRecharge, 7.8},
     {Artifact::StatType::kCritRate, 4.7},
     {Artifact::StatType::kCritDMG, 9.3},
     {Artifact::StatType::kHealingBonus, 5.4}}};

const std::map<Artifact::StatType, double> Artifact::kMainStatDiff[6] = {
    {},

    {{Artifact::StatType::kHP, 48.75},
     {Artifact::StatType::kATK, 3.25},
     {Artifact::StatType::kHPPercent, 1.2},
     {Artifact::StatType::kATKPercent, 1.2},
     {Artifact::StatType::kDEFPercent, 1.5},
     {Artifact::StatType::kPhysicalDMG, 1.5},
     {Artifact::StatType::kPyroDMG, 1.2},
     {Artifact::StatType::kHydroDMG, 1.2},
     {Artifact::StatType::kDendroDMG, 1.2},
     {Artifact::StatType::kElectroDMG, 1.2},
     {Artifact::StatType::kAnemoDMG, 1.2},
     {Artifact::StatType::kCryoDMG, 1.2},
     {Artifact::StatType::kGeoDMG, 1.2},
     {Artifact::StatType::kElementalMastery, 4.75},
     {Artifact::StatType::kEnergyRecharge, 1.325},
     {Artifact::StatType::kCritRate, 0.8},
     {Artifact::StatType::kCritDMG, 1.575},
     {Artifact::StatType::kHealingBonus, 0.925}},

    {{Artifact::StatType::kHP, 73.25},
     {Artifact::StatType::kATK, 4.75},
     {Artifact::StatType::kHPPercent, 1.2},
     {Artifact::StatType::kATKPercent, 1.2},
     {Artifact::StatType::kDEFPercent, 1.5},
     {Artifact::StatType::kPhysicalDMG, 1.5},
     {Artifact::StatType::kPyroDMG, 1.2},
     {Artifact::StatType::kHydroDMG, 1.2},
     {Artifact::StatType::kDendroDMG, 1.2},
     {Artifact::StatType::kElectroDMG, 1.2},
     {Artifact::StatType::kAnemoDMG, 1.2},
     {Artifact::StatType::kCryoDMG, 1.2},
     {Artifact::StatType::kGeoDMG, 1.2},
     {Artifact::StatType::kElementalMastery, 4.75},
     {Artifact::StatType::kEnergyRecharge, 1.3},
     {Artifact::StatType::kCritRate, 0.8},
     {Artifact::StatType::kCritDMG, 1.575},
     {Artifact::StatType::kHealingBonus, 0.925}},

    {{Artifact::StatType::kHP, 121.917},
     {Artifact::StatType::kATK, 7.917},
     {Artifact::StatType::kHPPercent, 1.492},
     {Artifact::StatType::kATKPercent, 1.492},
     {Artifact::StatType::kDEFPercent, 1.850},
     {Artifact::StatType::kPhysicalDMG, 1.850},
     {Artifact::StatType::kPyroDMG, 1.492},
     {Artifact::StatType::kHydroDMG, 1.492},
     {Artifact::StatType::kDendroDMG, 1.492},
     {Artifact::StatType::kElectroDMG, 1.492},
     {Artifact::StatType::kAnemoDMG, 1.492},
     {Artifact::StatType::kCryoDMG, 1.492},
     {Artifact::StatType::kGeoDMG, 1.492},
     {Artifact::StatType::kElementalMastery, 5.942},
     {Artifact::StatType::kEnergyRecharge, 1.65},
     {Artifact::StatType::kCritRate, 0.992},
     {Artifact::StatType::kCritDMG, 1.983},
     {Artifact::StatType::kHealingBonus, 1.15}},

    {{Artifact::StatType::kHP, 182.875},
     {Artifact::StatType::kATK, 11.875},
     {Artifact::StatType::kHPPercent, 1.781},
     {Artifact::StatType::kATKPercent, 1.781},
     {Artifact::StatType::kDEFPercent, 2.225},
     {Artifact::StatType::kPhysicalDMG, 2.225},
     {Artifact::StatType::kPyroDMG, 1.781},
     {Artifact::StatType::kHydroDMG, 1.781},
     {Artifact::StatType::kDendroDMG, 1.781},
     {Artifact::StatType::kElectroDMG, 1.781},
     {Artifact::StatType::kAnemoDMG, 1.781},
     {Artifact::StatType::kCryoDMG, 1.781},
     {Artifact::StatType::kGeoDMG, 1.781},
     {Artifact::StatType::kElementalMastery, 7.131},
     {Artifact::StatType::kEnergyRecharge, 1.981},
     {Artifact::StatType::kCritRate, 1.188},
     {Artifact::StatType::kCritDMG, 2.375},
     {Artifact::StatType::kHealingBonus, 1.375}},

    {{Artifact::StatType::kHP, 203.15},
     {Artifact::StatType::kATK, 13.2},
     {Artifact::StatType::kHPPercent, 1.98},
     {Artifact::StatType::kATKPercent, 1.98},
     {Artifact::StatType::kDEFPercent, 2.48},
     {Artifact::StatType::kPhysicalDMG, 2.48},
     {Artifact::StatType::kPyroDMG, 1.98},
     {Artifact::StatType::kHydroDMG, 1.98},
     {Artifact::StatType::kDendroDMG, 1.98},
     {Artifact::StatType::kElectroDMG, 1.98},
     {Artifact::StatType::kAnemoDMG, 1.98},
     {Artifact::StatType::kCryoDMG, 1.98},
     {Artifact::StatType::kGeoDMG, 1.98},
     {Artifact::StatType::kElementalMastery, 7.925},
     {Artifact::StatType::kEnergyRecharge, 2.2},
     {Artifact::StatType::kCritRate, 1.32},
     {Artifact::StatType::kCritDMG, 2.645},
     {Artifact::StatType::kHealingBonus, 1.525}}};

const std::map<Artifact::StatType, std::vector<double>>
    Artifact::kPossibleSubStatDiff[6] = {
        {},

        {
            {Artifact::StatType::kHP, {23.90, 29.88}},
            {Artifact::StatType::kATK, {1.56, 1.95}},
            {Artifact::StatType::kDEF, {1.85, 2.31}},
            {Artifact::StatType::kHPPercent, {1.17, 1.46}},
            {Artifact::StatType::kATKPercent, {1.17, 1.46}},
            {Artifact::StatType::kDEFPercent, {1.46, 1.82}},
            {Artifact::StatType::kElementalMastery, {4.66, 5.83}},
            {Artifact::StatType::kEnergyRecharge, {1.30, 1.62}},
            {Artifact::StatType::kCritRate, {0.78, 0.97}},
            {Artifact::StatType::kCritDMG, {1.55, 1.94}},
        },

        {
            {Artifact::StatType::kHP, {50.19, 60.95, 71.70}},
            {Artifact::StatType::kATK, {3.27, 3.97, 4.67}},
            {Artifact::StatType::kDEF, {3.89, 4.72, 5.56}},
            {Artifact::StatType::kHPPercent, {1.63, 1.98, 2.33}},
            {Artifact::StatType::kATKPercent, {1.63, 1.98, 2.33}},
            {Artifact::StatType::kDEFPercent, {2.04, 2.48, 2.91}},
            {Artifact::StatType::kElementalMastery, {6.53, 7.93, 9.33}},
            {Artifact::StatType::kEnergyRecharge, {1.81, 2.20, 2.59}},
            {Artifact::StatType::kCritRate, {1.09, 1.32, 1.55}},
            {Artifact::StatType::kCritDMG, {2.18, 2.64, 3.11}},
        },

        {
            {Artifact::StatType::kHP, {100.38, 114.72, 129.06, 143.40}},
            {Artifact::StatType::kATK, {6.54, 7.47, 8.40, 9.34}},
            {Artifact::StatType::kDEF, {7.78, 8.89, 10.00, 11.11}},
            {Artifact::StatType::kHPPercent, {2.45, 2.80, 3.15, 3.50}},
            {Artifact::StatType::kATKPercent, {2.45, 2.80, 3.15, 3.50}},
            {Artifact::StatType::kDEFPercent, {3.06, 3.50, 3.93, 4.37}},
            {Artifact::StatType::kElementalMastery,
             {9.79, 11.19, 12.59, 13.99}},
            {Artifact::StatType::kEnergyRecharge, {2.72, 3.11, 3.50, 3.89}},
            {Artifact::StatType::kCritRate, {1.63, 1.86, 2.10, 2.33}},
            {Artifact::StatType::kCritDMG, {3.26, 3.73, 4.20, 4.66}},
        },

        {
            {Artifact::StatType::kHP, {167.30, 191.20, 215.10, 239.00}},
            {Artifact::StatType::kATK, {10.89, 12.45, 14.00, 15.56}},
            {Artifact::StatType::kDEF, {12.96, 14.82, 16.67, 18.52}},
            {Artifact::StatType::kHPPercent, {3.26, 3.73, 4.20, 4.66}},
            {Artifact::StatType::kATKPercent, {3.26, 3.73, 4.20, 4.66}},
            {Artifact::StatType::kDEFPercent, {4.08, 4.66, 5.25, 5.83}},
            {Artifact::StatType::kElementalMastery,
             {13.06, 14.92, 16.79, 18.56}},
            {Artifact::StatType::kEnergyRecharge, {3.63, 4.14, 4.66, 5.18}},
            {Artifact::StatType::kCritRate, {2.18, 2.49, 2.80, 3.11}},
            {Artifact::StatType::kCritDMG, {4.35, 4.97, 5.60, 6.22}},
        },

        {
            {Artifact::StatType::kHP, {209.13, 239.00, 268.88, 298.75}},
            {Artifact::StatType::kATK, {13.62, 15.56, 17.51, 19.45}},
            {Artifact::StatType::kDEF, {16.20, 18.52, 20.83, 23.15}},
            {Artifact::StatType::kHPPercent, {4.08, 4.66, 5.25, 5.83}},
            {Artifact::StatType::kATKPercent, {4.08, 4.66, 5.25, 5.83}},
            {Artifact::StatType::kDEFPercent, {5.10, 5.83, 6.56, 7.29}},
            {Artifact::StatType::kElementalMastery,
             {16.32, 18.65, 20.98, 23.31}},
            {Artifact::StatType::kEnergyRecharge, {4.53, 5.18, 5.83, 6.48}},
            {Artifact::StatType::kCritRate, {2.72, 3.11, 3.50, 3.89}},
            {Artifact::StatType::kCritDMG, {5.44, 6.22, 6.99, 7.77}},
        },
};

const int Artifact::kRarityMaxLevelList[6] = {0, 4, 4, 12, 16, 20};

const std::map<std::string, std::vector<std::string>>
    Artifact::kSetEffectDescriptionDict = {
        {"Adventurer",
         {"Max HP increased by 1,000.",
          "Opening a chest regenerates 30% Max HP over 5s."}}};

}  // namespace genshicraft
