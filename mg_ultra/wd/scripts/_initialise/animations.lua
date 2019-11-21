--Some static functions for querying 

--Allfunctions within this namespace
AnimationMaster = {}

--querys an animation set for the size of an animation type
--will return nil on failure
function AnimationMaster.query_animation_size(animationSet, animationType)
	animationType = animationType or 1
	local resultx, resulty = queryAnimation(animationSet, animationType)

	if resultx < 0 or resulty < 0  then 
		return nil
	else 
		return resultx, resulty
	end
end
