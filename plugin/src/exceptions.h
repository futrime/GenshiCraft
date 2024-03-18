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
 * @file exceptions.h
 * @author Futrime (futrime@outlook.com)
 * @brief Declaration of exception classes
 * @version 1.0.0
 * @date 2022-08-05
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#ifndef GENSHICRAFT_EXCEPTIONS_H_
#define GENSHICRAFT_EXCEPTIONS_H_

#include <stdexcept>
#include <string>

namespace genshicraft {

/**
 * @brief The Exception class is the base class for all exceptions.
 *
 */
class Exception : public std::exception {
 public:
  Exception(const std::string& what_arg) : what_arg_(what_arg) {}

  virtual const char* what() const noexcept { return this->what_arg_.c_str(); }

 private:
  std::string what_arg_;
};

/**
 * @brief The ExceptionActorEx class is the base class for exceptions related
 * to the ActorEx class.
 *
 */
class ExceptionActorEx : public Exception {
 public:
  using Exception::Exception;
};

/**
 * @brief The ExceptionActorNotFound represents a failed attempt to get the
 * Actor object.
 *
 */
class ExceptionActorNotFound : public ExceptionActorEx {
 public:
  ExceptionActorNotFound()
      : ExceptionActorEx(
            "[genshicraft::ExceptionActorNotFound] The Actor object is not "
            "found.") {}

  using ExceptionActorEx::ExceptionActorEx;
};

/**
 * @brief The ExceptionCharacter class is the base class for exceptions related
 * to the Character class.
 *
 */
class ExceptionCharacter : public Exception {
 public:
  using Exception::Exception;
};

/**
 * @brief The ExceptionInvalidCharacterData class represents that the character
 * data is invalid.
 *
 */
class ExceptionInvalidCharacterData : public ExceptionCharacter {
 public:
  ExceptionInvalidCharacterData()
      : ExceptionCharacter(
            "[genshicraft::ExceptionInvalidCharacterData] The character data "
            "is invalid.") {}

  using ExceptionCharacter::ExceptionCharacter;
};

/**
 * @brief The ExceptionNotACharacter class represents that the name does not
 * match any character.
 *
 */
class ExceptionNotACharacter : public ExceptionCharacter {
 public:
  ExceptionNotACharacter()
      : ExceptionCharacter(
            "[genshicraft::ExceptionNotACharacter] The name does not match any "
            "character.") {}

  using ExceptionCharacter::ExceptionCharacter;
};

/**
 * @brief The ExceptionDamage class is the base class for exceptions related
 * to the Damage class.
 *
 */
class ExceptionDamage : public Exception {
 public:
  using Exception::Exception;
};

/**
 * @brief The ExceptionNotNormalDamage class represents that the damage is not
 * normal damage.
 *
 */
class ExceptionNotNormalDamage : public ExceptionDamage {
 public:
  ExceptionNotNormalDamage()
      : ExceptionDamage(
            "[genshicraft::ExceptionNotNormalDamage] The damage is not normal "
            "damage.") {}

  using ExceptionDamage::ExceptionDamage;
};

/**
 * @brief The ExceptionNotPrimaryDamage class represents that the damage is not
 * secondary transformative damage.
 *
 */
class ExceptionNotPrimaryDamage : public ExceptionDamage {
 public:
  ExceptionNotPrimaryDamage()
      : ExceptionDamage(
            "[genshicraft::ExceptionNotPrimaryDamage] The damage is not "
            "primary damage.") {}

  using ExceptionDamage::ExceptionDamage;
};

/**
 * @brief The ExceptionNotTransformativeDamage class represents that the damage
 * is not transformative damage.
 *
 */
class ExceptionNotTransformativeDamage : public ExceptionDamage {
 public:
  ExceptionNotTransformativeDamage()
      : ExceptionDamage(
            "[genshicraft::ExceptionNotTransformativeDamage] The damage is not "
            "transformative "
            "damage.") {}

  using ExceptionDamage::ExceptionDamage;
};

/**
 * @brief The ExceptionNotTrueDamage class represents that the damage is not
 * true damage.
 *
 */
class ExceptionNotTrueDamage : public ExceptionDamage {
 public:
  ExceptionNotTrueDamage()
      : ExceptionDamage(
            "[genshicraft::ExceptionNotTrueDamage] The damage is not true "
            "damage.") {}

  using ExceptionDamage::ExceptionDamage;
};

/**
 * @brief The ExceptionFood class is the base class for exceptions related
 * to food.
 *
 */
class ExceptionFood : public Exception {
 public:
  using Exception::Exception;
};

/**
 * @brief The ExceptionNotFood class represents that the items are not food.
 *
 */
class ExceptionNotFood : public ExceptionFood {
 public:
  ExceptionNotFood()
      : ExceptionFood(
            "[genshicraft::ExceptionNotFood] The items are not food."){};

  using ExceptionFood::ExceptionFood;
};

/**
 * @brief The ExceptionMethodNotAllowed class represents that the method is not
 * allowed to call.
 *
 */
class ExceptionMethodNotAllowed : public Exception {
 public:
  ExceptionMethodNotAllowed()
      : Exception(
            "[genshicraft::ExceptionMethodNotAllowed] This method is not "
            "allowed to call."){};

  using Exception::Exception;
};

/**
 * @brief The ExceptionMobEx class is the base class for exceptions related
 * to the MobEx class.
 *
 */
class ExceptionMobEx : public Exception {
 public:
  using Exception::Exception;
};

/**
 * @brief The ExceptionMobNotFound represents a failed attempt to get the Mob
 * object.
 *
 */
class ExceptionMobNotFound : public ExceptionMobEx {
 public:
  ExceptionMobNotFound()
      : ExceptionMobEx(
            "[genshicraft::ExceptionMobNotFound] The Mob object is not "
            "found.") {}

  using ExceptionMobEx::ExceptionMobEx;
};

/**
 * @brief The ExceptionModifier class is the base class for exceptions related
 * to the Modifier class.
 *
 */
class ExceptionModifier : public Exception {
 public:
  using Exception::Exception;
};

/**
 * @brief The ExceptionIncorrectModifierType represents a failed attempt to get
 * the modifier as the target type.
 *
 */
class ExceptionIncorrectModifierType : public ExceptionModifier {
 public:
  ExceptionIncorrectModifierType()
      : ExceptionModifier(
            "[genshicraft::ExceptionIncorrectModifierType] The modifier cannot "
            "be converted to the target type.") {}

  using ExceptionModifier::ExceptionModifier;
};

/**
 * @brief The ExceptionPlayerEx class is the base class for exceptions related
 * to the PlayerEx class.
 *
 */
class ExceptionPlayerEx : public Exception {
 public:
  using Exception::Exception;
};

/**
 * @brief The ExceptionCharacterNumberOutOfRange class represents that the
 * character number is out of range.
 *
 */
class ExceptionCharacterNumberOutOfRange : public ExceptionPlayerEx {
 public:
  ExceptionCharacterNumberOutOfRange()
      : ExceptionPlayerEx(
            "[genshicraft::ExceptionCharacterNumberOutOfRange] The character "
            "number is out of range.") {}

  using ExceptionPlayerEx::ExceptionPlayerEx;
};

/**
 * @brief The ExceptionItemsNotEnough class represents that the item is not a
 * GenshiCraft item.
 *
 */
class ExceptionItemsNotEnough : public ExceptionPlayerEx {
 public:
  ExceptionItemsNotEnough()
      : ExceptionPlayerEx(
            "[genshicraft::ExceptionItemsNotEnough] The items are not "
            "enough.") {}

  using ExceptionPlayerEx::ExceptionPlayerEx;
};

/**
 * @brief The ExceptionMorasNotEnough class represents that the Mora is not a
 * GenshiCraft Mora.
 *
 */
class ExceptionMoraNotEnough : public ExceptionPlayerEx {
 public:
  ExceptionMoraNotEnough()
      : ExceptionPlayerEx(
            "[genshicraft::ExceptionMoraNotEnough] The Mora is not enough.") {}

  using ExceptionPlayerEx::ExceptionPlayerEx;
};

/**
 * @brief The ExceptionWeapon class is the base class for exceptions related to
 * the Weapon class.
 *
 */
class ExceptionWeapon : public Exception {
 public:
  using Exception::Exception;
};

/**
 * @brief The ExceptionNotAWeapon class represents that the item is not a
 * GenshiCraft item.
 *
 */
class ExceptionNotAWeapon : public ExceptionWeapon {
 public:
  ExceptionNotAWeapon()
      : ExceptionWeapon(
            "[genshicraft::ExceptionNotAWeapon] The item is not a GenshiCraft "
            "weapon.") {}

  using ExceptionWeapon::ExceptionWeapon;
};

/**
 * @brief The ExceptionArtifact class is the base class for exceptions related
 * to the Artifact class.
 *
 */
class ExceptionArtifact : public Exception {
 public:
  using Exception::Exception;
};

/**
 * @brief The ExceptionNotAnArtifact class represents that the item is not a
 * GenshiCraft item.
 *
 */
class ExceptionNotAnArtifact : public ExceptionArtifact {
 public:
  ExceptionNotAnArtifact()
      : ExceptionArtifact(
            "[genshicraft::ExceptionNotAnArtifact] The item is not a "
            "GenshiCraft "
            "Artifact.") {}

  using ExceptionArtifact::ExceptionArtifact;
};

}  // namespace genshicraft

#endif  // GENSHICRAFT_EXCEPTIONS_H_