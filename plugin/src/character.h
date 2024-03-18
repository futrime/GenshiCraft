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
 * @file character.h
 * @author Futrime (futrime@outlook.com)
 * @brief Declaration of the Character class
 * @version 1.0.0
 * @date 2022-08-08
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#ifndef GENSHICRAFT_CHARACTER_H_
#define GENSHICRAFT_CHARACTER_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "damage.h"
#include "modifier.h"
#include "stats.h"

namespace genshicraft {

class PlayerEx;

/**
 * @brief The Character class contains interfaces for characters.
 *
 */
class Character {
 public:
  static const int kAcensionPhaseMaxLevelList[7];  // the maximum level of each
                                                   // acension phase

  Character() = delete;

  /**
   * @brief Add a modifier
   *
   * @param modifier The modifier
   */
  void AddModifier(Modifier modifier);

  /**
   * @brief Get the ascension materials
   *
   * @return The names and the numbers of the ascension materials
   */
  virtual std::map<std::string, int> GetAscensionMaterials() const = 0;

  /**
   * @brief Get the ascension phase
   *
   * @return The ascension phase (0 <= x <= 6)
   */
  int GetAscensionPhase() const;

  /**
   * @brief Get the base stats
   *
   * @return The stats
   */
  virtual Stats GetBaseStats() const = 0;

  /**
   * @brief Get the CD remaining of elemental burst
   *
   * @return The CD remaing
   */
  double GetCDElementalBurst() const;

  /**
   * @brief Get the CD remaining of elemental burst
   *
   * @return The CD remaing
   */
  virtual double GetCDElementalBurstMax() const = 0;

  /**
   * @brief Get the CD remaining of elemental skill
   *
   * @return The CD remaining
   */
  double GetCDElementalSkill() const;

  /**
   * @brief Get the CD of elemental skill
   *
   * @return The CD
   */
  virtual double GetCDElementalSkillMax() const = 0;

  /**
   * @brief Get the character EXP
   *
   * @return The character EXP (x >= 0)
   */
  int GetCharacterEXP() const;

  /**
   * @brief Get the constellation
   *
   * @return The constellation (0 <= x <= 6)
   */
  int GetConstellation() const;

  /**
   * @brief Get the Damage object of elemental burst
   *
   * @return The Damage object
   */
  virtual Damage GetDamageElementalBurst() = 0;

  /**
   * @brief Get the Damage object of elemental skill
   *
   * @return The Damage object
   */
  virtual Damage GetDamageElementalSkill() = 0;

  /**
   * @brief Get the Damage object of normal attack
   *
   * @return The Damage object
   */
  virtual Damage GetDamageNormalAttack() = 0;

  /**
   * @brief Get the energy
   *
   * @return The energy
   */
  int GetEnergy() const;

  /**
   * @brief Get the max energy
   *
   * @return The max energy
   */
  virtual int GetEnergyMax() const = 0;

  /**
   * @brief Get the fullness
   *
   * @return The fullness
   */
  double GetFullness() const;

  /**
   * @brief Get the HP
   *
   * @return The HP (x >= 0)
   */
  int GetHP() const;

  /**
   * @brief Get the level (1 <= x <= 90)
   *
   * @return The level
   */
  int GetLevel() const;

  /**
   * @brief Get the level with the character EXP
   *
   * @param character_exp The character EXP
   * @return The level
   */
  int GetLevelByCharacterEXP(int character_exp) const;

  /**
   * @brief Get the name
   *
   * @return The name
   */
  virtual std::string GetName() const = 0;

  /**
   * @brief Get the PlayerEx object of the owner
   *
   * @return The PlayerEx object
   */
  PlayerEx* GetPlayerEx() const;

  /**
   * @brief Get the rarity
   *
   * @return The rarity (4 <= x <= 5)
   */
  virtual int GetRarity() const = 0;

  /**
   * @brief Get the stats
   *
   * @return The stats
   */
  Stats GetStats() const;

  /**
   * @brief Describe the stats
   *
   * @param verbose True if get verbose description
   * @return The description
   */
  std::vector<std::string> GetStatsDescription(bool verbose = false) const;

  /**
   * @brief Get the talent elemental burst level
   *
   * @return The level (1 <= x <= 13)
   */
  int GetTalentElementalBurstLevel() const;

  /**
   * @brief Get the talent elemental skill level
   *
   * @return The level (1 <= x <= 13)
   */
  int GetTalentElementalSkillLevel() const;

  /**
   * @brief Get the talent normal attack level
   *
   * @return The level (1 <= x <= 11)
   */
  int GetTalentNormalAttackLevel() const;

  /**
   * @brief Check if the character is holding a weapon
   *
   * @return True if the character is holding a weapon
   */
  virtual bool HasWeapon() const = 0;

  /**
   * @brief Increase 1 ascension phase till 6. If not time to ascend, it will
   * not take effect.
   *
   */
  void IncreaseAscensionPhase();

  /**
   * @brief Increase the character EXP
   *
   * @param value The value to increase. Negative value will not take effect.
   */
  void IncreaseCharacterEXP(int value);

  /**
   * @brief Increase 1 constellation till 6
   *
   */
  void IncreaseConstellation();

  /**
   * @brief Increase energy
   *
   * @param value The value to increase. Negative value for decreasing
   */
  void IncreaseEnergy(int value);

  /**
   * @brief Increase fullness
   *
   * @param value The value to increase. Negative value for decreasing
   */
  void IncreaseFullness(double value);

  /**
   * @brief Increase the HP. If dead, the HP would not increase
   *
   * @param value The value to increase. Negative value for decreasing
   */
  void IncreaseHP(int value);

  /**
   * @brief Check if the character is dead
   *
   * @return True if dead
   */
  bool IsDead() const;

  /**
   * @brief Check if the energy is full
   *
   * @return True if full
   */
  bool IsEnergyFull() const;

  /**
   * @brief Refresh the character
   *
   */
  void Refresh();

  /**
   * @brief Remove a modifier
   *
   * @param id The ID of the modifier
   */
  void RemoveModifier(int id);

  /**
   * @brief Revive
   *
   */
  void Revive();

  /**
   * @brief Make a Character object
   *
   * @param playerex A pointer to the PlayerEx object owning the character
   * @param name The name of the character
   * @param ascension_phase The ascention phase (0 <= x <= 6)
   * @param character_EXP The character EXP (x >= 0)
   * @param constellation The constellation (0 <= x <= 6)
   * @param energy The energy (x >= 0)
   * @param HP The HP (x >= 0)
   * @param talent_elemental_burst_level The level of elemental burst (1 <= x <=
   * 10)
   * @param talent_elemental_skill_level The level of elemental skill (1 <= x <=
   * 10)
   * @param talent_normal_attakck_level The level of normal attack (1 <= X <=
   * 10)
   * @return A pointer to the character object
   *
   * @exception ExceptionInvalidCharacterData The character data is invalid.
   */
  static std::shared_ptr<Character> Make(
      PlayerEx* playerex, const std::string& name, int ascension_phase = 0,
      int character_EXP = 0, int constellation = 0, int energy = 0, int HP = 0,
      int talent_elemental_burst_level = 1,
      int talent_elemental_skill_level = 1, int talent_normal_attack_level = 1);

 protected:
  /**
   * @brief Construct a new Character object
   *
   * @param playerex The PlayerEx object of the owner
   * @param ascension_phase The ascention phase (0 <= x <= 6)
   * @param character_EXP The character EXP (x >= 0)
   * @param constellation The constellation (0 <= x <= 6)
   * @param energy The energy (x >= 0)
   * @param HP The HP (x >= 0)
   * @param talent_elemental_burst_level The level of elemental burst (1 <= x <=
   * 10)
   * @param talent_elemental_skill_level The level of elemental skill (1 <= x <=
   * 10)
   * @param talent_normal_attakck_level The level of normal attack (1 <= X <=
   * 10)
   *
   * @exception ExceptionInvalidCharacterData The character data is invalid.
   */
  Character(PlayerEx* playerex, int ascension_phase, int character_EXP,
            int constellation, int energy, int HP,
            int talent_elemental_burst_level, int talent_elemental_skill_level,
            int talent_normal_attack_level);

  double last_elemental_burst_clock_;  // the clock of last elemental burst
  double last_elemental_skill_clock_;  // the clock of last elemental skill

 private:
  static const int
      kLevelMinCharacterEXPList[91];  // the minimum character EXP of each level

  int ascension_phase_;
  int character_EXP_;
  int constellation_;
  int energy_;
  double fullness_;
  int HP_;
  std::vector<Modifier> modifier_list_;  // the modifiers
  PlayerEx* playerex_;
  int talent_elemental_burst_level_;
  int talent_elemental_skill_level_;
  int talent_normal_attack_level_;
};

}  // namespace genshicraft

#endif  // GENSHICRAFT_CHARACTER_H_