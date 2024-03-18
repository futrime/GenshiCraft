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
 * @file modifier.h
 * @author Futrime (futrime@outlook.com)
 * @brief Declaration of the Modifier class
 * @version 1.0.0
 * @date 2022-08-16
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#ifndef GENSHICRAFT_MODIFIER_H_
#define GENSHICRAFT_MODIFIER_H_

#include "stats.h"

namespace genshicraft {

/**
 * @brief The Modifier class represents the modification to character attributes
 * and effects.
 *
 */
class Modifier {
 public:
  /**
   * @brief The modifier type
   *
   */
  enum class Type { kStats = 0 };

  Modifier() = delete;

  /**
   * @brief Construct a new stats Modifier object
   *
   * @param stats The stats
   * @param expired_clock The expired clock
   */
  Modifier(Stats stats, double expired_clock);

  /**
   * @brief Get the expired clock
   *
   * @return The expired clock
   */
  double GetExpiredClock() const;

  /**
   * @brief Get the the modifier ID
   *
   * @return The modifier ID
   */
  int GetID() const;

  /**
   * @brief Get the base stats
   *
   * @return The stats
   * @exception ExceptionIncorrectModifierType The modifier is not a stats
   * modifier.
   */
  Stats GetBaseStats() const;

  /**
   * @brief Get the type of the modifier
   *
   * @return The type
   */
  Type GetType() const;

  /**
   * @brief Generate an modifier ID
   *
   * @return The ID
   */
  static int GenerateID();

 private:
  int id_;                  // the ID
  double expired_clock_;    // the clock when the modifier expired
  Stats stats_;  // the stats
  Type type_;

  static int latest_id_;  // the latest used ID
};

}  // namespace genshicraft

#endif  // GENSHICRAFT_MODIFIER_H_