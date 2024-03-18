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
 * @file damage.h
 * @author Futrime (futrime@outlook.com)
 * @brief Declaration of the damage class
 * @version 1.0.0
 * @date 2022-08-10
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#ifndef GENSHICRAFT_DAMAGE_H_
#define GENSHICRAFT_DAMAGE_H_

#include "stats.h"
#include "world.h"

namespace genshicraft {

/**
 * @brief The Damage class is used for damage calculation.
 *
 */
class Damage {
 public:
  /**
   * @brief The source type
   *
   */
  enum class SourceType { kMob = 0, kEnvironment };

  /**
   * @brief Construct a new Damage object
   *
   */
  Damage();

  /**
   * @brief Get the damage value (or the max HP proportion of the true damage)
   *
   * @return The damage value or the proportion
   */
  double Get() const;

  /**
   * @brief Get the elemental reaction group
   *
   * @return The elemental reaction group.
   * @exception ExceptionNotNormalDamage The damage is not normal damage.
   *
   * @warning This method is only available for normal damage.
   */
  world::ElementalReactionGroup GetElementalReactionGroup() const;

  /**
   * @brief Get the elemental reaction type
   *
   * @return The elemental reaction type
   * @exception ExceptionNotNormalDamage The damage is not normal damage.
   *
   * @warning This method is only available for normal damage.
   */
  world::ElementalReactionType GetElementalReactionType() const;

  /**
   * @brief Get the element type of the damage
   *
   * @return The element type
   * @exception ExceptionNotNormalDamage The damage is not normal damage.
   *
   * @warning This method is only available for normal damage.
   */
  world::ElementType GetElementType() const;

  /**
   * @brief Get the secondary damage caused by transformative reactions
   *
   * @return The Damage object of the secondary damage
   * @exception ExceptionNotNormalDamage The damage is not normal damage.
   * @exception ExceptionNotTransformativeDamage The damage is not
   * transformative damage.
   *
   * @warning This method is only available for normal transformative damage.
   */
  Damage GetSecondaryDamage() const;

  /**
   * @brief Get the source type
   *
   * @return The source type
   * @exception ExceptionNotNormalDamage The damage is not normal damage.
   *
   * @warning This method is only available for normal damage.
   */
  SourceType GetSourceType() const;

  /**
   * @brief Check if the damage is true damage
   *
   * @return True if the damage is true damage
   */
  bool IsTrueDamage() const;

  /**
   * @brief Set the element type of the attack
   *
   * @param element The element type
   * @exception ExceptionNotNormalDamage The damage is not normal damage.
   *
   * @warning This method is only available for normal damage.
   */
  void SetAttackElementType(const world::ElementType& element);

  /**
   * @brief Set the skill amplifier of the attacker
   *
   * @param amplifier The amplifier
   * @exception ExceptionNotNormalDamage The damage is not normal damage.
   *
   * @warning This method is only available for normal damage.
   */
  void SetAttackerAmplifier(double amplifier);

  /**
   * @brief Set the level of the attacker
   *
   * @param level The level
   * @exception ExceptionNotNormalDamage The damage is not normal damage.
   *
   * @warning This method is only available for normal damage.
   */
  void SetAttackerLevel(int level);

  /**
   * @brief Set the stats of the attacker
   *
   * @param stats The stats
   * @exception ExceptionNotNormalDamage The damage is not normal damage.
   *
   * @warning This method is only available for normal damage.
   */
  void SetAttackerStats(const Stats& stats);

  /**
   * @brief Set the source type
   *
   * @param source_type The source type
   * @exception ExceptionNotPrimaryDamage The damage is not primary damage.
   *
   * @warning This method is only available for primary damage.
   */
  void SetSourceType(const SourceType& source_type);

  /**
   * @brief Set the proportion of HP to the max HP of the true damage
   *
   * @param proportion The proportion
   */
  void SetTrueDamageProportion(double proportion);

  /**
   * @brief Set the element attached to the victim
   *
   * @param element The element type
   */
  void SetVictimAttachedElement(const world::ElementType& element);

  /**
   * @brief Set the level of the victim
   *
   * @param level The level
   */
  void SetVictimLevel(int level);

  /**
   * @brief Set the stats of the victim
   *
   * @param stats The stats
   */
  void SetVictimStats(const Stats& stats);

 private:
  SourceType source_type_;  // the type of the damage source

  // The attack attributes
  world::ElementType attack_element_;
  bool is_secondary_;        // true if the damage is secondary damage of
                             // transformative reaction
  bool is_secondary_swirl_;  // true if the damge is the secondary damage of
                             // swirl reaction
  world::ElementalReactionType
      secondary_reaction_type_;    // the type of the secondary
                                   // transformative reaction
  double true_damage_proportion_;  // the proportion of the max HP dealt by the
                                   // true damage

  // The attacker attributes
  double attacker_amplifier_;
  int attacker_level_;  // the level of the attacker
  Stats attacker_stats_;

  // The victim attributes
  world::ElementType victim_element_;
  int victim_level_;  // the level of the victim
  Stats victim_stats_;
};

}  // namespace genshicraft

#endif  // GENSHICRAFT_DAMAGE_H_