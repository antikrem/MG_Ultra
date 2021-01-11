-- extra string functionality

-- Pads a string with spaces until it reaches the given length
-- returns new string
function string.pad_string(string, paddedLength, token, reverse)
	if is_nil(token) then token = " " end
	if is_nil(reverse) then reverse = false end
	local padding = paddedLength - string.len(string)
	padding = padding > 0 and padding or 0
	padding = math.clamp(padding, 0, padding)
	if reverse then
		return string.rep(token, padding) .. string
	else
		return string .. string.rep(token, padding)
	end
end

-- Takes a substring of a string from start to length
function string.sub_string(str, length)
	return string.sub(str, 0, length)
end

-- Combined substring and pad 
-- i.e. will pad if substring is too short
function string.pad_sub(str, length, token)
	return string.sub_string(string.pad_string(str, length, token), length)
end


-- Replaces a string with chacter at position
function string.replace_char(str, pos, r)
    return str:sub(1, pos-1) .. r .. str:sub(pos+1)
end