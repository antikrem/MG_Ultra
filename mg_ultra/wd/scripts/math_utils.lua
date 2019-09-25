--A bunch of mathematical functions

--returns value clammped between min and max
function clamp(value, min, max)
	if value < min then
		return min
	elseif value > max then
	    return max
	else
		return value
	end
end

function to_degrees(r)
	return r*(180/math.pi)
end

function to_radians(d)
	return d*(math.pi/180)
end

function to_magnitude(x, y)
	return math.sqrt(x^2+y^2)
end

--takes a cartesian point, and returns a polar vector (mag, ang)
function to_polar(x, y)
	return to_magnitude(x,y), to_degrees(math.atan(y,x))
end

--takes a polar vector, and returns cartesian
function to_point(mag, ang)
	return mag*math.cos(to_radians(ang)), mag*math.sin(to_radians(ang))
end