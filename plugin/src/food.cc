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
 * @file food.cc
 * @author Futrime (futrime@outlook.com)
 * @brief Definition of food interfaces
 * @version 1.0.0
 * @date 2022-08-16
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#include "food.h"

#include <MC/ItemStack.hpp>
#include <map>
#include <string>
#include <vector>

#include "character.h"
#include "exceptions.h"
#include "playerex.h"
#include "plugin.h"

namespace genshicraft {

namespace food {

const std::map<std::string, std::string> kFoodDescriptionDict = {
    {"minecraft:apple", "Restores §l300§r HP."},
    {"genshicraft:sunsettia", "Restores §l300§r HP."}};

bool CheckIsFood(ItemStack* item) {
  if (kFoodDescriptionDict.count(item->getTypeName()) == 1) {
    return true;
  } else {
    return false;
  }
}

bool EatFood(PlayerEx* playerex, ItemStack* food) {
  if (!CheckIsFood(food)) {
    throw ExceptionNotFood();
  }

  auto food_name = food->getTypeName();

  // Players cannot eat GenshiCraft food if they are full
  if (playerex->GetCharacter()->GetFullness() > 100.) {
    if (food_name.substr(0, 12) == "genshicraft:") {
      return false;
    } else {
      return true;
    }
  }

  if (food_name == "minecraft:apple") {
    // Treat the apple as a native food if the player's HP is full
    if (playerex->GetHP() == playerex->GetStats().GetMaxHP()) {
      return true;
    }

    playerex->ConsumeItem("minecraft:apple", 1);
    playerex->IncreaseHP(300);
    playerex->GetCharacter()->IncreaseFullness(GetFullnessIncrement(
        0, true, playerex->GetCharacter()->GetStats().GetMaxHP()));
  }

  if (food_name == "genshicraft:sunsettia") {
    // Prevent eating if the player's HP is full,
    if (playerex->GetHP() == playerex->GetStats().GetMaxHP()) {
      return false;
    }

    playerex->GetCharacter()->IncreaseHP(300);
    playerex->GetCharacter()->IncreaseFullness(GetFullnessIncrement(
        0, true, playerex->GetCharacter()->GetStats().GetMaxHP()));
  }

  return true;
}

double GetFullnessIncrement(int rarity, bool is_instant, int max_HP) {
  double a = 0., b = 0.;
  if (rarity == 0) {
    a = 5.;
    b = 15000.;
  } else if (rarity == 1 && is_instant) {
    a = 14.;
    b = 45000.;
  } else if (rarity == 1 && !is_instant) {
    a = 14.;
    b = 69000.;
  } else if (rarity == 2 && is_instant) {
    a = 21.;
    b = 62500.;
  } else if (rarity == 12 && !is_instant) {
    a = 20.;
    b = 120000.;
  } else if (rarity == 3 && is_instant) {
    a = 26.;
    b = 87500.;
  } else if (rarity == 3 && !is_instant) {
    a = 24.;
    b = 267000.;
  }

  return a + b / max_HP;
}

void RegisterFood(PlayerEx* playerex, ItemStack* food) {
  if (!CheckIsFood(food)) {
    throw ExceptionNotFood();
  }

  if (food->getCustomLore().size() == 0) {
    food->setCustomLore(std::vector<std::string>{
        "§f" + kFoodDescriptionDict.at(food->getTypeName())});
    playerex->RefreshItems();
  }
}

}  // namespace food

}  // namespace genshicraft
