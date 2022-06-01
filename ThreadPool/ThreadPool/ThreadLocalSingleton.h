#pragma once
/*
*	NOTE
*	Singleton<class name> ��� ��
*	DECLARE_THREAD_LOCAL_SINGLETON(class name) �� private�� ���� �� ������ ����
*	DECLARE_THREAD_LOCAL_SINGLETON�� ������, �Ҹ���
*	 CreateSingleton()		singleton Ŭ���� ���� �Ǵ� ��ȯ
*	GetSingleton()			singleton Ŭ���� ��ȯ
*	DistorySingleton()		singleton Ŭ���� �Ҹ�
*/

template<typename T_Class>
class ThreadLocalSingleton {
public:
	typedef T_Class SingletonClass;

	static SingletonClass* CreateSingleton();
	template <typename ...T_Args> static SingletonClass* CreateSingleton(const T_Args& ...args);
	static void DestroySingleton();
	static SingletonClass* GetSingleton();

protected:
	ThreadLocalSingleton() {}
	virtual ~ThreadLocalSingleton() { }

private:
	static thread_local SingletonClass* m_instance;
};

/*
*	Initialize
*/
template <typename T_Class>
typename thread_local ThreadLocalSingleton<T_Class>::SingletonClass* ThreadLocalSingleton<T_Class>::m_instance = nullptr;

/*
*	Create Singleton
*/
template <typename T_Class>
typename ThreadLocalSingleton<T_Class>::SingletonClass* ThreadLocalSingleton<T_Class>::CreateSingleton() {
	if (nullptr == m_instance)
		m_instance = new SingletonClass();

	return m_instance;
}
template <typename T_Class>
template <typename ...T_Args>
typename ThreadLocalSingleton<T_Class>::SingletonClass* ThreadLocalSingleton<T_Class>::CreateSingleton(const T_Args& ...args) {
	if (nullptr == m_instance)
		m_instance = new SingletonClass(args...);

	return m_instance;
}
/*
*	Get Singleton
*/
template <typename T_Class>
typename ThreadLocalSingleton<T_Class>::SingletonClass* ThreadLocalSingleton<T_Class>::GetSingleton() {
	return m_instance;
}

/*
*	Destory Singleton
*/
template<typename T_Class>
void ThreadLocalSingleton<T_Class>::DestroySingleton() {
	delete m_instance;
	m_instance = nullptr;
}
//#Singleton End


/*
*   DECLARE_SINGLETON
*   Make class to singlton
*   must define a constructor and destroyer
*/
#ifndef DECLARE_THREAD_LOCAL_SINGLETON
#define DECLARE_THREAD_LOCAL_SINGLETON( ClassName ) \
        friend class ThreadLocalSingleton< ClassName >; ClassName(); ~ClassName();
#endif // DECLARE_SINGLETON
