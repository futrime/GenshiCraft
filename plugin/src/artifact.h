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
 * @file artifact.h
 * @author Futrime (futrime@outlook.com)
 * @brief Declaration of the Artifact class
 * @version 1.0.0
 * @date 2022-08-17
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#ifndef GENSHICRAFT_ARTIFACT_H_
#define GENSHICRAFT_ARTIFACT_H_

#include <MC/ItemStack.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "stats.h"

namespace genshicraft {

class PlayerEx;

/**
 * @brief The Artifact class contains interfaces for artifacts.
 *
 */
class Artifact {
 public:
  /**
   * @brief The possible types of artifact stats
   *
   */
  enum class StatType {
    kHP = 0,
    kHPPercent,
    kATK,
    kATKPercent,
    kDEF,
    kDEFPercent,
    kElementalMastery,
    kCritRate,
    kCritDMG,
    kHealingBonus,
    kEnergyRecharge,
    kPyroDMG,
    kHydroDMG,
    kDendroDMG,
    kElectroDMG,
    kAnemoDMG,
    kCryoDMG,
    kGeoDMG,
    kPhysicalDMG
  };

  /**
   * @brief The types of artifacts
   *
   */
  enum class Type {
    kFlowerOfLife = 0,
    kPlumeOfDeath,
    kSandsOfEon,
    kGobletOfEonothem,
    kCircletOfLogos
  };

  /**
   * @brief The ArtifactInfo struct contains artifact information.
   *
   */
  struct ArtifactInfo {
    std::string name;
    std::string set_name;
    Type type;
  };

  /**
   * @brief The StatItem struct represents a stat item.
   *
   */
  struct StatItem {
    double value;
    StatType type;
  };

  Artifact() = delete;

  /**
   * @brief Construct a new Artifact object
   *
   * @param item The artifact item
   * @param playerex The PlayerEx object of the player of the holder
   *
   * @exception ExceptionNotAnArtifact The item is not a GenshiCraft artifact
   */
  Artifact(ItemStack *item, PlayerEx *playerex);

  /**
   * @brief Destroy the Artifact object
   *
   */
  ~Artifact();

  /**
   * @brief Apply the lore
   *
   */
  void ApplyLore();

  /**
   * @brief Get the artifact EXP
   *
   * @return The artifact EXP
   */
  int GetArtifactEXP() const;

  /**
   * @brief Get the base artifact EXP as artifact EXP material
   *
   * @return The base artifact EXP
   */
  int GetBaseConsumableEXP() const;

  /**
   * @brief Get the base stats
   *
   * @param stats The stats
   */
  Stats GetBaseStats() const;

  /**
   * @brief Describe the base stats
   *
   * @return The description. The first line is the main stat.
   */
  std::vector<std::string> GetBaseStatsDescription() const;

  /**
   * @brief Get the level
   *
   * @return The level of the artifact (0 <= x <= 4 for 1-Star and 2-Star
   * artifacts, 0 <= x <= 12 for 3-Star artifacts, 0 <= x <= 16 for 4-Star
   * artifacts, 0 <= x <= 20 for 5-Star artifacts)
   */
  int GetLevel() const;

  /**
   * @brief Predict the level of the artifact under the artifact EXP
   *
   * @param artifact_exp The artifact EXP
   * @return The level
   */
  int GetLevelByArtifactEXP(int artifact_exp) const;

  /**
   * @brief Get the max level
   *
   * @return The max level (4 for 1-Star and 2-Star artifacts, 12 for 3-Star
   * artifacts, 16 for 4-Star artifacts, 20 for 5-Star artifacts)
   */
  int GetLevelMax() const;

  /**
   * @brief Get the name
   *
   * @return The name
   */
  std::string GetName() const;

  /**
   * @brief Get the rarity of the artifact
   *
   * @return The rarity of the artifact (1 ~ 5)
   */
  int GetRarity() const;

  /**
   * @brief Get the artifact set name
   *
   * @return The artifact set name
   */
  std::string GetSetName() const;

  /**
   * @brief Get the artifact type
   *
   * @return The artifact type
   */
  Type GetType() const;

  /**
   * @brief Increase the artifact EXP
   *
   * @param value The value to increase. Negative value will not take effect.
   */
  void IncreaseArtifactEXP(int value);

  /**
   * @brief Check if the item is a GenshiCraft artifact
   *
   * @param item The item
   * @return True if the item is a GenshiCraft artifact
   */
  static bool CheckIsArtifact(ItemStack *item);

  /**
   * @brief Get the number of artifacts of the set equipped by the player
   *
   * @param set_name The set name
   * @param playerex The PlayerEx object of the player
   * @return The number
   */
  static int GetSetCount(const std::string &set_name, PlayerEx *playerex);

  /**
   * @brief Get the set effect descriptions
   *
   * @return A list whose first element is the description of the 2-piece set
   * and the second is the description of the 4-piece set
   */
  static std::vector<std::string> GetSetEffectDescription(
      const std::string &set_name);

  /**
   * @brief Make an Artifact object
   *
   * @param item The artifact item
   * @param playerex A pointer to the PlayerEx object of the holder
   * @return A pointer to the artifact object
   *
   * @exception ExceptionNotAnArtifact The item is not a GenshiCraft artifact
   */
  static std::shared_ptr<Artifact> Make(ItemStack *item, PlayerEx *playerex);

 private:
  /**
   * @brief Initialize the stats
   *
   */
  void InitStats();

  const static int
      kRarityBaseConsumableEXPList[6];  // the base artifact EXP of artifacts
                                        // with different rarities

  const static std::map<std::string, ArtifactInfo>
      kArtifactInfoDict;  // information of all artifacts

  const static int
      kLevelMinArtifactEXPList[6]
                              [21];  // [A][B] means the minimum artifact EXP
                                     // for A-Star artifacts to reach level B.

  const static std::map<StatType, double>
      kMainStatBase[6];  // [A][B] means the main stat of level 0 for
                         // stat B of A-Star artifacts.

  const static std::map<StatType, double>
      kMainStatDiff[6];  // [A][B] means the main stat difference for
                         // stat B of A-Star artifacts.

  const static std::map<StatType, std::vector<double>>
      kPossibleSubStatDiff[6];  // [A][B] means a list of the possible B-stat
                                // unit substat difference of A-star artifacts.

  const static int
      kRarityMaxLevelList[6];  // the maximum level of different rarities

  const static std::map<std::string, std::vector<std::string>>
      kSetEffectDescriptionDict;  // the set effect descriptions

  int artifact_exp_;
  std::string identifier_;
  ItemStack *item_;
  StatItem main_stat_;
  std::vector<StatItem> sub_stat_list_;
  PlayerEx *playerex_;  // the PlayerEx object of the owner
  int rarity_;
};

}  // namespace genshicraft

#endif  // GENSHICRAFT_ARTIFACT_H_