
local Avatar = Class()

function Avatar:on_ready()
end

function Avatar:msg_from_cell(msg)
	print(string.format("[lua] Avatar:msg_from_cell %s", msg))
end


return Avatar
