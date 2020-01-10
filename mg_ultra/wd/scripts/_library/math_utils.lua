--A bunch of mathematical functions

--returns value clammped between min and max
function math.clamp(value, min, max)
	if value < min then
		return min
	elseif value > max then
	    return max
	else
		return value
	end
end

function math.to_degrees(r)
	return r*(180/math.pi)
end

function math.to_radians(d)
	return d*(math.pi/180)
end

--takes an angle and reframes it within the principle
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

--takes a cartesian point, and returns a polar vector (mag, ang)
function math.to_polar(x, y)
	return math.to_magnitude(x,y), math.to_degrees(math.atan2(y,x))
end

--takes a polar vector, and returns cartesian
function math.to_point(mag, ang)
	return mag*math.cos(math.to_radians(ang)), mag*math.sin(math.to_radians(ang))
end

--takes two angles a and b
--computes what to add to a to get to b in the fastest way
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

----The array class of functions deal with handling arrays
----framed as n-dimensional vectors of numbers
arrays = {}

--takes multiple arrays and a index
--and returns multiple indexed values
function arrays.index_multiple_array(i, first, ...)
	if is_nil(first) then
		return nil;
	else 
		return first[i], arrays.index_multiple_array(i, ...)
	end
end

--adds an element to the end of a new array
function arrays.append(current, element)
	current[#current + 1] = element
end

--takes multiple n-d arrays and composes into a single (n+1)-d array 
--e.g:
--a = {10, 13, 14}
--b = {11, 12, 13}
--c = {{10, 13, 14}, {11, 12, 13}}
function arrays.compose(...) 
	local current = {}
	for i = 1,select("#",...) do
		arrays.append(current, select(i, ...))
	end
	return current
end


--takes an n-d array and decomposes it into
--a set of (n-1)-d arrays
function arrays.decompose(array)
	return unpack(array)
end

--create an iterator which allows iterating over multiple arrays
--will continue until first array is nil
function arrays.multi_iter(...)
	local i = 0
	local p = arrays.compose(...) 
	return function()
		i = i + 1
		return arrays.index_multiple_array(i, arrays.decompose(p))
	end
end

--takes an n-d array and returns a negated version
function arrays.negate(a)
	if is_number(a) then
		return -a
	else 
		return arrays.compose( apply_function_to_set(arrays.negate, arrays.decompose(a)) )
	end
end

--takes an n-d array and transposes upon the 2 highest dimensions
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

--takes m count n-dimensional arrays
--alternativly takes a composition of n-dimension arrays
--and adds each element
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

--takes an n-dimension array A,
--and multiplies by m
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