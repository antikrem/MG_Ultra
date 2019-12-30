/*manages a particle manager cached entity*/
#ifndef __SYSTEM_PARTICLE_SPAWNER__
#define __SYSTEM_PARTICLE_SPAWNER__

#include "system.h"

#include "component_particle.h"
#include "component_position.h"
#include "component_graphics.h"

#include "texture.h"
#include "particle_master.h"

class SystemParticleSpawner : public System {

	ParticleMaster* particleMaster = nullptr;

	vector<Particle> particleCache;
public:


	SystemParticleSpawner() {
		debugName = "s_particle_spawner";
		target = SubPoolTarget(
			SubPoolComponents::ByComponents<ComponentParticle>()
		);	
	}

	void setParticleMaster(ParticleMaster* particleMaster) {
		this->particleMaster = particleMaster;
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		shared_ptr<ComponentParticle> comPar = getComponent<ComponentParticle>(components);
		shared_ptr<ComponentPosition> comPos = getComponent<ComponentPosition>(components);
		shared_ptr<ComponentGraphics> comGra = getComponent<ComponentGraphics>(components);
		
		//check if comPar is initialised
		if (comPar->getKey() < 0) {
			int key = particleMaster->getKeyFromName(comPar->getKeyName());
			if (key < 0) {
				err::logMessage("invalid particle key thing");
			}
			else {
				comPar->setKey(key);
				if (comGra) {
					string setName = comGra->getAnimationState().animationSetName;
					int x, y;
					tie(x, y) = g_aniquery::getAnimationSize(setName, 1);
					if (x > 0 && y > 0) {
						comPar->setPositionDeviation((float)max(x, y) / 2);
					}
					
				}
			}
		}

		Point3 position = comPos ? comPos->getPosition3() : Point3(0.0f);

		comPar->updateSpawner(particleCache, position);
	}

	void postcycle(EntityPool* entityPool) {
		particleMaster->addParticles(particleCache);
		particleCache.clear();
	}
};

#endif