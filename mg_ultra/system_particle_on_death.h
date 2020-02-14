/*manages a particle manager cached entity*/
#ifndef __SYSTEM_PARTICLE_ON_DEATH__
#define __SYSTEM_PARTICLE_ON_DEATH__

#include "system.h"

#include "algorithm_ex.h"

#include "random_ex.h"

#include "component_particle_on_death.h"
#include "component_damage.h"
#include "component_health.h"
#include "component_position.h"
#include "component_movement.h"

#include "particle_master.h"

class SystemParticleOnDeath : public System {

	ParticleMaster* particleMaster = nullptr;

	vector<shared_ptr<Entity>> internalEntities;

	vector<Particle> particleCache;

	//handles the spawning of particles on death
	void handledDeath(shared_ptr<Entity> ent) {
		//If component has damage, it has not died due to combat collision
		auto dam = ent->getComponent<ComponentDamage>();
		if (dam && dam->getDamage() > 0) {
			return;
		}


		//otherwise spawn appropiate 
		Point3 vel(0.0f);
		auto mov = ent->getComponent<ComponentMovement>();
		if (mov) {
			vel.setXY(Point2::generateFromPolar(mov->getSpeed(), mov->getAngle()));
			vel += mov->getVelocity();
		}

		auto pod = ent->getComponent<ComponentParticleOnDeath>();

		auto pos = pod->isAdjustedPosition() ?
			pod->getAdjustedPosition() :
			ent->getComponent<ComponentPosition>()->getPosition3();

		auto spec = pod->getSpec();

		float lower, higher;
		tie(lower, higher) = pod->getVelocityFactorRange();

		float offset[3];

		float maxLifeDeviation = pod->getMaxLifeDeviation();
		float directionDeviation = pod->getDirectionDeviation();

		for (int i = 0; i < pod->getCount(); i++) {

			rand_ex::populate_next_norms(offset, 3, 0.0f, pod->getRadius() / 2);

			Point3 adjustedPos
				= pos + Point3(offset[0], offset[1], offset[2]);

			Point3 adjustedVel 
				= (vel * rand_ex::next_unif(lower, higher)).rotate(Point3(0.0f, 0.0f, 1.0f), rand_ex::next_norm(0.0f, directionDeviation));

			particleCache.push_back(
				Particle(
					pod->getKey(),
					rand_ex::next_unif(1.0f, maxLifeDeviation),
					adjustedPos,
					adjustedVel,
					spec.feathernessMean,
					spec.weightMean,
					spec.rotateToFace
				)
			);
		}
	}
public:


	SystemParticleOnDeath() {
		debugName = "s_particle_on_death";
		target = SubPoolTarget(
			SubPoolComponents::ByComponents<ComponentParticleOnDeath>()
		);
	}

	void setParticleMaster(ParticleMaster* particleMaster) {
		this->particleMaster = particleMaster;
	}

	void handleComponentMap(map<type_index, shared_ptr<Component>>& components, shared_ptr<Entity> ent, int id) override {
		auto pod = getComponent<ComponentParticleOnDeath>(components);
		if (!pod->isRegistered()) {
			int key = particleMaster->getKeyFromName(pod->getName());

			if (key < 0) {

			} 
			else {
				pod->setKey(key);

				//if pod has no radius, assign it based on graphics
				if (pod->getRadius() < 0) {
					auto gra = getComponent<ComponentGraphics>(components);
					if (gra) {
						int x, y;
						tie(x, y) = g_aniquery::getAnimationSize(gra->getAnimationState().animationSetName, 1);
						if (x > 0 && y > 0) {
							pod->setRadius((float)max(x, y) / 2);
						}

					}
				}

				internalEntities.push_back(ent);
				auto spec = particleMaster->getTypeSpecification(key);
				pod->setAllParameters(spec);
				pod->markIsRegistered();
			}
			
		}
	}

	void postcycle(EntityPool* entityPool) override {
		auto it = internalEntities.begin();
		
		bool cleanFlag = false;

		for (unsigned int i = 0; i < internalEntities.size(); i++) {
			if (!internalEntities[i]->getFlag()) {
				cleanFlag = true;
				handledDeath(internalEntities[i]);
			}
		}

		if (cleanFlag) {
			erase_sequential_if(internalEntities, [](shared_ptr<Entity> &ent) { return !ent->getFlag(); });
		}
		
		if (particleCache.size()) {
			particleMaster->addParticles(particleCache);
			particleCache.clear();
		}
		
	}
};

#endif