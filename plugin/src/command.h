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
 * @file command.h
 * @author Futrime (futrime@outlook.com)
 * @brief Declaration of the Command class
 * @version 1.0.0
 * @date 2022-07-22
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#ifndef GENSHICRAFT_COMMAND_H_
#define GENSHICRAFT_COMMAND_H_

namespace genshicraft {

/**
 * @brief The Command class contains commands for GenshiCraft.
 *
 */
class Command {
 public:
  /**
   * @brief Initialize commands
   * 
   */
  static void Init();
};

}  // namespace genshicraft

#endif  // GENSHICRAFT_COMMAND_H_