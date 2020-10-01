-- A bunch of mathematical functions

-- Returns value clammped between min and max
function math.clamp(value, min, max)
	if value < min then
		return min
	elseif value > max then
	    return max
	else
		return value
	end
end

-- Implementation of tend to
function math.tend_to(current, target, step)
	local difference = math.min(math.abs(target - current), step)
	if current > target then return current - difference else return current + difference end
end

function math.to_degrees(r)
	return r*(180/math.pi)
end

function math.to_radians(d)
	return d*(math.pi/180)
end

-- Takes an angle and reframes it within the principle
-- range of -180 to 180
function math.to_principle(angle)
	angle = angle % 360
	if (angle > 180) then 
		return angle - 360 
	else 
		return angle 
	end
end

function math.to_magnitude(x, y)
	return math.sqrt(x^2+y^2)
end

-- Takes a cartesian point, and returns a polar vector (mag, ang)
function math.to_polar(x, y)
	return math.to_magnitude(x,y), math.to_degrees(math.atan2(y,x))
end

-- Takes a polar vector, and returns cartesian
function math.to_point(mag, ang)
	return mag*math.cos(math.to_radians(ang)), mag*math.sin(math.to_radians(ang))
end

-- Rotate a point about the origin
function math.rotate_point(x, y, delta)
	local mag, ang = math.to_polar(x, y)
	ang = ang + delta
	return math.to_point(mag, ang)
end

-- Takes two angles a and b
-- computes what to add to a to get to b in the fastest way
function math.compute_smallest_turn(target, current)
	local difference = math.to_principle(target) - math.to_principle(current)
	if -180 < difference and difference < 180 then
		return difference
	elseif 180 <= difference then
		return difference - 360
	else
		return difference + 360
	end
end

-- Interpolates [0,1] to [a,b] linearly
function math.lerp(t, a, b)
	return a + t * (b - a)
end

--R eturn of vector of random a, b, default 0,1
function math.rand_vec(length, a, b)
	if is_nil(a) then a = 0 end
	if is_nil(b) then b = 1 end

	return math.q_rand_vec(length, a, b)
end

-- Return of vector of random a, b, no nil check
function math.q_rand_vec(length, a, b)
	if length <= 0 then 
		return nil 
	else
		return math.lerp(math.random(), a, b), math.q_rand_vec(length - 1, a, b) 
	end
end

-- Returns a point from a random circle
function math.sample_uniform_circle(radius)
	local r = radius * math.sqrt(math.random())
	local t = math.pi * 2 * math.random()
	return r * math.cos(t), r * math.sin(t)
end

--- The array class of functions deal with handling arrays
--- framed as n-dimensional vectors of numbers
arrays = {}

-- Takes multiple arrays and a index
-- and returns multiple indexed values
function arrays.index_multiple_array(i, first, ...)
	if is_nil(first) then
		return nil;
	else 
		return first[i], arrays.index_multiple_array(i, ...)
	end
end

-- Adds an element to the end of a new array
function arrays.append(current, element)
	current[#current + 1] = element
end

-- Takes multiple n-d arrays and composes into a single (n+1)-d array 
-- e.g:
-- a = {10, 13, 14}
-- b = {11, 12, 13}
-- c = {{10, 13, 14}, {11, 12, 13}}
function arrays.compose(...) 
	local current = {}
	for i = 1,select("#",...) do
		arrays.append(current, select(i, ...))
	end
	return current
end


-- Takes an n-d array and decomposes it into
-- a set of (n-1)-d arrays
function arrays.decompose(array)
	return unpack(array)
end

-- Create an iterator which allows iterating over multiple arrays
-- will continue until first array is nil
function arrays.multi_iter(...)
	local i = 0
	local p = arrays.compose(...) 
	return function()
		i = i + 1
		return arrays.index_multiple_array(i, arrays.decompose(p))
	end
end

-- Takes an n-d array and returns a negated version
function arrays.negate(a)
	if is_number(a) then
		return -a
	else 
		return arrays.compose( apply_function_to_set(arrays.negate, arrays.decompose(a)) )
	end
end

-- Takes an n-d array and transposes upon the 2 highest dimensions
function arrays.transpose(a)
	local result = {}

	for i = 1, #a[1] do
        result[i] = {}
        for j = 1, #a do
            result[i][j] = a[j][i]
        end
    end
 
    return result
end

-- Takes m count n-dimensional arrays
-- alternativly takes a composition of n-dimension arrays
-- and adds each element
function arrays.sum(first, ...)
	--if we have more than 1 element
	--this is the first invokation
	--and we compose
	if not is_nil(...) then
		return arrays.sum(arrays.compose(first, ...))
	end

	--can't decompose further
	if is_number(first[1]) then
		--reached first order stage of algorithm
		--this reduces to sum
		return sum_set(arrays.decompose(first))		
	end

	--otherwise we need to transpose highest two orders
	local tFirst = arrays.transpose(first)

	--decompose, apply next iteration, recompose
	return arrays.compose( apply_function_to_set(arrays.sum, arrays.decompose(tFirst)) )
end

-- Takes an n-dimension array A,
-- and multiplies by m
function arrays.multiply(m, A)
	if is_number(A) then
		return m*A 
	else
		for i = 1,#A do
			A[i] = arrays.multiply(m, A[i])
		end
		return A
	end
end