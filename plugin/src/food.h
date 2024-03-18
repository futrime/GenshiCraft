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
 * @file food.h
 * @author Futrime (futrime@outlook.com)
 * @brief Declaration of food interfaces
 * @version 1.0.0
 * @date 2022-08-16
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#ifndef GENSHICRAFT_FOOD_H_
#define GENSHICRAFT_FOOD_H_

#include <MC/ItemStack.hpp>
#include <map>
#include <string>

#include "playerex.h"

namespace genshicraft {

namespace food {

extern const std::map<std::string, std::string>
    kFoodDescriptionDict;  // the food descriptions

/**
 * @brief Check if the items are food
 *
 * @param item The items
 * @return True if the items are food
 */
bool CheckIsFood(ItemStack* item);

/**
 * @brief Eat food
 *
 * @param playerex The player
 * @param food The food
 * @return True if the food can be eaten
 * @exception ExceptionNotFood The items are not food.
 */
bool EatFood(PlayerEx* playerex, ItemStack* food);

/**
 * @brief Get the fullness increment for recovery dishes
 *
 * @param rarity The rarity
 * @param is_instant True if the food is instant recovery dishes
 * @param max_HP The HP of the character
 * @return The fullness increment
 */
double GetFullnessIncrement(int rarity, bool is_instant, int max_HP);

/**
 * @brief Register food
 *
 * @param playerex The player
 * @param food The food
 * @exception ExceptionNotFood The items are not food.
 */
void RegisterFood(PlayerEx* playerex, ItemStack* food);

}  // namespace food

}  // namespace genshicraft

#endif  // GENSHICRAFT_FOOD_H_