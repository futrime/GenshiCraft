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
 * @file kuki_shinobu.cc
 * @author Futrime (futrime@outlook.com)
 * @brief Definition of the KukiShinobu class
 * @version 1.0.0
 * @date 2022-08-09
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#include "kuki_shinobu.h"

#include <map>
#include <memory>
#include <string>

#include "character.h"
#include "damage.h"
#include "exceptions.h"
#include "playerex.h"
#include "plugin.h"
#include "stats.h"
#include "weapon.h"
#include "world.h"

namespace genshicraft {

KukiShinobu::KukiShinobu(PlayerEx* playerex, int ascension_phase,
                         int character_EXP, int constellation, int energy,
                         int HP, int talent_elemental_burst_level,
                         int talent_elemental_skill_level,
                         int talent_normal_attack_level)
    : Character(playerex, ascension_phase, character_EXP, constellation, energy,
                HP, talent_elemental_burst_level, talent_elemental_skill_level,
                talent_normal_attack_level) {
  if (HP > this->GetStats().GetMaxHP()) {
    throw ExceptionInvalidCharacterData();
  }
}

std::map<std::string, int> KukiShinobu::GetAscensionMaterials() const {
  return KukiShinobu::kAscensionMaterialsList[this->GetAscensionPhase()];
}

Stats KukiShinobu::GetBaseStats() const {
  Stats stats;
  stats.max_HP_base = KukiShinobu::kStatsMaxHPBase[this->GetAscensionPhase()] +
                      KukiShinobu::kStatsMaxHPDiff * this->GetLevel();
  stats.ATK_base = KukiShinobu::kStatsATKBase[this->GetAscensionPhase()] +
                   KukiShinobu::kStatsATKDiff * this->GetLevel();
  stats.DEF_base = KukiShinobu::kStatsDEFBase[this->GetAscensionPhase()] +
                   KukiShinobu::kStatsDEFDiff * this->GetLevel();

  stats.max_HP_percent =
      KukiShinobu::kStatsMaxHPPercent[this->GetAscensionPhase()];

  return stats;
}

double KukiShinobu::GetCDElementalBurstMax() const { return 15.; };

double KukiShinobu::GetCDElementalSkillMax() const { return 15.; };

Damage KukiShinobu::GetDamageElementalBurst() {
  if (this->GetCDElementalBurst() < 0.0001 && this->IsEnergyFull()) {
    this->last_elemental_burst_clock_ = GetNowClock();
    this->IncreaseEnergy(-60);
    return Damage();
  }
  return Damage();
}

Damage KukiShinobu::GetDamageElementalSkill() {
  if (this->GetCDElementalSkill() < 0.0001) {
    this->last_elemental_skill_clock_ = GetNowClock();
    return Damage();
  }
  return Damage();
}

Damage KukiShinobu::GetDamageNormalAttack() {
  static int hit_count = 1;
  static auto last_hit_clock = GetNowClock();

  Damage damage;

  if (!(this->GetPlayerEx()->GetPlayer()->isOnGround()) &&
      (this->GetPlayerEx()->GetPlayer()->isSneaking())) {  // plunge

    damage.SetAttackerAmplifier(this->kTalentNormalAttackLowPlungeDMG
                                    [this->GetTalentNormalAttackLevel()]);

  } else if ((this->GetPlayerEx()->GetPlayer()->isSneaking()) &&
             (this->GetPlayerEx()->GetStamina() >
              this->kTalentNormalAttackChargedAttackStaminaCost)) {  // charged
                                                                     // attack

    damage.SetAttackerAmplifier(this->kTalentNormalAttackChargedAttackDMG
                                    [this->GetTalentNormalAttackLevel()]);
    this->GetPlayerEx()->IncreaseStamina(
        -(this->kTalentNormalAttackChargedAttackStaminaCost));
  } else {  // normal attack

    // Reset hit count if not attacking in 5s
    if (GetNowClock() - last_hit_clock > 5) {
      hit_count = 1;
    }

    damage.SetAttackerAmplifier(
        this->kTalentNormalAttackHitDMG[hit_count]
                                       [this->GetTalentNormalAttackLevel()]);

    ++hit_count;
    if (hit_count > 4) {
      hit_count = 1;
    }

    last_hit_clock = GetNowClock();
  }

  damage.SetAttackElementType(world::ElementType::kPhysical);
  damage.SetAttackerLevel(this->GetLevel());
  damage.SetAttackerStats(this->GetStats());
  damage.SetSourceType(Damage::SourceType::kMob);

  return damage;
}

int KukiShinobu::GetEnergyMax() const { return 60; }

std::string KukiShinobu::GetName() const { return "Kuki Shinobu"; }

int KukiShinobu::GetRarity() const { return 4; }

bool KukiShinobu::HasWeapon() const {
  if (this->GetPlayerEx()->GetWeapon() &&
      this->GetPlayerEx()->GetWeapon()->GetType() == Weapon::Type::kSword) {
    return true;
  } else {
    return false;
  }
}

const std::map<std::string, int> KukiShinobu::kAscensionMaterialsList[7] = {
    {{"genshicraft:mora_1", 20000}},
    {{"genshicraft:mora_1", 40000}},
    {{"genshicraft:mora_1", 60000}},
    {{"genshicraft:mora_1", 80000}},
    {{"genshicraft:mora_1", 100000}},
    {{"genshicraft:mora_1", 120000}},
    {}};

const int KukiShinobu::kStatsATKBase[7] = {17, 39, 58, 75, 90, 104, 118};

const int KukiShinobu::kStatsATKDiff = 1;

const int KukiShinobu::kStatsDEFBase[7] = {58, 109, 146, 193, 228, 263, 299};

const int KukiShinobu::kStatsDEFDiff = 5;

const int KukiShinobu::kStatsMaxHPBase[7] = {945,  1717, 2265, 2995,
                                             3543, 4090, 4638};

const int KukiShinobu::kStatsMaxHPDiff = 85;

const double KukiShinobu::kStatsMaxHPPercent[7] = {0,    0,    0.06, 0.12,
                                                   0.12, 0.18, 0.24};

const double KukiShinobu::kTalentNormalAttackChargedAttackDMG[12] = {
    0,      1.2240, 1.3236, 1.4232, 1.5656, 1.6652,
    1.7790, 1.9356, 2.0921, 2.2487, 2.4195, 2.5903};

const int KukiShinobu::kTalentNormalAttackChargedAttackStaminaCost = 20;

const double KukiShinobu::kTalentNormalAttackHitDMG[5][12] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0.4876, 0.5273, 0.5670, 0.6237, 0.6634, 0.7088, 0.7711, 0.8335, 0.8959,
     0.9639, 1.0319},
    {0, 0.4455, 0.4817, 0.5180, 0.5698, 0.6061, 0.6475, 0.7045, 0.7615, 0.8184,
     0.8806, 0.9428},
    {0, 0.5934, 0.6417, 0.6900, 0.7590, 0.8073, 0.8625, 0.9384, 1.0143, 1.0902,
     1.1730, 1.2558},
    {0, 0.7611, 0.8230, 0.8850, 0.9735, 1.0355, 1.1063, 1.2036, 1.3009, 1.3983,
     1.5045, 1.6107},
};

const double KukiShinobu::kTalentNormalAttackLowPlungeDMG[12] = {
    0,      1.2784, 1.3824, 1.4865, 1.6351, 1.7392,
    1.8581, 2.0216, 2.1851, 2.3486, 2.5270, 2.7054};

}  // namespace genshicraft
