---Basic functions, can be used in any script
--No classes or mg functionality have been bounded, outside of global functions

--gets length of vairable input/tables/etc
function length(a)
	count = 0
	for i,v in pairs(a) do
		count = count + 1
	end
	return count
end

--checks if value is nil
function is_nil(a)
	return a == nil
end

--checks if value is numer
function is_number(a)
	return type(a) == "number"
end

function print_table(class)
	for key,val in pairs(class) do
		print(key, "=>", val);
	end
end

--Variadic print to console/log
function print(...) 
	--get size of input
	size = #{...}

	--handle single input of type table, for doing things like printing classes
	if size == 1 and type(select(1, ...)) == "table" then
		print_table(select(1, ...))
		return
	end
	
	printAdd(">>")

	--check for nil inputs
	if size == 0 then
		printAdd("nil")
		printPush()
		return
	end

	--otherwise general print
	for i = 1,size do

		v = select(i, ...)
	
		if v == nil then
			printAdd("nil")
		elseif type(v) == "userdata" and v.__name then
			--v is a class
			printAdd(v.__name)
		else
			printAdd(v)
		end

		printAdd(" ")
	
	end
	printPush()
end

function typeof(a)
	t = type(a)
	--check if a is of userdata, and if so, check if it has a name
	if t == "userdata" and a.__name then
	    --t is a class
		print(a.__name)
		return
	end
	print(t)
end

--checks element is in the provided list of elements
function check_element_in_set(element, ...)
	for i = 1,#{...} do
		if (element == select(i, ...)) then
			return true;
		end
	end
	return false;
end

--sums all elements in a set
function sum_set(first, ...)
	if is_nil(first) then
		return 0
	else
		return first + sum_set(...)
	end
end

--implementation of lua global unpack
function unpack(a, i)
	i = i or 1
	if not is_nil(a[i]) then
		return a[i], unpack(a, i + 1)
	end
end

--takes a function f and multiple arguments
--f will be applied with each of the arguments
--f should be in the form f : x -> f(x)
--then returned
function apply_function_to_set(f, ...)
	local result = {}
	for i = 1, #{...} do
		local temp = select(i, ...)
		result[i] = f(temp)
	end
	return unpack(result)
end

--definitions for events
EventNone = 0
EventQuit = 1
EventStateChange = 2
EventCallback = 3
EventProfileUpdate = 4

--Emits an event to the event que, 
function emit_event(eventType, ...)
	--check event is valid
	if not check_element_in_set(eventType, EventNone, EventQuit, EventStateChange, EventCallback, EventProfileUpdate) then
		print("EVENT: error, invalid event type submitted with emit_event")
		return
	end

	setEvent(eventType)
	--set values
	for i = 1,#{...} do
		addEventData( select(i, ...) )
	end

	pushEvent()
end

--creates a python style range
--shamelessly stolen from http://lua-users.org/wiki/RangeIterator
function range(from, to, step)
	step = step or 1
	return function(_, lastvalue)
		local nextvalue = lastvalue + step
		if step > 0 and nextvalue < to or step < 0 and nextvalue >= to then
			return nextvalue
		end
	end, nil, from - step
end

--Requests engine shutdown by emmiting a quit event
function quit()
	print("Exiting from console...")
	emit_event(EventQuit)
end

