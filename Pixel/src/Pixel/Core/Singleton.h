#pragma once
namespace Pixel {
	template<typename T>
	class Singleton {
		static T* m_Singleton;

	public:
		Singleton()
		{
			PX_CORE_ASSERT(!m_Singleton, "singleton is not nullptr!");
			//calculate the offset of T* and Singleton<T>* distance
			std::uintptr_t offset = (std::uintptr_t)(T*)1 - (std::uintptr_t)(Singleton<T>*)(T*)1;
			m_Singleton = (T*)((std::uintptr_t)this + offset);//get the derived class's pointer
		}
		virtual ~Singleton() 
		{
			PX_CORE_ASSERT(m_Singleton != nullptr, "singleton is nullptr!");
			m_Singleton = nullptr;
		}

		static T& GetSingleton()
		{
			PX_CORE_ASSERT(m_Singleton, "singleton is nullptr!");
			return *m_Singleton;
		}
		static T* GetSingletonPtr()
		{
			return m_Singleton;
		}
	};

	template<typename T> T* Singleton<T>::m_Singleton = nullptr;
}