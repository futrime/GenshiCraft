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
 * @file sidebar.h
 * @author Futrime (futrime@outlook.com)
 * @brief Declaration of the Sidebar class
 * @version 1.0.0
 * @date 2022-08-08
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#ifndef GENSHICRAFT_SIDEBAR_H_
#define GENSHICRAFT_SIDEBAR_H_

#include <string>
#include <memory>

namespace genshicraft {

class PlayerEx;

/**
 * @brief The Sidebar class provide sidebar features to players.
 *
 */
class Sidebar {
 public:
  /**
   * @brief Construct a new Sidebar object
   *
   * @param playerex The PlayerEx the Sidebar object belonging to
   */
  explicit Sidebar(PlayerEx* playerex);

  Sidebar() = delete;

  /**
   * @brief Refresh the sidebar
   *
   */
  void Refresh();

 private:
  /**
   * @brief Generate a text progress bar
   *
   * @param value The value of the progress bar (0 ~ 1)
   * @param steps The steps of the progress bar (eight steps per chraracter)
   * @param format_code The formatting codes
   * @return The text progress bar
   */
  static std::string GenerateProgressBar(double value, int steps = 10,
                                         std::string formatting_codes = "");

  PlayerEx* playerex_;
};

}  // namespace genshicraft

#endif  // GENSHICRAFT_SIDEBAR_H_