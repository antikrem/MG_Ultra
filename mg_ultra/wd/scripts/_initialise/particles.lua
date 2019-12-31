--particles.lua

--Particles are generated from particle types
--These templates can be added and then spawned
--from entities with the ComponentParticle
--This interface can be used for managing particle types
Particles = {}

--Add a new particle type
--Two possible signatures:
--Particles.add_new_type(string particleTypeName, string animationSetName) -> uses default animation (1)
--Particles.add_new_type(string particleTypeName, string animationSetName, int animation)
--returns key for further modification
Particles.add_new_type = function(particleTypeName, animationSetName, animation)
	animation = is_nil(animation) and 1 or animation
	addNewParticleType(particleTypeName, animationSetName, animation)
end

--Gets the particle key for a specific particle name
Particles.get_type = getParticleType


--Some default particle types
Particles.add_new_type("gold", "particle_gold")