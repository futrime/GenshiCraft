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
 * @file weapon.h
 * @author Futrime (futrime@outlook.com)
 * @brief Declaration of the Weapon class
 * @version 1.0.0
 * @date 2022-08-04
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#ifndef GENSHICRAFT_WEAPON_H_
#define GENSHICRAFT_WEAPON_H_

#include <MC/ItemStack.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "character.h"
#include "stats.h"

namespace genshicraft {

class PlayerEx;

/**
 * @brief The Weapon class contains interfaces for weapons.
 *
 */
class Weapon {
 public:
  const static int kAcensionPhaseMaxLevelList[7];  // the maximum level of each
                                                   // acension phase

  /**
   * @brief The types of weapons
   *
   */
  enum class Type { kSword = 0, kClaymore, kPolearm, kCatalyst, kBow };

  Weapon() = delete;

  /**
   * @brief Apply the lore to the weapon item
   *
   * @param item The weapon item
   * @param playerex A pointer to the PlayerEx object of the holder
   *
   * @exception ExceptionNotAWeapon The item is not a GenshiCraft weapon
   */
  void ApplyLore(ItemStack *item, PlayerEx *playerex);

  /**
   * @brief Get the ascension materials
   *
   * @return The ascension materials
   */
  virtual std::map<std::string, int> GetAscensionMaterials() const = 0;

  /**
   * @brief Get the ascension phase
   *
   * @return The ascension phase (0 ~ 4 for 1-Star and 2-Star weapons, 0 ~ 6
   * for others)
   */
  int GetAscensionPhase() const;

  /**
   * @brief Get the base stats
   *
   * @param stats The stats
   */
  virtual Stats GetBaseStats() const = 0;

  /**
   * @brief Describe the base stats
   *
   * @return The description
   */
  std::vector<std::string> GetBaseStatsDescription() const;

  /**
   * @brief Get the level of the weapon
   *
   * @return The level of the weapon (1 ~ 70 for 1-Star and 2-Star weapons, 1 ~
   * 90 for others)
   */
  int GetLevel() const;

  /**
   * @brief Predict the level with the weapon EXP provided
   *
   * @param weapon_exp The weapon EXP
   * @return The level
   */
  int GetLevelByWeaponEXP(int weapon_exp) const;

  /**
   * @brief Get the name
   *
   * @return The name
   */
  virtual std::string GetName() const = 0;

  /**
   * @brief Get the rarity of the weapon
   *
   * @return The rarity of the weapon (1 ~ 5)
   */
  virtual int GetRarity() const = 0;

  /**
   * @brief Get the refinement
   *
   * @return The refinement (1 ~ 5)
   */
  int GetRefinement() const;

  /**
   * @brief Get the max refinement
   *
   * @return The max refinement (1 or 5)
   */
  virtual int GetRefinementMax() const = 0;

  /**
   * @brief Get the weapon type
   *
   * @return The weapon type
   */
  virtual Type GetType() const = 0;

  /**
   * @brief Get the weapon EXP
   *
   * @return The weapon EXP
   */
  int GetWeaponEXP() const;

  /**
   * @brief Increase 1 ascension phase till 4 (for 1-Star and 2-Star weapons) or
   * 6 (for others). If not time to ascend, it will not take effect.
   *
   */
  void IncreaseAscensionPhase();

  /**
   * @brief Increase 1 refinement till 5. 1-Star and 2-Star weapons will not
   * take effect.
   *
   */
  void IncreaseRefinement();

  /**
   * @brief Increase the weapon EXP
   *
   * @param value The value to increase. Negative value will not take effect.
   */
  void IncreaseWeaponEXP(int value);

  /**
   * @brief Check if the item is a GenshiCraft weapon
   *
   * @param item The item
   * @return True if the item is a GenshiCraft weapon
   */
  static bool CheckIsWeapon(ItemStack *item);

  /**
   * @brief Make a Weapon object
   *
   * @param item The weapon item
   * @param playerex A pointer to the PlayerEx object of the holder
   * @return A pointer to the weapon object
   *
   * @exception ExceptionNotAWeapon The item is not a GenshiCraft weapon
   */
  static std::shared_ptr<Weapon> Make(ItemStack *item, PlayerEx *playerex);

 protected:
  /**
   * @brief Construct a new Weapon object
   *
   * @param item The weapon item
   * @param playerex The PlayerEx object of the player of the holder
   *
   * @exception ExceptionNotAWeapon The item is not a GenshiCraft weapon
   */
  Weapon(ItemStack *item, PlayerEx *playerex);

  /**
   * @brief Destroy the Weapon object
   *
   */
  virtual ~Weapon();

 private:
  const static std::vector<std::string>
      kIdentifierList;  // identifiers of all weapons

  const static int
      k1StarLevelMinWeaponEXPList[71];  // weapon EXP required by each
                                        // level of 1-Star weapons
  const static int
      k2StarLevelMinWeaponEXPList[71];  // weapon EXP required by each
                                        // level of 2-Star weapons
  const static int
      k3StarLevelMinWeaponEXPList[91];  // weapon EXP required by each
                                        // level of 3-Star weapons
  const static int
      k4StarLevelMinWeaponEXPList[91];  // weapon EXP required by each
                                        // level of 4-Star weapons
  const static int
      k5StarLevelMinWeaponEXPList[91];  // weapon EXP required by each
                                        // level of 5-Star weapons

  int ascension_phase_;  // the Ascension Phase
  ItemStack *item_;      // the ItemStack object
  PlayerEx *playerex_;   // the PlayerEx object of the owner
  int refinement_;       // the Refinement
  int weapon_exp_;       // the Weapon EXP
};

}  // namespace genshicraft

#endif  // GENSHICRAFT_WEAPON_H_