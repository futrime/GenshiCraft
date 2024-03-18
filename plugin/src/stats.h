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
 * @file stats.h
 * @author Futrime (futrime@outlook.com)
 * @brief Declaration of the Stats class
 * @version 1.0.0
 * @date 2022-08-16
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#ifndef GENSHICRAFT_STATS_H_
#define GENSHICRAFT_STATS_H_

namespace genshicraft {

class Stats {
 public:
  /**
   * @brief Get the ATK
   *
   * @return The ATK
   */
  int GetATK() const;

  /**
   * @brief Get the DEF
   *
   * @return The DEF
   */
  int GetDEF() const;

  /**
   * @brief Get the max HP
   *
   * @return The max HP
   */
  int GetMaxHP() const;

  Stats operator+(const Stats& other);

  Stats operator+=(const Stats& other);

  Stats operator-(const Stats& other);

  Stats operator-=(const Stats& other);

  // Base stats
  int max_HP_base = 0;
  int max_HP_ext = 0;
  double max_HP_percent = 0.;
  int ATK_base = 0;
  int ATK_ext = 0;
  double ATK_percent = 0.;
  int DEF_base = 0;
  int DEF_ext = 0;
  double DEF_percent = 0.;
  int elemental_mastery = 0;
  int max_stamina = 0;

  // Advanced stats
  double CRIT_rate = 0.;
  double CRIT_DMG = 0.;
  double healing_bonus = 0.;
  double incoming_healing_bonus = 0.;
  double energy_recharge = 0.;
  double CD_reduction = 0.;
  double shield_strength = 0.;

  // Elemental type
  double pyro_DMG_bonus = 0.;
  double pyro_RES = 0.;
  double hydro_DMG_bonus = 0.;
  double hydro_RES = 0.;
  double dendro_DMG_bonus = 0.;
  double dendro_RES = 0.;
  double electro_DMG_bonus = 0.;
  double electro_RES = 0.;
  double anemo_DMG_bonus = 0.;
  double anemo_RES = 0.;
  double cryo_DMG_bonus = 0.;
  double cryo_RES = 0.;
  double geo_DMG_bonus = 0.;
  double geo_RES = 0.;
  double physical_DMG_bonus = 0.;
  double physical_RES = 0.;
};

}  // namespace genshicraft

#endif  // GENSHICRAFT_STATS_H_