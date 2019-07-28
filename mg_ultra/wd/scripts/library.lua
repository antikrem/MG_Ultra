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

--definitions for events
EventNone = 0
EventQuit = 1
EventStateChange = 2
EventCallback = 3

--Emits an event to the event que, 
function emit_event(eventType, ...) 
	setEvent(eventType)
	--otherwise general print
	for i = 1,#{...} do
		addEventData( select(i, ...) )
	end

	pushEvent()
end

--Requests engine shutdown by emmiting a quit event
function quit()
	print("Exiting from console...")
	emit_event(EventQuit)
end

