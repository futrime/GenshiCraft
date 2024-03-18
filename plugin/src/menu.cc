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
 * @file menu.cc
 * @author Futrime (futrime@outlook.com)
 * @brief Definition of the Menu class
 * @version 1.0.0
 * @date 2022-07-22
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#include "menu.h"

#include <FormUI.h>
#include <ScheduleAPI.h>

#include <MC/ItemStack.hpp>
#include <MC/Player.hpp>
#include <MC/SimpleContainer.hpp>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "artifact.h"
#include "character.h"
#include "playerex.h"
#include "plugin.h"
#include "weapon.h"

namespace genshicraft {

Menu::Menu(PlayerEx* playerex) : playerex_(playerex) {
  // Empty
}

void Menu::OpenCharacter() {
  auto character = this->playerex_->GetCharacter();

  std::string content;

  content += "§f";
  for (int i = 0; i < 6; ++i) {
    if (i == character->GetAscensionPhase()) {
      content += "§7";
    }
    content += "✦";
  }
  content += "\n";

  content +=
      "§fLevel " + std::to_string(character->GetLevel()) + " §7/ " +
      std::to_string(
          Character::kAcensionPhaseMaxLevelList[character
                                                    ->GetAscensionPhase()]) +
      "\n";

  content += "§f";
  for (const auto& line : character->GetStatsDescription(false)) {
    content += "§f" + line + "\n";
  }

  Form::SimpleForm form("Character / " + character->GetName(), content);

  form = form.addButton(
      "Details", "", [this](Player* player) { this->OpenCharacterDetails(); });

  if (Character::kAcensionPhaseMaxLevelList[character->GetAscensionPhase()] ==
      character->GetLevel()) {
    if (character->GetLevel() != 90) {
      form = form.addButton("Ascend", "", [this](Player* player) {
        this->OpenCharacterAscend();
      });
    }
  } else {
    form = form.addButton("Level Up", "", [this](Player* player) {
      this->OpenCharacterLevelUp();
    });
  }

  // Show the weapon menu only if the player is holding a GenshiCraft weapon
  if (character->HasWeapon()) {
    form = form.addButton(
        "Weapon", "", [this](Player* player) { this->OpenCharacterWeapon(); });
  }

  // Show the artifacts menu only if the player is equipped with at least one
  // GenshiCraft artifact
  if (Artifact::CheckIsArtifact(
          this->playerex_->GetPlayer()->getArmorContainer().getSlot(0)) ||
      Artifact::CheckIsArtifact(
          this->playerex_->GetPlayer()->getArmorContainer().getSlot(1)) ||
      Artifact::CheckIsArtifact(
          this->playerex_->GetPlayer()->getArmorContainer().getSlot(2)) ||
      Artifact::CheckIsArtifact(
          this->playerex_->GetPlayer()->getArmorContainer().getSlot(3)) ||
      Artifact::CheckIsArtifact(const_cast<ItemStack*>(
          &(this->playerex_->GetPlayer()->getOffhandSlot())))) {
    form = form.addButton("Artifacts", "", [this](Player* player) {
      this->OpenCharacterArtifacts();
    });
  }

  // form = form.addButton("Constellation");

  // form = form.addButton("Talents");

  form.sendTo(this->playerex_->GetPlayer(), [this](Player* player, int option) {
    if (option == -1) {
      this->OpenMain();
      return;
    }
  });
}

void Menu::OpenCharacterArtifacts() {
  Form::SimpleForm form("Artifacts", "");

  auto player = this->playerex_->GetPlayer();

  bool has_artifact = false;

  if (Artifact::CheckIsArtifact(player->getArmorContainer().getSlot(0))) {
    form = form.addButton(
        "Flower of Life", "textures/menu/artifacts/flower_of_life.png",
        [this](Player* player) {
          this->OpenCharacterArtifacts(Artifact::Type::kFlowerOfLife);
        });
    has_artifact = true;
  }

  if (Artifact::CheckIsArtifact(player->getArmorContainer().getSlot(1))) {
    form = form.addButton(
        "Plume of Death", "textures/menu/artifacts/plume_of_death.png",
        [this](Player* player) {
          this->OpenCharacterArtifacts(Artifact::Type::kPlumeOfDeath);
        });
    has_artifact = true;
  }

  if (Artifact::CheckIsArtifact(player->getArmorContainer().getSlot(2))) {
    form = form.addButton(
        "Sands of Eon", "textures/menu/artifacts/sands_of_eon.png",
        [this](Player* player) {
          this->OpenCharacterArtifacts(Artifact::Type::kSandsOfEon);
        });
    has_artifact = true;
  }

  if (Artifact::CheckIsArtifact(player->getArmorContainer().getSlot(3))) {
    form = form.addButton(
        "Goblet of Eonothem", "textures/menu/artifacts/goblet_of_eonothem.png",
        [this](Player* player) {
          this->OpenCharacterArtifacts(Artifact::Type::kGobletOfEonothem);
        });
    has_artifact = true;
  }

  if (Artifact::CheckIsArtifact(
          const_cast<ItemStack*>(&(player->getOffhandSlot())))) {
    form = form.addButton(
        "Circlet of Logos", "textures/menu/artifacts/circlet_of_logos.png",
        [this](Player* player) {
          this->OpenCharacterArtifacts(Artifact::Type::kCircletOfLogos);
        });
    has_artifact = true;
  }

  if (!has_artifact) {
    this->OpenCharacter();
    return;
  }

  form.sendTo(this->playerex_->GetPlayer(), [this](Player* player, int option) {
    if (option == -1) {
      this->OpenCharacter();
      return;
    }
  });
}

void Menu::OpenCharacterArtifacts(Artifact::Type type) {
  // Go back if the player is not equipped with this type of artifact
  if ((type == Artifact::Type::kFlowerOfLife &&
       !Artifact::CheckIsArtifact(
           this->playerex_->GetPlayer()->getArmorContainer().getSlot(0))) ||
      (type == Artifact::Type::kPlumeOfDeath &&
       !Artifact::CheckIsArtifact(
           this->playerex_->GetPlayer()->getArmorContainer().getSlot(1))) ||
      (type == Artifact::Type::kSandsOfEon &&
       !Artifact::CheckIsArtifact(
           this->playerex_->GetPlayer()->getArmorContainer().getSlot(2))) ||
      (type == Artifact::Type::kGobletOfEonothem &&
       !Artifact::CheckIsArtifact(
           this->playerex_->GetPlayer()->getArmorContainer().getSlot(3))) ||
      (type == Artifact::Type::kCircletOfLogos &&
       !Artifact::CheckIsArtifact(const_cast<ItemStack*>(
           &(this->playerex_->GetPlayer()->getOffhandSlot()))))) {
    this->OpenCharacterArtifacts();
    return;
  }

  // Construct the artifact object and display the type name
  std::shared_ptr<Artifact> artifact;
  std::string type_str;
  switch (type) {
    case Artifact::Type::kFlowerOfLife:
      artifact = Artifact::Make(
          this->playerex_->GetPlayer()->getArmorContainer().getSlot(0),
          this->playerex_);
      type_str = "§7Flower of Life\n";
      break;

    case Artifact::Type::kPlumeOfDeath:
      artifact = Artifact::Make(
          this->playerex_->GetPlayer()->getArmorContainer().getSlot(1),
          this->playerex_);
      type_str = "§7Plume of Death\n";
      break;

    case Artifact::Type::kSandsOfEon:
      artifact = Artifact::Make(
          this->playerex_->GetPlayer()->getArmorContainer().getSlot(2),
          this->playerex_);
      type_str = "§7Sands of Eon\n";
      break;

    case Artifact::Type::kGobletOfEonothem:
      artifact = Artifact::Make(
          this->playerex_->GetPlayer()->getArmorContainer().getSlot(3),
          this->playerex_);
      type_str = "§7Goblet of Eonothem\n";
      break;

    case Artifact::Type::kCircletOfLogos:
      artifact =
          Artifact::Make(const_cast<ItemStack*>(
                             &(this->playerex_->GetPlayer()->getOffhandSlot())),
                         this->playerex_);
      type_str = "§7Circlet of Logos\n";
      break;

    default:
      break;
  }

  Form::CustomForm form("Artifact / " + artifact->GetName());

  form = form.addLabel("type", type_str);

  // Display the main stat description
  form = form.addLabel("main_stat",
                       "§f" + artifact->GetBaseStatsDescription().at(0));

  // Display the rarity
  std::string rarity_str = "§6";
  for (int i = 0; i < artifact->GetRarity(); ++i) {
    rarity_str += "★";
  }
  form = form.addLabel("rarity", rarity_str);

  // Display the level
  form = form.addLabel("level", "§f+" + std::to_string(artifact->GetLevel()));

  // Display the sub stats descriptions
  for (int i = 1; i < artifact->GetBaseStatsDescription().size(); ++i) {
    form = form.addLabel("sub_stat_" + std::to_string(i - 1),
                         "§f" + artifact->GetBaseStatsDescription().at(i));
  }

  // Display the set name
  form = form.addLabel("set_name",
                       "§a" + artifact->GetSetName() + ": (" +
                           std::to_string(Artifact::GetSetCount(
                               artifact->GetSetName(), this->playerex_)) +
                           ")");

  // Display the set effect descriptions
  form = form.addLabel(
      "set_effect_description_2",
      ((Artifact::GetSetCount(artifact->GetSetName(), this->playerex_) >= 2)
           ? "§a☑"
           : "§7⊘") +
          std::string("2-Piece Set: ") +
          Artifact::GetSetEffectDescription(artifact->GetSetName()).at(0));

  form = form.addLabel(
      "set_effect_description_4",
      ((Artifact::GetSetCount(artifact->GetSetName(), this->playerex_) >= 4)
           ? "§a☑"
           : "§7⊘") +
          std::string("4-Piece Set: ") +
          Artifact::GetSetEffectDescription(artifact->GetSetName()).at(1));

  if (artifact->GetLevel() < artifact->GetLevelMax()) {
    // Calculate the maximum levels to increase
    int artifact_EXP = 0;
    int mora_to_consume = 0;

    for (int i = 0; i < this->playerex_->GetPlayer()->getInventory().getSize();
         ++i) {
      auto item = this->playerex_->GetPlayer()->getInventory().getSlot(i);

      if (!Artifact::CheckIsArtifact(item)) {
        continue;
      };

      auto artifact_to_consume = Artifact::Make(item, this->playerex_);
      if (artifact_to_consume->GetBaseConsumableEXP() + mora_to_consume <=
          this->playerex_->GetMoraCount()) {
        mora_to_consume += artifact_to_consume->GetBaseConsumableEXP();
        artifact_EXP +=
            artifact_to_consume->GetBaseConsumableEXP() +
            static_cast<int>(artifact_to_consume->GetArtifactEXP() * 0.8);
      }
    }

    auto max_level_increment = artifact->GetLevelByArtifactEXP(
                                   artifact->GetArtifactEXP() + artifact_EXP) -
                               artifact->GetLevel();

    if (max_level_increment ==
        0) {  // if the artifact EXP is not enough to inrease at least one level
      form = form.addToggle("is_all_in", "Consume the most artifacts", false);
    } else {
      form = form.addSlider("level", "The levels to increase", 0,
                            max_level_increment);
    }
  }

  form.sendTo(
      this->playerex_->GetPlayer(),
      [this, type](
          Player* player,
          std::map<string, std::shared_ptr<Form::CustomFormElement>> data) {
        if (data.empty()) {  // if the player closed the menu
          this->OpenCharacterArtifacts();
          return;
        }

        auto artifact = this->playerex_->GetArtifactDict().at(type);

        int target_level = artifact->GetLevel();

        if (data.find("level") != data.end()) {
          target_level += data.at("level")->getInt();
        }

        if (data.find("is_all_in") !=
            data.end()) {  // if consuming the most artifacts
          target_level =
              artifact
                  ->GetLevelMax();  // attempt to enhance to the highest level
        }

        for (int i = 0;
             i < this->playerex_->GetPlayer()->getInventory().getSize(); ++i) {
          if (artifact->GetLevel() >= target_level) {
            break;
          }

          auto item = this->playerex_->GetPlayer()->getInventory().getSlot(i);

          if (!Artifact::CheckIsArtifact(item)) {
            continue;
          };

          auto artifact_to_consume = Artifact::Make(item, this->playerex_);

          if (artifact_to_consume->GetBaseConsumableEXP() >
              this->playerex_->GetMoraCount()) {  // if the mora is not enough
            continue;
          }

          // Increase the artifact EXP
          artifact->IncreaseArtifactEXP(
              artifact_to_consume->GetBaseConsumableEXP() +
              static_cast<int>(0.8 * artifact_to_consume->GetArtifactEXP()));

          // Consume the mora and the artifact consumable
          this->playerex_->ConsumeMora(
              artifact_to_consume->GetBaseConsumableEXP());

          artifact_to_consume.reset();
          
          this->playerex_->GetPlayer()->getInventory().removeItem_s(i, 1);
        }
        Schedule::nextTick(
            [this, type]() { this->OpenCharacterArtifacts(type); });
      });
}

void Menu::OpenCharacterAscend() {
  auto character = this->playerex_->GetCharacter();

  // Check if it is time to ascend
  if ((Character::kAcensionPhaseMaxLevelList[character->GetAscensionPhase()] !=
       character->GetLevel()) ||
      character->GetLevel() == 90) {
    this->OpenCharacter();
    return;
  }

  bool can_ascend = true;
  for (const auto& item : character->GetAscensionMaterials()) {
    if (item.first == "genshicraft:mora_1") {
      if (this->playerex_->GetMoraCount() < item.second) {
        can_ascend = false;
      }
    } else {
      if (this->playerex_->GetItemCount(item.first) < item.second) {
        can_ascend = false;
      }
    }
  }

  if (!can_ascend) {
    std::string content;

    content += "§cNot enough ascension materials!\n\n";
    content += "§fAscension materials:\n";

    for (const auto& item : character->GetAscensionMaterials()) {
      std::string item_name;
      if (item.first == "genshicraft:mora_1") {
        item_name = "§bMora";
      } else {
        item_name = ItemStack::create(item.first)->getName();
      }
      content += "§f" + item_name + " §fx" + std::to_string(item.second) + "\n";
    }

    Form::SimpleForm form("Ascend / " + character->GetName(), content);

    form.sendTo(this->playerex_->GetPlayer(),
                [this](Player* player, int option) {
                  if (option == -1) {
                    this->OpenCharacter();
                    return;
                  }
                });
  } else {
    Form::SimpleForm form("Ascend / " + character->GetName(),
                          "Are you sure to ascend?");

    form.addButton("Continue");

    form.sendTo(this->playerex_->GetPlayer(),
                [this](Player* player, int option) {
                  if (option == -1) {
                    this->OpenCharacter();
                    return;
                  }

                  auto character = this->playerex_->GetCharacter();

                  for (const auto& item : character->GetAscensionMaterials()) {
                    if (item.first == "genshicraft:mora_1") {
                      this->playerex_->ConsumeMora(item.second);
                    } else {
                      this->playerex_->ConsumeItem(item.first, item.second);
                    }
                  }

                  character->IncreaseAscensionPhase();

                  this->OpenCharacter();
                  return;
                });
  }
}

void Menu::OpenCharacterDetails() {
  auto character = this->playerex_->GetCharacter();

  std::string content;

  content += "§f";
  auto stats_description = character->GetStatsDescription(true);
  for (int i = 0; i < stats_description.size(); ++i) {
    if (i == 0) {
      content += "§7Base Stats\n";
    } else if (i == 5) {
      content += "\n§7Advanced Stats\n";
    } else if (i == 12) {
      content += "\n§7Elemental Type\n";
    }
    content += "§f" + stats_description[i] + "\n";
  }

  Form::SimpleForm form("Details / " + character->GetName(), content);

  form.sendTo(this->playerex_->GetPlayer(), [this](Player* player, int option) {
    if (option == -1) {
      this->OpenCharacter();
      return;
    }
  });
}

void Menu::OpenCharacterLevelUp() {
  auto character = this->playerex_->GetCharacter();

  // Check if it is time to level up
  if (Character::kAcensionPhaseMaxLevelList[character->GetAscensionPhase()] ==
      character->GetLevel()) {
    this->OpenCharacter();
    return;
  }

  auto max_character_exp =
      this->playerex_->GetItemCount("genshicraft:wanderer_s_advice") * 1000 +
      this->playerex_->GetItemCount("genshicraft:adventurer_s_experience") *
          5000 +
      this->playerex_->GetItemCount("genshicraft:hero_s_wit") * 20000;

  auto max_up_level = character->GetLevelByCharacterEXP(
      max_character_exp + character->GetCharacterEXP());

  Form::CustomForm form("Level Up / " + character->GetName());

  form = form.addLabel("text_level",
                       "Lv." + std::to_string(character->GetLevel()));

  form.addSlider("level", "Level(s) to up", 0,
                 max_up_level - character->GetLevel());

  form.sendTo(
      this->playerex_->GetPlayer(),
      [this](Player* player,
             std::map<string, std::shared_ptr<Form::CustomFormElement>> data) {
        // Go back to main menu if the player cancels the form
        if (data.empty()) {
          this->OpenCharacter();
          return;
        }

        auto character = this->playerex_->GetCharacter();

        auto enhanced_level =
            data.at("level")->getInt() + character->GetLevel();

        while (character->GetLevel() < enhanced_level &&
               this->playerex_->GetItemCount("genshicraft:hero_s_wit") > 0) {
          this->playerex_->ConsumeItem("genshicraft:hero_s_wit", 1);
          character->IncreaseCharacterEXP(20000);
        }

        while (character->GetLevel() < enhanced_level &&
               this->playerex_->GetItemCount(
                   "genshicraft:adventurer_s_experience") > 0) {
          this->playerex_->ConsumeItem("genshicraft:adventurer_s_experience",
                                       1);
          character->IncreaseCharacterEXP(5000);
        }

        while (character->GetLevel() < enhanced_level &&
               this->playerex_->GetItemCount("genshicraft:wanderer_s_advice") >
                   0) {
          this->playerex_->ConsumeItem("genshicraft:wanderer_s_advice", 1);
          character->IncreaseCharacterEXP(1000);
        }

        Schedule::nextTick([this]() { this->OpenCharacterLevelUp(); });
      });
}

void Menu::OpenCharacterWeapon() {
  static const std::string kRefinementSymbolList[6] = {" ", "①", "②",
                                                       "③", "④", "§6⑤"};

  // Check if the player is holding a GenshiCraft weapon
  if (!this->playerex_->GetWeapon()) {
    this->OpenCharacter();
    return;
  }

  auto weapon = this->playerex_->GetWeapon();

  std::string content;

  content += "§7";
  if (weapon->GetType() == Weapon::Type::kSword) {
    content += "Sword";
  } else if (weapon->GetType() == Weapon::Type::kClaymore) {
    content += "Claymore";
  } else if (weapon->GetType() == Weapon::Type::kPolearm) {
    content += "Polearm";
  } else if (weapon->GetType() == Weapon::Type::kCatalyst) {
    content += "Catalyst";
  } else if (weapon->GetType() == Weapon::Type::kBow) {
    content += "Bow";
  }
  content += "\n";

  for (const auto& line : weapon->GetBaseStatsDescription()) {
    content += "§f" + line + "\n";
  }

  content += "§6";
  for (int i = 0; i < weapon->GetRarity(); ++i) {
    content += "★";
  }
  content += "\n";

  content +=
      "§fLv." + std::to_string(weapon->GetLevel()) + " §7/ " +
      std::to_string(
          Weapon::kAcensionPhaseMaxLevelList[weapon->GetAscensionPhase()]);
  content += " §f";
  for (int i = 0; i < ((weapon->GetRarity() <= 2) ? 4 : 6); ++i) {
    if (i == weapon->GetAscensionPhase()) {
      content += "§7";
    }
    content += "✦";
  }
  content += "\n";

  if (weapon->GetRarity() >= 3) {
    content +=
        "§eRefinement Rank " + std::to_string(weapon->GetRefinement()) + "\n";
  }

  Form::SimpleForm form("Weapon / " + this->playerex_->GetWeapon()->GetName(),
                        content);

  if (Weapon::kAcensionPhaseMaxLevelList[weapon->GetAscensionPhase()] ==
      weapon->GetLevel()) {
    if ((weapon->GetRarity() >= 3 && weapon->GetLevel() != 90) ||
        (weapon->GetRarity() <= 2 && weapon->GetLevel() != 70)) {
      form = form.addButton("Ascend", "", [this](Player* player) {
        this->OpenCharacterWeaponAscend();
      });
    }
  } else {
    form = form.addButton("Enhance", "", [this](Player* player) {
      this->OpenCharacterWeaponEnhance();
    });
  }

  if (weapon->GetRarity() >= 3 && weapon->GetRefinement() < 5) {
    // form = form.addButton("Refine", "", [this](Player* player) {
    //   this->OpenCharacterWeaponRefine();
    // });
  }

  form.sendTo(this->playerex_->GetPlayer(), [this](Player* player, int option) {
    if (option == -1) {
      this->OpenCharacter();
      return;
    }
  });
}

void Menu::OpenCharacterWeaponAscend() {
  // Check if the player is holding a GenshiCraft weapon
  if (!this->playerex_->GetWeapon()) {
    this->OpenCharacter();
    return;
  }

  auto weapon = this->playerex_->GetWeapon();

  // Check if it is time to ascend
  if ((Weapon::kAcensionPhaseMaxLevelList[weapon->GetAscensionPhase()] !=
       weapon->GetLevel()) ||
      ((weapon->GetRarity() >= 3 && weapon->GetLevel() == 90) ||
       (weapon->GetRarity() <= 2 && weapon->GetLevel() == 70))) {
    this->OpenCharacterWeapon();
    return;
  }

  bool can_ascend = true;
  for (const auto& item : weapon->GetAscensionMaterials()) {
    if (item.first == "genshicraft:mora_1") {
      if (this->playerex_->GetMoraCount() < item.second) {
        can_ascend = false;
      }
    } else {
      if (this->playerex_->GetItemCount(item.first) < item.second) {
        can_ascend = false;
      }
    }
  }

  if (!can_ascend) {
    std::string content;

    content += "§cNot enough ascension materials!\n\n";
    content += "§fAscension materials:\n";

    for (const auto& item : weapon->GetAscensionMaterials()) {
      std::string item_name;
      if (item.first == "genshicraft:mora_1") {
        item_name = "§bMora";
      } else {
        item_name = ItemStack::create(item.first)->getName();
      }
      content += "§f" + item_name + " §fx" + std::to_string(item.second) + "\n";
    }

    Form::SimpleForm form("Ascend / " + weapon->GetName(),
                          content);  // the first 2 characters of the weapon
                                     // name are the formatting code

    form.sendTo(this->playerex_->GetPlayer(),
                [this](Player* player, int option) {
                  if (option == -1) {
                    this->OpenCharacterWeapon();
                    return;
                  }
                });
  } else {
    Form::SimpleForm form(
        "Ascend / " + weapon->GetName(),
        "Are you sure to ascend?");  // the first 2 characters of the weapon
                                     // name are the formatting code

    form.addButton("Continue");

    form.sendTo(this->playerex_->GetPlayer(),
                [this](Player* player, int option) {
                  if (option == -1) {
                    this->OpenCharacterWeapon();
                    return;
                  }

                  auto weapon = this->playerex_->GetWeapon();

                  for (const auto& item : weapon->GetAscensionMaterials()) {
                    if (item.first == "genshicraft:mora_1") {
                      this->playerex_->ConsumeMora(item.second);
                    } else {
                      this->playerex_->ConsumeItem(item.first, item.second);
                    }
                  }

                  weapon->IncreaseAscensionPhase();

                  Schedule::nextTick([this]() { this->OpenCharacterWeapon(); });
                  return;
                });
  }
}

void Menu::OpenCharacterWeaponEnhance() {
  // Check if the player is holding a GenshiCraft weapon
  if (!this->playerex_->GetWeapon()) {
    this->OpenCharacter();
    return;
  }

  auto weapon = this->playerex_->GetWeapon();

  // Check if it is time to enhance
  if (Weapon::kAcensionPhaseMaxLevelList[weapon->GetAscensionPhase()] ==
      weapon->GetLevel()) {
    this->OpenCharacterWeapon();
    return;
  }

  auto max_weapon_exp =
      this->playerex_->GetItemCount("genshicraft:enhancement_ore") * 400 +
      this->playerex_->GetItemCount("genshicraft:fine_enhancement_ore") * 2000 +
      this->playerex_->GetItemCount("genshicraft:mystic_enhancement_ore") *
          10000;

  auto max_enhanced_level =
      weapon->GetLevelByWeaponEXP(max_weapon_exp + weapon->GetWeaponEXP());

  Form::CustomForm form("Enhance / " + weapon->GetName());

  form =
      form.addLabel("text_level", "Lv." + std::to_string(weapon->GetLevel()));

  auto description = weapon->GetBaseStatsDescription();
  for (int i = 0; i < description.size(); ++i) {
    form = form.addLabel("text_weapon_stats_" + std::to_string(i),
                         description.at(i));
  }

  form.addSlider("level", "Level(s) to enhance", 0,
                 max_enhanced_level - weapon->GetLevel());

  form.sendTo(
      this->playerex_->GetPlayer(),
      [this](Player* player,
             std::map<string, std::shared_ptr<Form::CustomFormElement>> data) {
        // Go back to main menu if the player cancels the form
        if (data.empty()) {
          this->OpenCharacterWeapon();
          return;
        }

        auto weapon = this->playerex_->GetWeapon();

        auto enhanced_level = data.at("level")->getInt() + weapon->GetLevel();

        while (weapon->GetLevel() < enhanced_level &&
               this->playerex_->GetItemCount(
                   "genshicraft:mystic_enhancement_ore") > 0) {
          this->playerex_->ConsumeItem("genshicraft:mystic_enhancement_ore", 1);
          weapon->IncreaseWeaponEXP(10000);
        }

        while (weapon->GetLevel() < enhanced_level &&
               this->playerex_->GetItemCount(
                   "genshicraft:fine_enhancement_ore") > 0) {
          this->playerex_->ConsumeItem("genshicraft:fine_enhancement_ore", 1);
          weapon->IncreaseWeaponEXP(2000);
        }

        while (weapon->GetLevel() < enhanced_level &&
               this->playerex_->GetItemCount("genshicraft:enhancement_ore") >
                   0) {
          this->playerex_->ConsumeItem("genshicraft:enhancement_ore", 1);
          weapon->IncreaseWeaponEXP(400);
        }

        Schedule::nextTick([this]() { this->OpenCharacterWeaponEnhance(); });
      });
}

void Menu::OpenCharacterWeaponRefine() {
  // Check if the player is holding a GenshiCraft weapon
  if (!this->playerex_->GetWeapon()) {
    this->OpenCharacter();
    return;
  }
}

void Menu::OpenMain() {
  Form::SimpleForm form("GenshiCraft Menu", "");

  // form = form.addButton("Shop", "textures/menu/shop.bmp");

  form = form.addButton("Party Setup", "textures/menu/party_setup.bmp",
                        [this](Player* player) { this->OpenPartySetup(); });

  form = form.addButton("Character", "textures/menu/character.bmp",
                        [this](Player* player) { this->OpenCharacter(); });

  // form = form.addButton("Wish", "textures/menu/wish.bmp");

  form.sendTo(this->playerex_->GetPlayer());
}

void Menu::OpenPartySetup() {
  Form::SimpleForm form(
      "Party Setup",
      "Current character: " + this->playerex_->GetCharacter()->GetName());

  for (auto&& character : this->playerex_->GetAllCharacters()) {
    form =
        form.addButton(character->GetName(), "textures/menu/characters/" +
                                                 character->GetName() + ".png");
  }

  form.sendTo(this->playerex_->GetPlayer(), [this](Player* player, int option) {
    if (option == -1) {
      this->OpenMain();
      return;
    }

    this->playerex_->SelectCharacter(option);

    this->OpenPartySetup();
  });
}

}  // namespace genshicraft
