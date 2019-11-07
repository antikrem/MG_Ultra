--extra string functionality

--pads a string with spaces until it reaches the given length
--returns new string
function string.pad_string(string, paddedLength)
	local padding = paddedLength - string.len(string)
	padding = math.clamp(padding, 0, padding)
	return string .. string.rep(" ", padding)
end