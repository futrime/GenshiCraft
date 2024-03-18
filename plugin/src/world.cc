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
 * @file world.cc
 * @author Futrime (futrime@outlook.com)
 * @brief Definition of global world interfaces and objects
 * @version 1.0.0
 * @date 2022-08-11
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#include "world.h"

#include <MC/Actor.hpp>
#include <MC/ActorDamageSource.hpp>
#include <MC/Dimension.hpp>
#include <MC/Types.hpp>
#include <MC/Vec3.hpp>
#include <cmath>

namespace genshicraft {

namespace world {

double Get2DDistance(const Vec3& position_1, const Vec3& position_2) {
  return std::sqrt(std::pow(position_1.x - position_2.x, 2) +
                   std::pow(position_1.z - position_2.z, 2));
}

int GetWorldLevel(const Vec3& position, const Dimension& dimension) {
  static const double kWorldLevelMinDistanceList[9] = {
      0, 0, 1024, 2048, 4096, 8192, 16384, 32768, 65536};

  if (dimension.getDimensionId() == 2) {  // the End
    return 9;
  }

  auto distance = Get2DDistance(position, Vec3(0., 0., 0.));

  if (dimension.getDimensionId() == 1) {  // the Nether
    distance *=
        8;  // the distance in the Nether is 8 times of that in the Main World
  }

  int world_level = 1;
  for (int i = 1; i <= 8; ++i) {
    if (distance > kWorldLevelMinDistanceList[i]) {
      world_level = i;
    }
  }

  return world_level;
}

double GetEnemyMaxHPMultiplier(int level) {
  return (72.4317502144258 + 0.272784395235931 * level +
          0.261414242992725 * level * level +
          0.0340511473803347 * level * level * level) /
         73;
}

double GetEnemyATKMultiplier(int level) {
  return (25.9169123951706 - 1.19203469178426 * level +
          0.275319202076914 * level * level -
          0.00019690546398588 * level * level * level) /
         25;
}

void HurtActor(Actor* actor, float damage,
               ActorDamageCause cause,
               bool knockback, bool ignite) {
  char source[16];
  (*(ActorDamageSource*)source).ActorDamageSource::ActorDamageSource(cause);
  actor->_hurt((*(ActorDamageSource*)source), damage, knockback, ignite);
  (*(ActorDamageSource*)source).~ActorDamageSource();
}

}  // namespace world

}  // namespace genshicraft