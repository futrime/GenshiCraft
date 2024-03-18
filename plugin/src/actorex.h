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
 * @file actorex.h
 * @author Futrime (futrime@outlook.com)
 * @brief Declaration of the ActorEx class
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#ifndef GENSHICRAFT_ACTOREX_H_
#define GENSHICRAFT_ACTOREX_H_

#include <MC/Actor.hpp>
#include <memory>
#include <third-party/Nlohmann/json.hpp>

#include "damage.h"

namespace genshicraft {

/**
 * @brief The ActorEx class contains extended interfaces for the Actor class.
 *
 */
class ActorEx {
 public:
  ActorEx() = delete;

  /**
   * @brief Construct a new ActorEx object from an Actor object
   *
   * @param actor A pointer to the Actor object
   *
   * @note The method ActorEx::LoadData() should be called right after
   * construction.
   */
  ActorEx(Actor* actor);

  /**
   * @brief Destroy the ActorEx object
   *
   */
  virtual ~ActorEx();

  /**
   * @brief Get the Actor object
   *
   * @return The Actor object
   *
   * @note This function will return nullptr if failed to get the object.
   */
  virtual Actor* GetActor() const;

  /**
   * @brief Get the attack damage
   *
   * @return The Damage object
   */
  virtual Damage GetAttackDamage() const;

  /**
   * @brief Get the level
   *
   * @return The level
   */
  virtual int GetLevel() const;

  /**
   * @brief Get the stats
   *
   * @return The stats
   */
  virtual Stats GetStats() const;

  /**
   * @brief Get the unique ID
   *
   * @return The unique ID
   */
  virtual long long GetUniqueID() const;

  /**
   * @brief Check if the actor is a mob
   *
   * @return True if the actor is a mob
   */
  virtual bool IsMob() const;

  /**
   * @brief Check if the actor is a player
   *
   * @return True if the actor is a player
   */
  virtual bool IsPlayer() const;

  /**
   * @brief Set the ATK of the mob by native damage value
   *
   * @param native_damage The native damage
   */
  virtual void SetATKByNativeDamage(double native_damage);

  /**
   * @brief Get an ActorEx object by a unique ID
   *
   * @param unique_id The unique ID
   * @return A pointer to the ActorEx object
   *
   * @note This function will return an empty pointer if failed to get.
   */
  static std::shared_ptr<ActorEx> Get(long long unique_id);

  /**
   * @brief Make a ActorEx object from a Actor object
   *
   * @param actor A pointer to the Actor object
   * @return A pointer to the ActorEx object
   *
   * @note This method will return an empty pointer if failed to make.
   */
  static std::shared_ptr<ActorEx> Make(Actor* actor);

 protected:
  bool is_data_saved_ =
      false;  // this flag is used to prevent duplicated data save.

  int level_;
  Stats stats_;
  long long unique_id_;  // the unique ID

 private:
  inline static const int kActorExDataFormatVersion = 1;

  /**
   * @brief Load the data
   *
   * @note This method should only be called in constructor.
   */
  virtual void LoadData();

  /**
   * @brief Save the data
   *
   * @note This method should only be called in desctructor.
   */
  virtual void SaveData();

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

#endif  // GENSHICRAFT_ACTOREX_H_