#pragma once

namespace Pixel {
	//Forward declartion
	class ScriptableEntity;
	struct NativeScriptComponent
	{
		//------
		ScriptableEntity* Instance = nullptr;

		//ScriptableEntity*(*InstantiateScript)();
		//void (*DestroyScript)(NativeScriptComponent*);

		std::string m_path;

		void Instantiate(std::string path);

		void Destroy();


		/*template<typename T>
		void Bind()
		{
			InstantiateScript = [](){ return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}*/
		//------

		/*lua_State* m_pLuaState;

		void (*InstantiateScript)(NativeScriptComponent* nsc);
		void (*DestroyScript)(NativeScriptComponent*);

		void Bind()
		{
			InstantiateScript = [](NativeScriptComponent* nsc) { nsc->m_pLuaState = luaL_newstate();
				luaL_openlibs(nsc->m_pLuaState);
			};
			DestroyScript = [](NativeScriptComponent* nsc) { lua_close(nsc->m_pLuaState); };
		}*/
	};
}