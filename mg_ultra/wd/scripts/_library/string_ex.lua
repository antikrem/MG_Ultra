--extra string functionality

--pads a string with spaces until it reaches the given length
--returns new string
function string.pad_string(string, paddedLength, token)
	token = token or " "
	local padding = paddedLength - string.len(string)
	padding = padding > 0 and padding or 0
	padding = math.clamp(padding, 0, padding)
	return string .. string.rep(" ", padding)
end

--takes a substring of a string from start to length
function string.sub_string(str, length)
	return string.sub(str, 0, length)
end

--combined substring and pad 
function string.pad_sub(str, length, token)
	return string.sub_string(string.pad_string(str, length, token), length)
end