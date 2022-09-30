#include "pxpch.h"
#include "ScriptableEntity.h"

namespace Pixel {
	lua_State* ScriptableEntity::g_pLuaState;

	//Get The Transform Component


	void ScriptableEntity::OnCreate()
	{
		{
			if (g_pLuaState != nullptr)
			{
				//---Open Script---
				int ret = luaL_dofile(g_pLuaState, m_path.c_str());
				if (ret)
				{
					PIXEL_CORE_ERROR("Load Script Error");
					return;
				}
				//---Open Script---

				//---get module table index---
				m_tableIndex = lua_gettop(g_pLuaState);
				//---get module table index---
			}
		}
	}

	int Lua_Print(lua_State* pLuaState)
	{
		int32_t index = lua_gettop(pLuaState);
		const char* message = lua_tostring(pLuaState, index);
		PIXEL_CORE_INFO(message);
		lua_pop(pLuaState, 1);
		return 0;
	}
}