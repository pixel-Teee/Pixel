local camera = {}

totalTime = 0.0
function camera.Update(time)
	local entityId = Host_GetEntityByName("Camera")

	totalTime = totalTime + time
	if(Host_GetAsyncKeyInput(81))
	then
		x, y, z = Host_GetTransformComponentLocation(entityId)
		--print(entityId)
		--print(x)
		--print(y)
		--print(z)
		--[[
		print(x)
		print(y)
		print(z)
		--]]
		frontx, fronty, frontz, rightx, righty, rightz = Host_GetTransformComponentDirectionAndRight(entityId)

		if(totalTime > 0.4)
		then
			--print(x)
			--print(y)
			--print(z)
			local ballEntityId = Host_CopyEntity("Ball")
			--print(ballEntityId)
			--print(ballEntityId)
			Host_SetTransformComponentLocation(ballEntityId, x + frontx * 0.2, y + frontx * 0.2, z + frontz * 4.0)
			totalTime = 0.0
		end
	end
end


return camera