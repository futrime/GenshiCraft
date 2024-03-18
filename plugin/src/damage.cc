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
 * @file Damage.cc
 * @author Futrime (futrime@outlook.com)
 * @brief Definition of the Damage class
 * @version 1.0.0
 * @date 2022-08-10
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#include "damage.h"

#include <algorithm>
#include <map>
#include <random>

#include "character.h"
#include "exceptions.h"
#include "plugin.h"
#include "stats.h"

namespace genshicraft {

Damage::Damage()
    : source_type_(Damage::SourceType::kMob),

      attack_element_(world::ElementType::kPhysical),
      is_secondary_(false),
      is_secondary_swirl_(false),
      secondary_reaction_type_(world::ElementalReactionType::kNone),
      true_damage_proportion_(0.),

      attacker_amplifier_(1.),
      attacker_level_(1),
      attacker_stats_(Stats()),

      victim_element_(world::ElementType::kPhysical),
      victim_level_(1),
      victim_stats_(Stats()) {
  // Empty
}

double Damage::Get() const {
  const static std::map<world::ElementalReactionType, double>
      kReactionMultiplierDict = {
          {world::ElementalReactionType::kElectroCharged, 1.2},
          {world::ElementalReactionType::kOverloaded, 2.0},
          {world::ElementalReactionType::kShattered, 1.5},
          {world::ElementalReactionType::kSuperconduct, 0.5},
          {world::ElementalReactionType::kSwirl, 0.6}};

  static std::default_random_engine random_engine;
  static std::uniform_real_distribution<> dist(0, 1);

  // True damage
  if (this->IsTrueDamage()) {
    return this->true_damage_proportion_ * this->victim_stats_.GetMaxHP();
  }

  // Normal damage
  double damage = 0.;

  if (this->GetElementalReactionGroup() ==
          world::ElementalReactionGroup::kNone ||
      this->GetElementalReactionGroup() ==
          world::ElementalReactionGroup::kAmplifying ||
      this->GetElementalReactionGroup() ==
          world::ElementalReactionGroup::kTransformative) {
    // Base damage
    damage = this->attacker_stats_.GetATK() * this->attacker_amplifier_;

    // Damage bonus
    double damage_bonus = 1.;

    switch (this->attack_element_) {
      case world::ElementType::kPyro:
        damage_bonus += this->attacker_stats_.pyro_DMG_bonus;
        break;

      case world::ElementType::kHydro:
        damage_bonus += this->attacker_stats_.hydro_DMG_bonus;
        break;

      case world::ElementType::kDendro:
        damage_bonus += this->attacker_stats_.dendro_DMG_bonus;
        break;

      case world::ElementType::kElectro:
        damage_bonus += this->attacker_stats_.electro_DMG_bonus;
        break;

      case world::ElementType::kAnemo:
        damage_bonus += this->attacker_stats_.anemo_DMG_bonus;
        break;

      case world::ElementType::kCryo:
        damage_bonus += this->attacker_stats_.cryo_DMG_bonus;
        break;

      case world::ElementType::kGeo:
        damage_bonus += this->attacker_stats_.geo_DMG_bonus;
        break;

      case world::ElementType::kPhysical:
        damage_bonus += this->attacker_stats_.physical_DMG_bonus;
        break;

      default:
        break;
    }

    damage *= damage_bonus;

    // Critical hit
    if (dist(random_engine) < this->attacker_stats_.CRIT_rate) {
      damage *= (1. + this->attacker_stats_.CRIT_DMG);
    }

    // Defense
    damage *=
        ((this->attacker_level_ + 100) * 5.) /
        ((this->attacker_level_ + 100) * 5. + this->victim_stats_.GetDEF());
  }

  if (this->GetElementalReactionGroup() ==
      world::ElementalReactionGroup::kAmplifying) {
    // Amplifying reactions
    double reaction_bonus = 1.;

    if (this->attack_element_ == world::ElementType::kPyro &&
        this->victim_element_ == world::ElementType::kHydro) {
      reaction_bonus = 1.5;
    } else if (this->attack_element_ == world::ElementType::kHydro &&
               this->victim_element_ == world::ElementType::kPyro) {
      reaction_bonus = 2.;
    } else if (this->attack_element_ == world::ElementType::kPyro &&
               this->victim_element_ == world::ElementType::kCryo) {
      reaction_bonus = 2.;
    } else if (this->attack_element_ == world::ElementType::kCryo &&
               this->victim_element_ == world::ElementType::kPyro) {
      reaction_bonus = 1.5;
    }

    reaction_bonus *= 1. + (2.78 * this->attacker_stats_.elemental_mastery /
                            (this->attacker_stats_.elemental_mastery + 1400));

    damage *= reaction_bonus;
  }

  if (this->GetElementalReactionGroup() ==
      world::ElementalReactionGroup::kTransformativeSecondary) {
    // Level multiplier
    damage =
        18.2876719441606 + 1.84879588278956 * this->attacker_level_ +
        0.00930630666087628 * this->attacker_level_ * this->attacker_level_ +
        0.00163265442032016 * this->attacker_level_ * this->attacker_level_ *
            this->attacker_level_;

    // Reaction multiplier
    damage *= kReactionMultiplierDict.at(this->secondary_reaction_type_);

    // Elemental mastery bonus
    damage *= 1. + 16 * this->attacker_stats_.elemental_mastery /
                       (this->attacker_stats_.elemental_mastery + 2000);
  }

  // Resistance
  double RES = 0.;
  switch (this->GetElementType()) {
    case world::ElementType::kPyro:
      RES += this->victim_stats_.pyro_RES;
      break;

    case world::ElementType::kHydro:
      RES += this->victim_stats_.hydro_RES;
      break;

    case world::ElementType::kDendro:
      RES += this->victim_stats_.dendro_RES;
      break;

    case world::ElementType::kElectro:
      RES += this->victim_stats_.electro_RES;
      break;

    case world::ElementType::kAnemo:
      RES += this->victim_stats_.anemo_RES;
      break;

    case world::ElementType::kCryo:
      RES += this->victim_stats_.cryo_RES;
      break;

    case world::ElementType::kGeo:
      RES += this->victim_stats_.geo_RES;
      break;

    case world::ElementType::kPhysical:
      RES += this->victim_stats_.physical_RES;
      break;

    default:
      break;
  }

  if (RES < 0.) {
    damage *= 1. - RES / 2;
  } else if (RES < 0.75) {
    damage *= 1. - RES;
  } else {
    damage *= 1. / (1. + RES * 4);
  }

  damage = std::max(damage, 0.);  // the damage must not be negative

  return damage;
}

world::ElementalReactionGroup Damage::GetElementalReactionGroup() const {
  if (this->IsTrueDamage()) {
    throw ExceptionNotNormalDamage();
  }

  if (this->is_secondary_) {
    return world::ElementalReactionGroup::kTransformativeSecondary;
  }

  auto reaction_type = this->GetElementalReactionType();

  if (reaction_type == world::ElementalReactionType::kElectroCharged ||
      reaction_type == world::ElementalReactionType::kOverloaded ||
      reaction_type == world::ElementalReactionType::kShattered ||
      reaction_type == world::ElementalReactionType::kSuperconduct ||
      reaction_type == world::ElementalReactionType::kSwirl) {
    return world::ElementalReactionGroup::kTransformative;
  }

  if (reaction_type == world::ElementalReactionType::kMelt ||
      reaction_type == world::ElementalReactionType::kVaporize) {
    return world::ElementalReactionGroup::kAmplifying;
  }

  return world::ElementalReactionGroup::kNone;
}

world::ElementalReactionType Damage::GetElementalReactionType() const {
  if (this->IsTrueDamage()) {
    throw ExceptionNotNormalDamage();
  }

  if (this->is_secondary_ && !this->is_secondary_swirl_) {
    return world::ElementalReactionType::kNone;
  }

  if ((this->attack_element_ == world::ElementType::kDendro &&
       this->victim_element_ == world::ElementType::kHydro) ||
      (this->victim_element_ == world::ElementType::kDendro &&
       this->attack_element_ == world::ElementType::kHydro)) {
    return world::ElementalReactionType::kBloom;

  } else if ((this->attack_element_ == world::ElementType::kDendro &&
              this->victim_element_ == world::ElementType::kPyro) ||
             (this->victim_element_ == world::ElementType::kDendro &&
              this->attack_element_ == world::ElementType::kPyro)) {
    return world::ElementalReactionType::kBurning;

  } else if ((this->attack_element_ == world::ElementType::kDendro &&
              this->victim_element_ == world::ElementType::kElectro) ||
             (this->victim_element_ == world::ElementType::kDendro &&
              this->attack_element_ == world::ElementType::kElectro)) {
    return world::ElementalReactionType::kCatalyze;

  } else if ((this->attack_element_ == world::ElementType::kGeo &&
              (this->victim_element_ == world::ElementType::kCryo ||
               this->victim_element_ == world::ElementType::kElectro ||
               this->victim_element_ == world::ElementType::kHydro ||
               this->victim_element_ == world::ElementType::kPyro)) ||
             ((this->victim_element_ == world::ElementType::kCryo ||
               this->victim_element_ == world::ElementType::kElectro ||
               this->victim_element_ == world::ElementType::kHydro ||
               this->victim_element_ == world::ElementType::kPyro) &&
              this->victim_element_ == world::ElementType::kGeo)) {
    return world::ElementalReactionType::kCrystallize;

  } else if ((this->attack_element_ == world::ElementType::kElectro &&
              this->victim_element_ == world::ElementType::kHydro) ||
             (this->victim_element_ == world::ElementType::kElectro &&
              this->attack_element_ == world::ElementType::kHydro)) {
    return world::ElementalReactionType::kElectroCharged;

  } else if ((this->attack_element_ == world::ElementType::kCryo &&
              this->victim_element_ == world::ElementType::kHydro) ||
             (this->victim_element_ == world::ElementType::kCryo &&
              this->attack_element_ == world::ElementType::kHydro)) {
    return world::ElementalReactionType::kFrozen;

  } else if ((this->attack_element_ == world::ElementType::kCryo &&
              this->victim_element_ == world::ElementType::kPyro) ||
             (this->victim_element_ == world::ElementType::kCryo &&
              this->attack_element_ == world::ElementType::kPyro)) {
    return world::ElementalReactionType::kMelt;

  } else if ((this->attack_element_ == world::ElementType::kElectro &&
              this->victim_element_ == world::ElementType::kPyro) ||
             (this->victim_element_ == world::ElementType::kElectro &&
              this->attack_element_ == world::ElementType::kPyro)) {
    return world::ElementalReactionType::kOverloaded;

  } else if ((this->attack_element_ == world::ElementType::kCryo &&
              this->victim_element_ == world::ElementType::kElectro) ||
             (this->victim_element_ == world::ElementType::kCryo &&
              this->attack_element_ == world::ElementType::kElectro)) {
    return world::ElementalReactionType::kSuperconduct;

  } else if ((this->attack_element_ == world::ElementType::kAnemo &&
              (this->victim_element_ == world::ElementType::kCryo ||
               this->victim_element_ == world::ElementType::kElectro ||
               this->victim_element_ == world::ElementType::kHydro ||
               this->victim_element_ == world::ElementType::kPyro)) ||
             ((this->victim_element_ == world::ElementType::kCryo ||
               this->victim_element_ == world::ElementType::kElectro ||
               this->victim_element_ == world::ElementType::kHydro ||
               this->victim_element_ == world::ElementType::kPyro) &&
              this->victim_element_ == world::ElementType::kAnemo)) {
    return world::ElementalReactionType::kSwirl;

  } else if ((this->attack_element_ == world::ElementType::kHydro &&
              this->victim_element_ == world::ElementType::kPyro) ||
             (this->victim_element_ == world::ElementType::kHydro &&
              this->attack_element_ == world::ElementType::kPyro)) {
    return world::ElementalReactionType::kVaporize;

  } else {
    return world::ElementalReactionType::kNone;
  }
}

world::ElementType Damage::GetElementType() const {
  if (this->IsTrueDamage()) {
    throw ExceptionNotNormalDamage();
  }

  return this->attack_element_;
}

Damage Damage::GetSecondaryDamage() const {
  if (this->IsTrueDamage()) {
    throw ExceptionNotNormalDamage();
  }

  if (this->GetElementalReactionGroup() !=
          world::ElementalReactionGroup::kTransformative &&
      !(this->is_secondary_ && this->secondary_reaction_type_ ==
                                   world::ElementalReactionType::kSwirl)) {
    throw ExceptionNotTransformativeDamage();
  }

  Damage damage;

  damage.source_type_ = this->source_type_;

  switch (this->GetElementalReactionType()) {
    case world::ElementalReactionType::kElectroCharged:
      damage.attack_element_ = world::ElementType::kElectro;
      break;

    case world::ElementalReactionType::kOverloaded:
      damage.attack_element_ = world::ElementType::kPyro;
      break;

    case world::ElementalReactionType::kShattered:
      damage.attack_element_ = world::ElementType::kPhysical;
      break;

    case world::ElementalReactionType::kSuperconduct:
      damage.attack_element_ = world::ElementType::kCryo;
      break;

    case world::ElementalReactionType::kSwirl:
      damage.attack_element_ = this->victim_element_;
      break;

    default:
      break;
  }

  damage.is_secondary_ = true;
  damage.is_secondary_swirl_ = (this->GetElementalReactionType() ==
                                world::ElementalReactionType::kSwirl);
  damage.secondary_reaction_type_ = this->GetElementalReactionType();

  damage.attacker_amplifier_ = this->attacker_amplifier_;
  damage.attacker_level_ = this->attacker_level_;
  damage.attacker_stats_ = this->attacker_stats_;

  return damage;
}

Damage::SourceType Damage::GetSourceType() const {
  if (this->IsTrueDamage()) {
    throw ExceptionNotNormalDamage();
  }

  return this->source_type_;
}

bool Damage::IsTrueDamage() const {
  if (this->source_type_ == Damage::SourceType::kEnvironment) {
    return true;
  } else {
    return false;
  }
}

void Damage::SetAttackElementType(const world::ElementType& element) {
  if (this->IsTrueDamage()) {
    throw ExceptionNotNormalDamage();
  }

  this->attack_element_ = element;
}

void Damage::SetAttackerAmplifier(double amplifier) {
  if (this->IsTrueDamage()) {
    throw ExceptionNotNormalDamage();
  }

  this->attacker_amplifier_ = amplifier;
}

void Damage::SetAttackerLevel(int level) {
  if (this->IsTrueDamage()) {
    throw ExceptionNotNormalDamage();
  }

  this->attacker_level_ = level;
}

void Damage::SetAttackerStats(const Stats& stats) {
  if (this->IsTrueDamage()) {
    throw ExceptionNotNormalDamage();
  }

  this->attacker_stats_ = stats;
}

void Damage::SetSourceType(const Damage::SourceType& source_type) {
  if (this->is_secondary_) {
    throw ExceptionNotPrimaryDamage();
  }

  this->source_type_ = source_type;
};

void Damage::SetTrueDamageProportion(double proportion) {
  if (!this->IsTrueDamage()) {
    throw ExceptionNotTrueDamage();
  }

  this->true_damage_proportion_ = proportion;
}

void Damage::SetVictimAttachedElement(const world::ElementType& element) {
  this->victim_element_ = element;
}

void Damage::SetVictimLevel(int level) { this->victim_level_ = level; }

void Damage::SetVictimStats(const Stats& stats) { this->victim_stats_ = stats; }

}  // namespace genshicraft
