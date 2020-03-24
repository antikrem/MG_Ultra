/* A system that updates entities that contain 
 * a component movement, and component position
 * Moves acceleration into velocity and velocity into
 * component position
 */

#ifndef __SYSTEM_MOVEMENT__
#define __SYSTEM_MOVEMENT__

#include "system.h"

#include "component_position.h"
#include "component_movement.h"
#include "component_static_movement.h"
#include "component_clamp_position.h"
#include "component_magnetise_to_player.h"

class SystemMovement : public System {

	// Last player position
	Point3 playerPosition = Point3(0);

public:
	SystemMovement() {
		debugName = "s_movement";
		target = SubPoolTarget(
			SubPoolComponents::ByComponents<ComponentPosition, ComponentMovement>()
		);
	}

	void precycle(EntityPool* pool) override {
		auto player = pool->getCachedEnt(ETPlayer);
		if (player) {
			playerPosition = player->getComponent<ComponentPosition>()->getPosition3();
		}
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		auto pos = getComponent<ComponentPosition>(components);
		auto mov = getComponent<ComponentMovement>(components);
		auto sta = getComponent<ComponentStaticMovement>(components);
		auto cla = getComponent<ComponentClampPosition>(components);
		auto mag = getComponent<ComponentMagnetiseToPlayer>(components);

		if (mov->getUpdateInSystem()) {
			Point3 currentPos = pos->getPosition3();

			// Manage magnetise
			if (mag && mag->getMagnetise()) {
				// On first cycle freeze acceleration and push polar component to velocity
				if (!mag->markInitialised()) {
					mov->pushPolarToVelocity();
					mov->pinVelocity();
					mov->unlockCaps();
					mov->setSpeedChange(mag->getAcceleration());
				}

				// Accelerate to player
				mov->setAngle(pos->getPosition3().directionTo(playerPosition).getXY().getAng());
			}

			// Handle static movement update (exclusive to an active magnetise to player)
			else if (sta) {
				currentPos = sta->getUpdatedPosition(mov, currentPos);
			}

			// Update with internal conditions
			currentPos = mov->getUpdatedPosition(currentPos);

			// Clamp if required
			if (cla && cla->getActive()) {
				currentPos = cla->applyClamp(currentPos);
			}
			
			pos->setPosition(currentPos);
			
		}
		
	}

};


#endif