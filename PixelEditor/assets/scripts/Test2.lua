local light = {}

r = 0.0
g = 0.2
b = 0.4

totalTime = 0.0
function light.Update(time)
	entityId = Host_GetEntityByName("Light")
	--print(time)
	totalTime = totalTime + time

	if(totalTime > 1.0)
	then
			r = r + 0.2
			if(r > 1.0)
			then 
				r = 0.0
			end
			totalTime = 0.0
	end

	Host_SetLightComponentColor(entityId, r, g, b)
end

return light