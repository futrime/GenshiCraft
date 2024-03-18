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
 * @file character.cc
 * @author Futrime (futrime@outlook.com)
 * @brief Definition of the Character class
 * @version 1.0.0
 * @date 2022-08-08
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#include "character.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "characters/kuki_shinobu.h"
#include "exceptions.h"
#include "modifier.h"
#include "playerex.h"
#include "plugin.h"
#include "stats.h"

namespace genshicraft {

const int Character::kAcensionPhaseMaxLevelList[7] = {20, 40, 50, 60,
                                                      70, 80, 90};

void Character::AddModifier(Modifier modifier) {
  this->modifier_list_.push_back(modifier);
}

int Character::GetAscensionPhase() const { return this->ascension_phase_; }

double Character::GetCDElementalBurst() const {
  auto past_time = GetNowClock() - this->last_elemental_burst_clock_;
  return std::max(this->GetCDElementalBurstMax() - past_time, 0.);
};

double Character::GetCDElementalSkill() const {
  auto past_time = GetNowClock() - this->last_elemental_skill_clock_;
  return std::max(this->GetCDElementalSkillMax() - past_time, 0.);
};

int Character::GetCharacterEXP() const { return this->character_EXP_; }

int Character::GetConstellation() const { return this->constellation_; }

int Character::GetEnergy() const { return this->energy_; }

double Character::GetFullness() const { return this->fullness_; }

int Character::GetHP() const { return this->HP_; }

int Character::GetLevel() const {
  return this->GetLevelByCharacterEXP(this->character_EXP_);
}

int Character::GetLevelByCharacterEXP(int character_exp) const {
  int level = 1;

  // Get the level by the character EXP
  for (int i = 1; i <= 90; ++i) {
    if (Character::kLevelMinCharacterEXPList[i] <= character_exp) {
      level = i;
    } else {
      break;
    }
  }

  // Limit the level by the ascension phase
  level = std::min(
      level, Character::kAcensionPhaseMaxLevelList[this->ascension_phase_]);

  return level;
}

PlayerEx* Character::GetPlayerEx() const { return this->playerex_; }

Stats Character::GetStats() const {
  auto stats = this->GetBaseStats();

  if (this->HasWeapon()) {  // if the player is holding a GenshiCraft weapon
    stats += this->playerex_->GetWeapon()->GetBaseStats();
  }

  for (auto artifact_pair: this->playerex_->GetArtifactDict()) {
    auto artifact = artifact_pair.second;
    stats += artifact->GetBaseStats();
  }

  stats.max_stamina = this->playerex_->GetStaminaMax();
  stats.CRIT_rate += 0.05;
  stats.CRIT_DMG += 0.5;
  stats.energy_recharge += 1.;

  for (const auto& modifier : this->modifier_list_) {
    if (modifier.GetType() == Modifier::Type::kStats) {
      stats += modifier.GetBaseStats();
    }
  }

  return stats;
}

std::vector<std::string> Character::GetStatsDescription(bool verbose) const {
  std::vector<std::string> description;

  if (!verbose) {
    description.push_back("Max HP: " +
                          std::to_string(this->GetStats().GetMaxHP()));
    description.push_back("ATK: " + std::to_string(this->GetStats().GetATK()));
    description.push_back("DEF: " + std::to_string(this->GetStats().GetDEF()));
    description.push_back("Elemental Mastery: " +
                          std::to_string(this->GetStats().elemental_mastery));
    description.push_back("Max Stamina: " +
                          std::to_string(this->GetStats().max_stamina));
  } else {
    // Base stats
    description.push_back(
        "Max HP: " + std::to_string(this->GetStats().max_HP_base) + " §a+" +
        std::to_string(this->GetStats().GetMaxHP() -
                       this->GetStats().max_HP_base));
    description.push_back(
        "ATK: " + std::to_string(this->GetStats().ATK_base) + " §a+" +
        std::to_string(this->GetStats().GetATK() - this->GetStats().ATK_base));
    description.push_back(
        "DEF: " + std::to_string(this->GetStats().DEF_base) + " §a+" +
        std::to_string(this->GetStats().GetDEF() - this->GetStats().DEF_base));
    description.push_back("Elemental Mastery: " +
                          std::to_string(this->GetStats().elemental_mastery));
    description.push_back("Max Stamina: " +
                          std::to_string(this->GetStats().max_stamina));

    auto DoubleToString = [](double x) {
      std::ostringstream oss;
      oss << std::fixed << std::setprecision(1) << x;
      return oss.str();
    };

    // Advanced stats
    description.push_back(
        "CRIT Rate: " + DoubleToString(this->GetStats().CRIT_rate * 100) +
        "%%");
    description.push_back(
        "CRIT DMG: " + DoubleToString(this->GetStats().CRIT_DMG * 100) + "%%");
    description.push_back("Healing Bonus: " +
                          DoubleToString(this->GetStats().healing_bonus * 100) +
                          "%%");
    description.push_back(
        "Incoming Healing Bonus: " +
        DoubleToString(this->GetStats().incoming_healing_bonus * 100) + "%%");
    description.push_back(
        "Energy Recharge: " +
        DoubleToString(this->GetStats().energy_recharge * 100) + "%%");
    description.push_back(
        "CD Reduction: " + DoubleToString(this->GetStats().CD_reduction * 100) +
        "%%");
    description.push_back(
        "Shield Strength: " +
        DoubleToString(this->GetStats().shield_strength * 100) + "%%");

    // Elemental type
    description.push_back(
        "Pyro DMG Bonus: " +
        DoubleToString(this->GetStats().pyro_DMG_bonus * 100) + "%%");
    description.push_back(
        "Pyro RES: " + DoubleToString(this->GetStats().pyro_RES * 100) + "%%");

    description.push_back(
        "Hydro DMG Bonus: " +
        DoubleToString(this->GetStats().hydro_DMG_bonus * 100) + "%%");

    description.push_back(
        "Hydro RES: " + DoubleToString(this->GetStats().hydro_RES * 100) +
        "%%");

    description.push_back(
        "Dendro DMG Bonus: " +
        DoubleToString(this->GetStats().dendro_DMG_bonus * 100) + "%%");

    description.push_back(
        "Dendro RES: " + DoubleToString(this->GetStats().dendro_RES * 100) +
        "%%");

    description.push_back(
        "Electro DMG Bonus: " +
        DoubleToString(this->GetStats().electro_DMG_bonus * 100) + "%%");

    description.push_back(
        "Electro RES: " + DoubleToString(this->GetStats().electro_RES * 100) +
        "%%");

    description.push_back(
        "Anemo DMG Bonus: " +
        DoubleToString(this->GetStats().anemo_DMG_bonus * 100) + "%%");

    description.push_back(
        "Anemo RES: " + DoubleToString(this->GetStats().anemo_RES * 100) +
        "%%");

    description.push_back(
        "Cryo DMG Bonus: " +
        DoubleToString(this->GetStats().cryo_DMG_bonus * 100) + "%%");

    description.push_back(
        "Cryo RES: " + DoubleToString(this->GetStats().cryo_RES * 100) + "%%");

    description.push_back("Geo DMG Bonus: " +
                          DoubleToString(this->GetStats().geo_DMG_bonus * 100) +
                          "%%");

    description.push_back(
        "Geo RES: " + DoubleToString(this->GetStats().geo_RES * 100) + "%%");

    description.push_back(
        "Physical DMG Bonus: " +
        DoubleToString(this->GetStats().physical_DMG_bonus * 100) + "%%");

    description.push_back(
        "Physical RES: " + DoubleToString(this->GetStats().physical_RES * 100) +
        "%%");
  }

  return description;
}

int Character::GetTalentElementalBurstLevel() const {
  return this->talent_elemental_burst_level_;
}

int Character::GetTalentElementalSkillLevel() const {
  return this->talent_elemental_skill_level_;
}

int Character::GetTalentNormalAttackLevel() const {
  return this->talent_normal_attack_level_;
}

void Character::IncreaseAscensionPhase() {
  if (this->GetLevel() ==
      Character::kAcensionPhaseMaxLevelList
          [this->ascension_phase_]) {  // if it is time to ascend
    this->ascension_phase_ = std::min(this->ascension_phase_ + 1, 6);
  }
}

void Character::IncreaseCharacterEXP(int value) {
  this->character_EXP_ += std::max(value, 0);
}

void Character::IncreaseConstellation() {
  this->constellation_ = std::min(this->constellation_ + 1, 6);
}

void Character::IncreaseEnergy(int value) {
  this->energy_ += value;
  this->energy_ = std::min(this->energy_, this->GetEnergyMax());
  this->energy_ = std::max(this->energy_, 0);
}

void Character::IncreaseFullness(double value) {
  this->fullness_ += value;
  this->fullness_ = std::max(this->fullness_, 0.);

  // When increase to full, the state keeps 30 seconds
  if (this->fullness_ > 100. && value > 0) {
    this->fullness_ = 109.;
  }
}

void Character::IncreaseHP(int value) {
  // Dead characters cannot be healed
  if (value > 0 && this->HP_ == 0) {
    return;
  }

  if (value == 0) {
    return;
  }

  this->HP_ += value;
  this->HP_ = std::max(0, this->HP_);
  this->HP_ = std::min(this->GetStats().GetMaxHP(), this->HP_);

  if (value < 900000) {  // only respawning will reach such a large value
    this->playerex_->GetPlayer()->sendTitlePacket(
        ((value > 0) ? "§a+" : "§c-") +
            std::to_string(static_cast<int>(std::abs(value))),
        TitleType::SetActionBar, 0, 1, 0);
  }
}

bool Character::IsDead() const { return (this->HP_ == 0); }

bool Character::IsEnergyFull() const {
  return (this->energy_ == this->GetEnergyMax());
}

void Character::Refresh() {
  // Refresh the modifiers
  auto now_clock = GetNowClock();
  for (auto it = this->modifier_list_.begin();
       it != this->modifier_list_.end();) {
    if (it->GetExpiredClock() < now_clock) {
      this->modifier_list_.erase(it);
    } else {
      ++it;
    }
  }
}

void Character::RemoveModifier(int id) {
  for (auto it = this->modifier_list_.begin(); it != this->modifier_list_.end();
       ++it) {
    if (it->GetID() == id) {
      this->modifier_list_.erase(it);
      break;
    }
  }
}

void Character::Revive() {
  if (this->HP_ == 0) {
    this->HP_ = 1;
  }
}

std::shared_ptr<Character> Character::Make(
    PlayerEx* playerex, const std::string& name, int ascension_phase,
    int character_EXP, int constellation, int energy, int HP,
    int talent_elemental_burst_level, int talent_elemental_skill_level,
    int talent_normal_attack_level) {
  if (name == "Kuki Shinobu") {
    return std::make_shared<KukiShinobu>(
        playerex, ascension_phase, character_EXP, constellation, energy, HP,
        talent_elemental_burst_level, talent_elemental_skill_level,
        talent_normal_attack_level);
  }

  throw ExceptionNotACharacter();
}

const int Character::kLevelMinCharacterEXPList[91] = {
    0,       0,       1000,    2325,    4025,    6175,    8800,    11950,
    15675,   20025,   25025,   30725,   37175,   44400,   52450,   61375,
    71200,   81950,   93675,   106400,  120175,  135050,  151850,  169850,
    189100,  209650,  231525,  254775,  279425,  305525,  333100,  362200,
    392850,  425100,  458975,  494525,  531775,  570750,  611500,  654075,
    698500,  744800,  795425,  848125,  902900,  959800,  1018875, 1080150,
    1143675, 1209475, 1277600, 1348075, 1424575, 1503625, 1585275, 1669550,
    1756500, 1846150, 1938550, 2033725, 2131725, 2232600, 2341550, 2453600,
    2568775, 2687100, 2808625, 2933400, 3061475, 3192875, 3327650, 3465825,
    3614525, 3766900, 3922975, 4082800, 4246400, 4413825, 4585125, 4760350,
    4939525, 5122700, 5338925, 5581950, 5855050, 6161850, 6506450, 6893400,
    7327825, 7815450, 8362650};

Character::Character(PlayerEx* playerex, int ascension_phase, int character_EXP,
                     int constellation, int energy, int HP,
                     int talent_elemental_burst_level,
                     int talent_elemental_skill_level,
                     int talent_normal_attack_level)
    : last_elemental_burst_clock_(GetNowClock()),
      last_elemental_skill_clock_(GetNowClock()),
      ascension_phase_(ascension_phase),
      character_EXP_(character_EXP),
      constellation_(constellation),
      fullness_(0.),
      energy_(energy),
      HP_(HP),
      playerex_(playerex),
      talent_elemental_burst_level_(talent_elemental_burst_level),
      talent_elemental_skill_level_(talent_elemental_skill_level),
      talent_normal_attack_level_(talent_normal_attack_level) {
  // Validate the arguments
  if (ascension_phase < 0 || ascension_phase > 6) {
    throw ExceptionInvalidCharacterData();
  }
  if (character_EXP < 0) {
    throw ExceptionInvalidCharacterData();
  }
  if (constellation < 0 || constellation > 6) {
    throw ExceptionInvalidCharacterData();
  }
  if (HP < 0) {
    throw ExceptionInvalidCharacterData();
  }
  if (talent_elemental_burst_level < 0 || talent_elemental_burst_level > 10) {
    throw ExceptionInvalidCharacterData();
  }
  if (talent_elemental_skill_level < 0 || talent_elemental_skill_level > 10) {
    throw ExceptionInvalidCharacterData();
  }
  if (talent_normal_attack_level < 0 || talent_normal_attack_level > 10) {
    throw ExceptionInvalidCharacterData();
  }
}

}  // namespace genshicraft