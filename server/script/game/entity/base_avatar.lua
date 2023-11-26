
local Avatar = Class()

function Avatar:on_ready()
	print(string.format("[lua] Avatar:on_ready self.%s", self))
end

function Avatar:msg_from_cell(msg)
	print(string.format("[lua] yj_method_test ret.%s", self:yj_method_test(3, 5)))
end


return Avatar
