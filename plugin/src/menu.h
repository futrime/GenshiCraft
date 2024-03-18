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
 * @file menu.h
 * @author Futrime (futrime@outlook.com)
 * @brief Declaration of the Menu class
 * @version 1.0.0
 * @date 2022-07-22
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#ifndef GENSHICRAFT_MENU_H_
#define GENSHICRAFT_MENU_H_

#include "artifact.h"

namespace genshicraft {

class PlayerEx;

/**
 * @brief The Menu class provide menu features to players.
 *
 */
class Menu {
 public:
  /**
   * @brief Construct a new Menu object
   *
   * @param playerex The PlayerEx the Menu object belonging to
   */
  explicit Menu(PlayerEx* playerex);

  Menu() = delete;

  /**
   * @brief Open the character menu
   *
   */
  void OpenCharacter();

  /**
   * @brief Open the character artifacts selection menu
   *
   */
  void OpenCharacterArtifacts();

  /**
   * @brief Open the character artifacts menu for a specific type
   *
   * @param type The type of the artifact
   */
  void OpenCharacterArtifacts(Artifact::Type type);

  /**
   * @brief Open the character ascend menu
   *
   */
  void OpenCharacterAscend();

  /**
   * @brief Open the character details menu
   *
   */
  void OpenCharacterDetails();

  /**
   * @brief Open the character level up menu
   *
   */
  void OpenCharacterLevelUp();

  /**
   * @brief Open the character weapon menu
   *
   */
  void OpenCharacterWeapon();

  /**
   * @brief Open the character weapon ascend menu
   *
   */
  void OpenCharacterWeaponAscend();

  /**
   * @brief Open the character weapon enhance menu
   *
   */
  void OpenCharacterWeaponEnhance();

  /**
   * @brief Open the character weapon refine menu
   *
   */
  void OpenCharacterWeaponRefine();

  /**
   * @brief Open the main menu
   *
   */
  void OpenMain();

  /**
   * @brief Open the party setup menu
   *
   */
  void OpenPartySetup();

 private:
  PlayerEx* playerex_;
};

}  // namespace genshicraft

#endif  // GENSHICRAFT_MENU_H_