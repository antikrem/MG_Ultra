--particles.lua

--Particles are generated from particle types
--These templates can be added and then spawned
--from entities with the ComponentParticle
--This interface can be used for managing particle types
Particles = {}

--Different engine responses to a particle moving out of type box
ParticleBoxResponse = {}
ParticleBoxResponse.Nothing = 0
ParticleBoxResponse.Delete = 1
ParticleBoxResponse.Wrap = 2

--Add a new particle type
--Two possible signatures:
--Particles.add_new_type(string particleTypeName, string animationSetName) -> uses default animation (1)
--Particles.add_new_type(string particleTypeName, string animationSetName, int animation)
--returns key for further modification
Particles.add_new_type = function(particleTypeName, animationSetName, animation)
	animation = animation or 1
	addNewParticleType(particleTypeName, animationSetName, animation)
end

--Specifies the response for a given particle type
Particles.set_type_response = setParticleTypeResponse

--Specifies the box dimension (half dimension) for a given particle type
Particles.set_box_dimension = setParticleTypeBoxDimension

--Specifies the response for a given particle type
Particles.set_box_center = setParticleTypeBoxCenter

--Modifies the bloom factor of a particle type
Particles.set_bloom_factor = setParticleTypeBloomFactor

--Modifies the max life deviation of a particle type
Particles.set_max_life_deviation = setParticleTypeMaxLife

--Modifies the rotateToFace parameter, where a true value rotates particle to face 
--towards movement
Particles.set_rotate_to_face = setParticleTypeRotateToFace

--Modifies the featherness mean and featherness deviation of a particle type
Particles.set_featherness = setParticleTypeFeatherness

--Modifies the weight mean and weight deviation of a particle type
Particles.set_weight = setParticleTypeWeight

--Gets the particle key for a specific particle name
Particles.get_type = getParticleType

--Clears all particles
Particles.clear_all = clearAllParticles;


--Some default particle types
Particles.add_new_type("gold", "particle_gold")
Particles.set_bloom_factor("gold", 4.0)
Particles.set_max_life_deviation("gold", 4.0)
Particles.set_featherness("gold", 0.1, 0.01)

Particles.add_new_type("death_gold", "particle_gold_frag")
Particles.set_bloom_factor("death_gold", 4.0)
Particles.set_max_life_deviation("death_gold", 3.5)
Particles.set_featherness("death_gold", 0.012, 0.025)
Particles.set_rotate_to_face("death_gold", true)
