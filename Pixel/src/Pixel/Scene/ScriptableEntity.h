#pragma once

#include "Entity.h"

extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}

namespace Pixel
{
	//------test------
	int Lua_Print(lua_State* pLuaState);
	//------test------

	class ScriptableEntity
	{
	public:
		//------Lua State------
		static lua_State* g_pLuaState;

		std::string m_path;

		int32_t m_tableIndex = -1;
		//------Lua State------

		virtual ~ScriptableEntity(){}

		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}
	protected:
		virtual void OnCreate();
		virtual void OnDestroy()
		{
			if (g_pLuaState != nullptr)
			{
				lua_close(g_pLuaState);
				g_pLuaState = nullptr;
			}
		}
		virtual void OnUpdate(Timestep ts)
		{
			if (m_tableIndex != -1)
			{
				lua_getfield(g_pLuaState, m_tableIndex, "Update");
				lua_pushnumber(g_pLuaState, ts.GetSeconds());
				lua_call(g_pLuaState, 1, 0);
			}
		}
	private:
		Entity m_Entity;
		friend class Scene;
	};
}