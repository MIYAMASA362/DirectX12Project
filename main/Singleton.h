#ifndef __CORE_SINGLETON_H__
#define __CORE_SINGLETON_H__
namespace Common 
{
	template<class T>
	class Singleton
	{
	public:
		static void createInstance();
		static void destoryInstance();
		static inline T* getInstance()
		{
			return mpInstance;
		}
	protected:
		Singleton<T>() = default;
		virtual ~Singleton<T>() = default;

	private:
		static T* mpInstance;
	};
	
	template<class T>
	inline void Singleton<T>::createInstance()
	{
		if (mpInstance == nullptr) 
		{
			mpInstance = new T();
		}
	}

	template<class T>
	inline void Singleton<T>::destoryInstance()
	{
		if (mpInstance != nullptr) 
		{
			delete mpInstance;
			mpInstance = nullptr;
		}
	}
}

#define DEFINE_SIGLETON(type) type* Common::Singleton<type>::mpInstance = nullptr

#endif