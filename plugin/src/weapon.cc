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
 * @file weapon.cc
 * @author Futrime (futrime@outlook.com)
 * @brief Definition of the Weapon class
 * @version 1.0.0
 * @date 2022-08-04
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#include "weapon.h"

#include <EventAPI.h>
#include <ScheduleAPI.h>

#include <MC/CompoundTag.hpp>
#include <MC/ItemStack.hpp>
#include <MC/Player.hpp>
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "character.h"
#include "exceptions.h"
#include "playerex.h"
#include "plugin.h"
#include "weapons/dull_blade.h"
#include "weapons/silver_sword.h"

namespace genshicraft {

const int Weapon::kAcensionPhaseMaxLevelList[7] = {20, 40, 50, 60, 70, 80, 90};

void Weapon::ApplyLore(ItemStack* item, PlayerEx* playerex) {
  static const std::string kRefinementSymbolList[6] = {" ", "①", "②",
                                                       "③", "④", "§e⑤"};

  // Set the lore
  std::vector<std::string> lore;

  if (this->GetType() == Weapon::Type::kSword) {
    lore.push_back("§7Sword");
  } else if (this->GetType() == Weapon::Type::kClaymore) {
    lore.push_back("§7Claymore");
  } else if (this->GetType() == Weapon::Type::kPolearm) {
    lore.push_back("§7Polearm");
  } else if (this->GetType() == Weapon::Type::kCatalyst) {
    lore.push_back("§7Catalyst");
  } else if (this->GetType() == Weapon::Type::kBow) {
    lore.push_back("§7Bow");
  }

  for (const auto& line : this->GetBaseStatsDescription()) {
    lore.push_back("§f" + line);
  }

  std::string rarity_line = "§6";
  for (int i = 0; i < this->GetRarity(); ++i) {
    rarity_line += "★";
  }
  lore.push_back(rarity_line);

  std::string level_line;
  level_line +=
      "§fLv." + std::to_string(this->GetLevel()) + " §7/ " +
      std::to_string(
          Weapon::kAcensionPhaseMaxLevelList[this->GetAscensionPhase()]);
  level_line += " §f";
  for (int i = 0; i < ((this->GetRarity() <= 2) ? 4 : 6); ++i) {
    if (i == this->GetAscensionPhase()) {
      level_line += "§7";
    }
    level_line += "✦";
  }
  lore.push_back(level_line);

  if (this->GetRarity() >= 3) {
    lore.push_back("§eRefinement Rank " +
                   std::to_string(this->GetRefinement()));
  }

  // Prevent duplicated refresh
  auto old_lore = item->getCustomLore();
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
    item->setCustomLore(lore);
    playerex->RefreshItems();
  }
}

int Weapon::GetAscensionPhase() const { return this->ascension_phase_; }

std::vector<std::string> Weapon::GetBaseStatsDescription() const {
  std::vector<std::string> description;

  description.push_back("Base ATK: " +
                        std::to_string(this->GetBaseStats().ATK_base));

  if (this->GetBaseStats().max_HP_percent > 0.0001) {
    description.push_back(
        "Max HP: " + std::to_string(this->GetBaseStats().max_HP_percent * 100) +
        "%");
  } else if (this->GetBaseStats().ATK_percent > 0.0001) {
    description.push_back(
        "ATK: " + std::to_string(this->GetBaseStats().ATK_percent * 100) + "%");
  } else if (this->GetBaseStats().DEF_percent > 0.0001) {
    description.push_back(
        "DEF: " + std::to_string(this->GetBaseStats().DEF_percent * 100) + "%");
  } else if (this->GetBaseStats().elemental_mastery != 0) {
    description.push_back(
        "Elemental Mastery: " +
        std::to_string(this->GetBaseStats().elemental_mastery));
  } else if (std::abs(this->GetBaseStats().CRIT_rate) > 0.000001) {
    description.push_back(
        "CRIT Rate: " + std::to_string(this->GetBaseStats().CRIT_rate * 100) +
        "%");
  } else if (std::abs(this->GetBaseStats().CRIT_DMG) > 0.000001) {
    description.push_back(
        "CRIT DMG: " + std::to_string(this->GetBaseStats().CRIT_DMG * 100) +
        "%");
  } else if (std::abs(this->GetBaseStats().energy_recharge) > 0.000001) {
    description.push_back(
        "Energy Recharge: " +
        std::to_string(this->GetBaseStats().energy_recharge * 100) + "%");
  } else if (std::abs(this->GetBaseStats().physical_DMG_bonus) > 0.000001) {
    description.push_back(
        "Physical DMG Bonus: " +
        std::to_string(this->GetBaseStats().physical_DMG_bonus * 100) + "%");
  }

  return description;
}

int Weapon::GetLevel() const {
  return this->GetLevelByWeaponEXP(this->weapon_exp_);
}

int Weapon::GetLevelByWeaponEXP(int weapon_exp) const {
  auto rarity = this->GetRarity();

  // Get the level by the weapon EXP
  int level = 1;
  if (rarity == 1) {
    for (int i = 1; i <= 70; ++i) {
      if (Weapon::k1StarLevelMinWeaponEXPList[i] <= weapon_exp) {
        level = i;
      } else {
        break;
      }
    }
  }
  if (rarity == 2) {
    for (int i = 1; i <= 70; ++i) {
      if (Weapon::k2StarLevelMinWeaponEXPList[i] <= weapon_exp) {
        level = i;
      } else {
        break;
      }
    }
  }
  if (rarity == 3) {
    for (int i = 1; i <= 90; ++i) {
      if (Weapon::k3StarLevelMinWeaponEXPList[i] <= weapon_exp) {
        level = i;
      } else {
        break;
      }
    }
  }
  if (rarity == 4) {
    for (int i = 1; i <= 90; ++i) {
      if (Weapon::k4StarLevelMinWeaponEXPList[i] <= weapon_exp) {
        level = i;
      } else {
        break;
      }
    }
  }
  if (rarity == 5) {
    for (int i = 1; i <= 90; ++i) {
      if (Weapon::k5StarLevelMinWeaponEXPList[i] <= weapon_exp) {
        level = i;
      } else {
        break;
      }
    }
  }

  // Limit the level by the Ascension Phase
  level = std::min(level,
                   Weapon::kAcensionPhaseMaxLevelList[this->ascension_phase_]);

  return level;
}

int Weapon::GetRefinement() const { return this->refinement_; }

int Weapon::GetWeaponEXP() const { return this->weapon_exp_; }

void Weapon::IncreaseAscensionPhase() {
  if (this->GetLevel() ==
      Weapon::kAcensionPhaseMaxLevelList
          [this->ascension_phase_]) {  // if it is time to ascense

    if (this->GetRarity() <= 2) {
      this->ascension_phase_ = std::min(this->ascension_phase_ + 1, 4);
    } else {
      this->ascension_phase_ = std::min(this->ascension_phase_ + 1, 6);
    }
  }
}

void Weapon::IncreaseWeaponEXP(int value) {
  this->weapon_exp_ += std::max(value, 0);
}

bool Weapon::CheckIsWeapon(ItemStack* item) {
  auto current_identifier = item->getTypeName();

  for (const auto& identifier : Weapon::kIdentifierList) {
    if (current_identifier == identifier) {
      return true;
    }
  }

  return false;
}

std::shared_ptr<Weapon> Weapon::Make(ItemStack* item, PlayerEx* playerex) {
  if (item->getTypeName() == "genshicraft:dull_blade") {
    return std::make_shared<DullBlade>(item, playerex);
  }

  if (item->getTypeName() == "genshicraft:silver_sword") {
    return std::make_shared<SilverSword>(item, playerex);
  }

  throw ExceptionNotAWeapon();
}

Weapon::Weapon(ItemStack* item, PlayerEx* playerex)
    : item_(item), playerex_(playerex) {
  // Check if the item is a GenshiCraft item
  if (!Weapon::CheckIsWeapon(item)) {
    throw ExceptionNotAWeapon();
  }

  auto nbt = item->getNbt();

  // Initialize the weapon if it is not registered
  if (!nbt->contains("tag") ||
      !nbt->getCompound("tag")->contains("genshicraft")) {
    // Set member variables (for the lore)
    this->weapon_exp_ = 0;
    this->ascension_phase_ = 0;
    this->refinement_ = 1;

    nbt->put("tag", CompoundTag());

    auto tag = nbt->getCompound("tag");
    tag->put("genshicraft", CompoundTag());

    auto data = tag->getCompound("genshicraft");
    data->putInt("ascension_phase",
                 this->ascension_phase_);           // the Ascension Phase
    data->putInt("refinement", this->refinement_);  // the Refinement
    data->putInt("weapon_exp", this->weapon_exp_);  // the Weapon EXP

    item->setNbt(nbt.get());
    playerex->RefreshItems();
  }

  auto data = nbt->getCompound("tag")->getCompound("genshicraft");

  this->ascension_phase_ = data->getInt("ascension_phase");
  this->refinement_ = data->getInt("refinement");
  this->weapon_exp_ = data->getInt("weapon_exp");
}

Weapon::~Weapon() {
  auto nbt = this->item_->getNbt();
  auto data = nbt->getCompound("tag")->getCompound("genshicraft");

  bool is_modified = false;
  if (this->ascension_phase_ != data->getInt("ascension_phase") ||
      this->refinement_ != data->getInt("refinement") ||
      this->weapon_exp_ != data->getInt("weapon_exp")) {
    is_modified = true;
  }

  data->putInt("ascension_phase",
               this->ascension_phase_);           // the Ascension Phase
  data->putInt("refinement", this->refinement_);  // the Refinement
  data->putInt("weapon_exp", this->weapon_exp_);  // the Weapon EXP

  if (is_modified) {
    this->item_->setNbt(nbt.get());
    this->playerex_->RefreshItems();
  }
}

const std::vector<std::string> Weapon::kIdentifierList = {
    "genshicraft:dull_blade", "genshicraft:silver_sword"};

const int Weapon::k1StarLevelMinWeaponEXPList[71] = {
    0,      0,      125,    325,    600,    950,    1425,   2000,   2700,
    3550,   4550,   5700,   7000,   8475,   10125,  11975,  14025,  16275,
    18725,  21400,  24325,  27475,  31050,  34875,  38975,  43375,  48075,
    53075,  58375,  63975,  69900,  76175,  82775,  89725,  97050,  104725,
    112775, 121200, 130025, 139250, 148875, 158900, 169875, 181300, 193175,
    205525, 218350, 231650, 245425, 259700, 274500, 289800, 306425, 323600,
    341325, 359625, 378500, 397975, 418050, 438725, 460025, 481950, 505625,
    529975, 555000, 580700, 607100, 634225, 662050, 690600, 719875};

const int Weapon::k2StarLevelMinWeaponEXPList[71] = {
    0,      0,      175,    450,    850,    1400,   2100,    2975,   4025,
    5275,   6750,   8450,   10400,  12625,  15100,  17875,   20925,  24300,
    28000,  32025,  36400,  41125,  46475,  52225,  58400,   65000,  72025,
    79500,  87450,  95875,  104775, 114175, 124075, 134525,  145500, 157025,
    169100, 181750, 194975, 208800, 223225, 238275, 254725,  271850, 289675,
    308200, 327425, 347375, 368050, 389475, 411650, 434600,  459525, 485275,
    511875, 539325, 567650, 596875, 626975, 658000, 689950,  722825, 758325,
    794825, 832350, 870925, 910525, 951200, 992950, 1035775, 1079675};

const int Weapon::k3StarLevelMinWeaponEXPList[91] = {
    0,       0,       275,     700,     1300,    2100,    3125,    4400,
    5950,    7800,    9975,    12475,   15350,   18600,   22250,   26300,
    30800,   35750,   41150,   47050,   53475,   60400,   68250,   76675,
    85725,   95400,   105725,  116700,  128350,  140700,  153750,  167550,
    182075,  197375,  213475,  230375,  248075,  266625,  286025,  306300,
    327475,  349525,  373675,  398800,  424925,  452075,  480275,  509525,
    539850,  571275,  603825,  637475,  674025,  711800,  750800,  791075,
    832625,  875475,  919625,  965125,  1011975, 1060200, 1112275, 1165825,
    1220875, 1277425, 1335525, 1395175, 1456400, 1519200, 1583600, 1649625,
    1720700, 1793525, 1868100, 1944450, 2022600, 2102600, 2184450, 2268150,
    2353725, 2441225, 2544500, 2660575, 2791000, 2937500, 3102050, 3286825,
    3494225, 3727000, 3988200};

const int Weapon::k4StarLevelMinWeaponEXPList[91] = {
    0,       0,       400,     1025,    1925,    3125,    4675,    6625,
    8975,    11775,   15075,   18875,   23225,   28150,   33675,   39825,
    46625,   54125,   62325,   71275,   81000,   91500,   103400,  116175,
    129875,  144525,  160150,  176775,  194425,  213125,  232900,  253800,
    275825,  299025,  323400,  349000,  375825,  403925,  433325,  464050,
    496125,  529550,  566125,  604200,  643800,  684950,  727675,  772000,
    817950,  865550,  914850,  965850,  1021225, 1078450, 1137550, 1198575,
    1261525, 1326450, 1393350, 1462275, 1533250, 1606300, 1685200, 1766325,
    1849725, 1935425, 2023450, 2113825, 2206575, 2301725, 2399300, 2499350,
    2607025, 2717350, 2830350, 2946050, 3064475, 3185675, 3309675, 3436500,
    3566175, 3698750, 3855225, 4031100, 4228700, 4450675, 4699975, 4979925,
    5294175, 5646875, 6042650};

const int Weapon::k5StarLevelMinWeaponEXPList[91] = {
    0,       0,       600,     1550,    2900,    4700,    7025,    9950,
    13475,   17675,   22625,   28325,   34850,   42250,   50550,   59775,
    69975,   81225,   93525,   106950,  121550,  137300,  155150,  174325,
    194875,  216850,  240300,  265250,  291725,  319775,  349450,  380800,
    413850,  448650,  485225,  523625,  563875,  606025,  650125,  696225,
    744350,  794500,  849375,  906500,  965900,  1027625, 1091725, 1158225,
    1227150, 1298550, 1372500, 1449000, 1532075, 1617925, 1706575, 1798125,
    1892550, 1989950, 2090300, 2193700, 2300175, 2409750, 2528100, 2649800,
    2774900, 2903450, 3035500, 3171075, 3310200, 3452925, 3599300, 3749375,
    3910900, 4076400, 4245900, 4419450, 4597100, 4778900, 4964900, 5155150,
    5349675, 5548550, 5783275, 6047100, 6343500, 6676475, 7050425, 7470350,
    7941725, 8470775, 9064450};

}  // namespace genshicraft
