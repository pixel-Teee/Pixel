local test = {}

--- update rotation ---
MousePosX, MousePosY = 300, 400
firstMouse = true
function UpdateRotate(entityId)
	x, y = Host_GetMousePos()
	--print("x:%f", x)
	--print("y:%f", y)
	if(firstMouse)
	then
		MousePosX = x
		MousePosY = y
		firstMouse = false
	end
	
	xOffset = MousePosX - x
	yOffset = MousePosY - y
	MousePosX = x
	MousePosY = y
	
	pitch, yaw, roll = Host_GetTransformComponentRotation(entityId)
	
	yaw = yaw + xOffset * 0.01
	pitch = pitch + yOffset * 0.01

	
	if(pitch > 89.0)
	then
		pitch = 89.0
	end

	if(pitch < -89.0)
	then
		pitch = -89.0
	end
	
	Host_SetTransformComponentRotation(entityId, pitch, yaw, roll)
end
--- update rotation ---

function test.Update(time)
	local entityId = Host_GetEntityByName("Camera")

	x, y, z = Host_GetTransformComponentLocation(entityId)
	frontx, fronty, frontz, rightx, righty, rightz = Host_GetTransformComponentDirectionAndRight(entityId);

	--Host_SetTransformComponentRotation(entityId, pitch, yaw, roll)
	--print(string.format("%.f", frontx))
	--print(string.format("%.f", fronty))
	--print(string.format("%.f", frontz))
	lastY = y
	if(Host_GetAsyncKeyInput(87))
	then
		x = x + frontx * 0.02
		y = y + fronty * 0.02
		z = z + frontz * 0.02
	elseif(Host_GetAsyncKeyInput(83))
	then
		x = x - frontx * 0.02
		y = y - fronty * 0.02
		z = z - frontz * 0.02
	elseif(Host_GetAsyncKeyInput(65))
	then
		x = x - rightx * 0.02
		y = y - righty * 0.02
		z = z - rightz * 0.02
	elseif(Host_GetAsyncKeyInput(68))
	then
		x = x + rightx * 0.02
		y = y + righty * 0.02
		z = z + rightz * 0.02
	end
	y = lastY
--[[
	if(Host_GetAsyncKeyInput(87))
		y = y + 1
	elseif(Host_GetAsyncKeyInput(65))
		x = x - 1
	elseif(Host_GetAsyncKeyInput(68))
		x = x + 1
	elseif(Host_GetAsyncKeyInput(82))
		y = y - 1
--]]
	Host_SetTransformComponentLocation(entityId, x, y, z)

	UpdateRotate(entityId)
end

return test