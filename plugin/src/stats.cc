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
 * @file stats.cc
 * @author Futrime (futrime@outlook.com)
 * @brief Definition of the Stats class
 * @version 1.0.0
 * @date 2022-08-16
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#include "stats.h"

namespace genshicraft {

int Stats::GetATK() const {
  return static_cast<int>(this->ATK_base * (1 + this->ATK_percent) +
                          this->ATK_ext);
}

int Stats::GetDEF() const {
  return static_cast<int>(this->DEF_base * (1 + this->DEF_percent) +
                          this->DEF_ext);
}

int Stats::GetMaxHP() const {
  return static_cast<int>(this->max_HP_base * (1 + this->max_HP_percent) +
                          this->max_HP_ext);
}

Stats Stats::operator+(const Stats& other) {
  Stats stats;

  stats.max_HP_base = this->max_HP_base + other.max_HP_base;
  stats.max_HP_percent = this->max_HP_percent + other.max_HP_percent;
  stats.max_HP_ext = this->max_HP_ext + other.max_HP_ext;
  stats.ATK_base = this->ATK_base + other.ATK_base;
  stats.ATK_percent = this->ATK_percent + other.ATK_percent;
  stats.ATK_ext = this->ATK_ext = other.ATK_ext;
  stats.DEF_base = this->DEF_base + other.DEF_base;
  stats.DEF_percent = this->DEF_percent + other.DEF_percent;
  stats.DEF_ext = this->DEF_ext + other.DEF_ext;
  stats.max_stamina = this->max_stamina + other.max_stamina;

  stats.CRIT_rate = this->CRIT_rate + other.CRIT_rate;
  stats.CRIT_DMG = this->CRIT_DMG + other.CRIT_DMG;
  stats.healing_bonus = this->healing_bonus + other.healing_bonus;
  stats.incoming_healing_bonus =
      this->incoming_healing_bonus + other.incoming_healing_bonus;
  stats.energy_recharge = this->energy_recharge + other.energy_recharge;
  stats.CD_reduction = this->CD_reduction + other.CD_reduction;
  stats.shield_strength = this->shield_strength + other.shield_strength;

  stats.pyro_DMG_bonus = this->pyro_DMG_bonus + other.pyro_DMG_bonus;
  stats.pyro_RES = this->pyro_RES + other.pyro_RES;
  stats.hydro_DMG_bonus = this->hydro_DMG_bonus + other.hydro_DMG_bonus;
  stats.hydro_RES = this->hydro_RES + other.hydro_RES;
  stats.dendro_DMG_bonus = this->dendro_DMG_bonus + other.dendro_DMG_bonus;
  stats.dendro_RES = this->dendro_RES + other.dendro_RES;
  stats.electro_DMG_bonus = this->electro_DMG_bonus + other.electro_DMG_bonus;
  stats.electro_RES = this->electro_RES + other.electro_RES;
  stats.anemo_DMG_bonus = this->anemo_DMG_bonus + other.anemo_DMG_bonus;
  stats.anemo_RES = this->anemo_RES + other.anemo_RES;
  stats.cryo_DMG_bonus = this->cryo_DMG_bonus + other.cryo_DMG_bonus;
  stats.cryo_RES = this->cryo_RES + other.cryo_RES;
  stats.geo_DMG_bonus = this->geo_DMG_bonus + other.geo_DMG_bonus;
  stats.geo_RES = this->geo_RES + other.geo_RES;
  stats.physical_DMG_bonus =
      this->physical_DMG_bonus + other.physical_DMG_bonus;
  stats.physical_RES = this->physical_RES + other.physical_RES;

  return stats;
}

Stats Stats::operator+=(const Stats& other) {
  *this = *this + other;
  return *this;
}

Stats Stats::operator-(const Stats& other) {
  Stats stats;

  stats.max_HP_base = this->max_HP_base - other.max_HP_base;
  stats.max_HP_percent = this->max_HP_percent - other.max_HP_percent;
  stats.max_HP_ext = this->max_HP_ext - other.max_HP_ext;
  stats.ATK_base = this->ATK_base - other.ATK_base;
  stats.ATK_percent = this->ATK_percent - other.ATK_percent;
  stats.ATK_ext = this->ATK_ext = other.ATK_ext;
  stats.DEF_base = this->DEF_base - other.DEF_base;
  stats.DEF_percent = this->DEF_percent - other.DEF_percent;
  stats.DEF_ext = this->DEF_ext - other.DEF_ext;
  stats.max_stamina = this->max_stamina - other.max_stamina;

  stats.CRIT_rate = this->CRIT_rate - other.CRIT_rate;
  stats.CRIT_DMG = this->CRIT_DMG - other.CRIT_DMG;
  stats.healing_bonus = this->healing_bonus - other.healing_bonus;
  stats.incoming_healing_bonus =
      this->incoming_healing_bonus - other.incoming_healing_bonus;
  stats.energy_recharge = this->energy_recharge - other.energy_recharge;
  stats.CD_reduction = this->CD_reduction - other.CD_reduction;
  stats.shield_strength = this->shield_strength - other.shield_strength;

  stats.pyro_DMG_bonus = this->pyro_DMG_bonus - other.pyro_DMG_bonus;
  stats.pyro_RES = this->pyro_RES - other.pyro_RES;
  stats.hydro_DMG_bonus = this->hydro_DMG_bonus - other.hydro_DMG_bonus;
  stats.hydro_RES = this->hydro_RES - other.hydro_RES;
  stats.dendro_DMG_bonus = this->dendro_DMG_bonus - other.dendro_DMG_bonus;
  stats.dendro_RES = this->dendro_RES - other.dendro_RES;
  stats.electro_DMG_bonus = this->electro_DMG_bonus - other.electro_DMG_bonus;
  stats.electro_RES = this->electro_RES - other.electro_RES;
  stats.anemo_DMG_bonus = this->anemo_DMG_bonus - other.anemo_DMG_bonus;
  stats.anemo_RES = this->anemo_RES - other.anemo_RES;
  stats.cryo_DMG_bonus = this->cryo_DMG_bonus - other.cryo_DMG_bonus;
  stats.cryo_RES = this->cryo_RES - other.cryo_RES;
  stats.geo_DMG_bonus = this->geo_DMG_bonus - other.geo_DMG_bonus;
  stats.geo_RES = this->geo_RES - other.geo_RES;
  stats.physical_DMG_bonus =
      this->physical_DMG_bonus - other.physical_DMG_bonus;
  stats.physical_RES = this->physical_RES - other.physical_RES;

  return stats;
}

Stats Stats::operator-=(const Stats& other) {
  *this = *this - other;
  return *this;
}

}  // namespace genshicraft