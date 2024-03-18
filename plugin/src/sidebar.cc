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
 * @file Sidebar.cc
 * @author Futrime (futrime@outlook.com)
 * @brief Definition of the Sidebar class
 * @version 1.0.0
 * @date 2022-08-08
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#include "sidebar.h"

#include <MC/Player.hpp>
#include <MC/Types.hpp>
#include <cmath>
#include <string>
#include <utility>
#include <vector>

#include "playerex.h"
#include "plugin.h"

namespace genshicraft {

Sidebar::Sidebar(PlayerEx* playerex) : playerex_(playerex) {
  // Empty
}

void Sidebar::Refresh() {
  std::vector<std::pair<std::string, int>> content;

  // Character name
  auto character_name =
      std::string((this->playerex_->GetCharacter()->HasWeapon()) ? "§f"
                                                                 : "§c") +
      this->playerex_->GetCharacter()->GetName();

  // Stamina
  auto stamina_progress = static_cast<double>(this->playerex_->GetStamina()) /
                          this->playerex_->GetStaminaMax();
  content.push_back(
      {"Stamina " + Sidebar::GenerateProgressBar(
                        stamina_progress, this->playerex_->GetStaminaMax() / 10,
                        (stamina_progress < 0.3)
                            ? "§c"
                            : ((stamina_progress > 0.999) ? "§a" : "§e")),
       0});

  // HP
  auto HP_progress =
      static_cast<double>(this->playerex_->GetCharacter()->GetHP()) /
      this->playerex_->GetCharacter()->GetStats().GetMaxHP();
  content.push_back(
      {"HP " + Sidebar::GenerateProgressBar(HP_progress, 36,
                                            (HP_progress < 0.3) ? "§c" : "§a"),
       1});

  // Fullness
  auto fullness_progress =
      std::min(this->playerex_->GetCharacter()->GetFullness(), 100.) / 100.;
  content.push_back(
      {"Fullness " + Sidebar::GenerateProgressBar(
                         fullness_progress, 21,
                         (fullness_progress < 0.75)
                             ? "§a"
                             : ((fullness_progress > 0.999) ? "§c" : "§e")),
       2});

  // Elemental Skill CD
  auto elemental_skill_CD_progress =
      1 - static_cast<double>(
              this->playerex_->GetCharacter()->GetCDElementalSkill()) /
              this->playerex_->GetCharacter()->GetCDElementalSkillMax();
  content.push_back(
      {"Skill CD " + Sidebar::GenerateProgressBar(
                         elemental_skill_CD_progress, 24,
                         (elemental_skill_CD_progress > 0.999) ? "§a" : "§e"),
       3});

  // Elemental Burst CD
  auto elemental_burst_CD_progress =
      1 - static_cast<double>(
              this->playerex_->GetCharacter()->GetCDElementalBurst()) /
              this->playerex_->GetCharacter()->GetCDElementalBurstMax();
  content.push_back(
      {"Burst CD " + Sidebar::GenerateProgressBar(
                         elemental_burst_CD_progress, 20,
                         (elemental_burst_CD_progress > 0.999) ? "§a" : "§e"),
       4});

  // Elemental Burst Energy
  auto elemental_burst_energy_progress =
      static_cast<double>(this->playerex_->GetCharacter()->GetEnergy()) /
      this->playerex_->GetCharacter()->GetEnergyMax();
  content.push_back(
      {"Energy " + Sidebar::GenerateProgressBar(
                       elemental_burst_energy_progress, 24,
                       (elemental_burst_energy_progress > 0.999) ? "§a" : "§e"),
       5});

  auto player = this->playerex_->GetPlayer();
  player->removeSidebar();
  player->setSidebar(character_name, content, ObjectiveSortOrder::Ascending);
}

std::string Sidebar::GenerateProgressBar(double value, int steps,
                                         std::string formatting_codes) {
  auto filled_steps = static_cast<int>(value * steps);

  std::string progress_bar = formatting_codes;
  for (int i = 0; i < steps; ++i) {
    if (i < filled_steps) {
      progress_bar += "|";
    } else {
      progress_bar += ".";
    }
  }

  return progress_bar;
}

}  // namespace genshicraft
