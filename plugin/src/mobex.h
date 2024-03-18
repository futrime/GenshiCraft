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
 * @file mobex.h
 * @author Futrime (futrime@outlook.com)
 * @brief Declaration of the MobEx class
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#ifndef GENSHICRAFT_MOBEX_H_
#define GENSHICRAFT_MOBEX_H_

#include <MC/Mob.hpp>
#include <memory>
#include <third-party/Nlohmann/json.hpp>

#include "actorex.h"
#include "damage.h"
#include "stats.h"

namespace genshicraft {

/**
 * @brief The MobEx class contains extended interfaces for the Mob class.
 *
 */
class MobEx : public ActorEx {
 public:
  MobEx() = delete;

  /**
   * @brief Construct a new MobEx object from a Mob object
   *
   * @param mob A pointer to the Mob object
   *
   * @note The method ActorEx::LoadData() should be called right after
   * construction.
   */
  MobEx(Mob* mob);

  /**
   * @brief Destroy the MobEx object
   *
   */
  virtual ~MobEx();

  /**
   * @brief Apply damage to the mob
   *
   * @param damage The damage
   */
  virtual void ApplyDamage(const Damage& damage);

  /**
   * @brief Get the HP
   *
   * @return The HP
   */
  virtual int GetHP() const;

  /**
   * @brief Get the Damage object of the last damage
   *
   * @return The Damage object
   */
  virtual Damage GetLastDamage() const;

  /**
   * @brief Get the native health last time processed
   *
   * @return The native health
   */
  virtual int GetLastNativeHealth() const;

  /**
   * @brief Get the Mob object
   *
   * @return The Mob object
   *
   * @note This function will return nullptr if failed to get the object.
   */
  virtual Mob* GetMob() const;

  /**
   * @brief Increase the HP
   *
   * @param value The HP increment. Negative value means reduction.
   */
  virtual void IncreaseHP(int value);

  /**
   * @brief Check if the mob is a mob
   *
   * @return True
   */
  virtual bool IsMob() const override;

  /**
   * @brief Set the native health last time processed
   *
   * @param health the native health
   */
  virtual void SetLastNativeHealth(int health);

  /**
   * @brief Get a MobEx object by a unique ID
   *
   * @param unique_id The unique ID
   * @return A pointer to the MobEx object
   *
   * @note This method will return an empty pointer if failed to get.
   */
  static std::shared_ptr<MobEx> Get(long long unique_id);

  /**
   * @brief Make a MobEx object from a Mob object
   *
   * @param mob A pointer to the Mob object
   * @return A pointer to the MobEx object
   *
   * @note This method will return an empty pointer if failed to make.
   */
  static std::shared_ptr<MobEx> Make(Mob* mob);

 protected:
  int HP_;
  Damage latest_damage_;
  int last_native_health_;

 private:
  inline static const int kMobExDataFormatVersion = 1;

  /**
   * @brief Load the data
   *
   * @note This method should only be called in constructor.
   */
  virtual void LoadData() override;

  /**
   * @brief Save the data
   *
   * @note This method should only be called in desctructor.
   */
  virtual void SaveData() override;

  /**
   * @brief Migrate the data with previous data format version to current
   * version.
   *
   * @param old_data The data to migrate
   * @return The data with current data format version
   *
   * @note If the data is invalid, this method will return an empty JSON object.
   */
  static nlohmann::json MigrateData(const nlohmann::json& old_data);
};

}  // namespace genshicraft

#endif  // GENSHICRAFT_MOBEX_H_