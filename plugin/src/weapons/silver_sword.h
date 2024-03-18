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
 * @file SilverSword.h
 * @author Futrime (futrime@outlook.com)
 * @brief Declaration of the SilverSword class
 * @version 1.0.0
 * @date 2022-08-10
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#ifndef GENSHICRAFT_WEAPONS_SILVER_SWORD_H_
#define GENSHICRAFT_WEAPONS_SILVER_SWORD_H_

#include <MC/ItemStack.hpp>
#include <string>

#include "character.h"
#include "playerex.h"
#include "stats.h"
#include "weapon.h"

namespace genshicraft {

/**
 * @brief The SilverSword class contains interfaces for Dull Blade.
 *
 */
class SilverSword : public Weapon {
 public:
  /**
   * @brief Construct a new Silver Sword object
   *
   * @param item The weapon item
   * @param playerex The PlayerEx object of the holder
   *
   * @exception ExceptionNotAWeapon The item is not a GenshiCraft weapon
   */
  SilverSword(ItemStack* item, PlayerEx* playerex);

  SilverSword() = delete;

  /**
   * @brief Get the ascension materials
   *
   * @return The ascension materials
   */
  std::map<std::string, int> GetAscensionMaterials() const override;

  /**
   * @brief Get the base stats
   *
   * @param stats The stats
   */
  Stats GetBaseStats() const override;

  /**
   * @brief Get the name
   *
   * @return "Silver Sword"
   */
  std::string GetName() const override;

  /**
   * @brief Get the rarity of the weapon
   *
   * @return 2
   */
  int GetRarity() const override;

  /**
   * @brief Get the max refinement
   *
   * @return 1
   */
  int GetRefinementMax() const override;

  /**
   * @brief Get the weapon type
   *
   * @return Weapon::Type::kSword
   */
  Weapon::Type GetType() const override;

 private:
  static const std::map<std::string, int> kAscensionMaterialsList[7];

  static const int
      kATKBase[5];  // the inferred 0-level ATK of each ascension phase

  static const int kATKDiff;  // the difference of ATK between levels
};

}  // namespace genshicraft

#endif  // GENSHICRAFT_WEAPONS_SILVER_SWORD_H_