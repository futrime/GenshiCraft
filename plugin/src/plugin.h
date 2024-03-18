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
 * @file plugin.h
 * @author Futrime (futrime@outlook.com)
 * @brief Declaration of global interfaces and objects
 * @version 1.0.0
 * @date 2022-08-04
 *
 * @copyright Copyright (c) 2022 Futrime
 *
 */

#ifndef GENSHICRAFT_PLUGIN_H_
#define GENSHICRAFT_PLUGIN_H_

#include <EventAPI.h>
#include <LoggerAPI.h>

#include <string>

#include "damage.h"
#include "version.h"

namespace genshicraft {

/**
 * @brief Check the protocol version and warn if not matched
 *
 */
void CheckProtocolVersion();

/**
 * @brief Initialize the plugin
 *
 */
void Init();

/**
 * @brief Get the clocks in seconds
 *
 * @return The clocks
 */
double GetNowClock();

/**
 * @brief The handler for MobHurtEvent
 *
 * @param event The event
 * @return True if the action can be performed
 */
bool OnMobHurt(Event::MobHurtEvent& event);

/**
 * @brief The handler for PlayerDropItemEvent
 *
 * @param event The event
 * @return True if the action can be performed
 */
bool OnPlayerDropItem(Event::PlayerDropItemEvent& event);

/**
 * @brief THe handler for PlayerExperienceAddEvent
 *
 * @param event The event
 * @return True if the action can be performed
 */
bool OnPlayerExperienceAdd(Event::PlayerExperienceAddEvent& event);

/**
 * @brief The handler for PlayerInventoryChangeEvent
 *
 * @param event The event
 * @return Always true
 */
bool OnPlayerInventoryChange(Event::PlayerInventoryChangeEvent& event);

/**
 * @brief The handler for PlayerJoinEvent
 *
 * @param event The event
 * @return Always true
 */
bool OnPlayerJoin(Event::PlayerJoinEvent& event);

/**
 * @brief The handler for PlayerLeftEvent
 *
 * @param event The event
 * @return Always true
 */
bool OnPlayerLeft(Event::PlayerLeftEvent& event);

/**
 * @brief The handler for PlayerOpenContainerEvent
 *
 * @param event The event
 * @return True if the action can be performed
 */
bool OnPlayerOpenContainer(Event::PlayerOpenContainerEvent& event);

/**
 * @brief The handler for PlayerOpenContainerScreenEvent
 *
 * @param event The event
 * @return True if the action can be performed
 */
bool OnPlayerOpenContainerScreen(Event::PlayerOpenContainerScreenEvent& event);

/**
 * @brief The handler for PlayerRespawnEvent
 *
 * @param event The event
 * @return Always true
 */
bool OnPlayerRespawn(Event::PlayerRespawnEvent& event);

/**
 * @brief The handler for PlayerUseItemEvent
 *
 * @param event The event
 * @return True if the action can be performed
 */
bool OnPlayerUseItem(Event::PlayerUseItemEvent& event);

/**
 * @brief This function executes per tick.
 *
 */
void OnTick();

extern Logger logger;  // The logger

}  // namespace genshicraft

#endif  // GENSHICRAFT_PLUGIN_H_