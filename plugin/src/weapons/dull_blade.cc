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
 * @file dull_blade.h
 * @author Futrime (futrime@outlook.com)
 * @brief Definition of the DullBlade class
 * @version 1.0.0
 * @date 2022-08-10
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#include "dull_blade.h"

#include <MC/ItemStack.hpp>
#include <string>

#include "character.h"
#include "exceptions.h"
#include "playerex.h"
#include "stats.h"
#include "weapon.h"

namespace genshicraft {

DullBlade::DullBlade(ItemStack* item, PlayerEx* playerex)
    : Weapon(item, playerex) {
  // Check if the item is a GenshiCraft item
  if (!Weapon::CheckIsWeapon(item)) {
    throw ExceptionNotAWeapon();
  }

  this->ApplyLore(item, playerex);
}

std::map<std::string, int> DullBlade::GetAscensionMaterials() const {
  return DullBlade::kAscensionMaterialsList[this->GetAscensionPhase()];
}

Stats DullBlade::GetBaseStats() const {
  Stats stats;
  stats.ATK_base = DullBlade::kATKBase[this->GetAscensionPhase()] +
                   DullBlade::kATKDiff * this->GetLevel();
  return stats;
}

std::string DullBlade::GetName() const { return "Dull Blade"; }

int DullBlade::GetRarity() const { return 1; }

int DullBlade::GetRefinementMax() const { return 1; }

Weapon::Type DullBlade::GetType() const { return Weapon::Type::kSword; }

const std::map<std::string, int> DullBlade::kAscensionMaterialsList[7] = {
    {},
    {{"genshicraft:mora_1", 5000}},
    {{"genshicraft:mora_1", 5000}},
    {{"genshicraft:mora_1", 10000}},
    {},
    {},
    {}};

const int DullBlade::kATKBase[5] = {22, 48, 73, 91, 109};

const int DullBlade::kATKDiff = 1;

}  // namespace genshicraft