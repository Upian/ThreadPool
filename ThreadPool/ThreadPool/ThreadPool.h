#pragma once

#include <thread>
#include <queue>
#include <list>
#include <mutex>
#include <functional>
#include <chrono>
#include "Singleton.h"

/*
*	NOTE
*	Thread pool
*	Singleton���� �����Ǵ� ������ ���� ��ü
*	void InitializeBaseUtil()���� ���� ��ü�� �ε��ϸ� ���� ��
*   WatchThreadPool()�� ���� ������ �߰� �Ҵ� �� ��ð� ������ �ʴ� ������ ����
*	EnqueueJob()			�����忡 �۾��� �Ҵ��ϱ� ���� ť�� �־� ���´�.
*	WorkerThread()			�۾��� �ϴ� ������
*	AllocationThread()		������ �Ҵ�
*	DeallocateThread()		������ ����
*	WatchThreadPool()		������ Ǯ ���� (�̿ϼ�)
*
*
*/

enum class ThreadState : uint8_t {
	None = 0,
	Running,
	Idle,
	Count
};

class ThreadPool : public Singleton<ThreadPool>{
private:
	class Worker {
	public:
		Worker();
		void operator()(ThreadPool& _threadPool);

	private:
		ThreadState m_state = ThreadState::None; //Thread state
		std::chrono::system_clock::time_point m_allocTime; //Manage waiting time 
	};

//	using WorkerThread = std::tuple<ThreadState, std::thread>;
	using WorkerThread = std::pair<Worker, std::thread>;
public:
	void EnqueueJob(std::function<void(void)> _job);
	void Initialize(uint32_t _threadCnt = 0);

private:
	DECLARE_SINGLETON(ThreadPool)
	friend class ThreadPool;

//	void WorkerThread(); //run in the thread
	void AllocationThread(uint32_t _threadCnt); //������ �߰� �Ҵ�
	void DeallocateThread();
	void WatchThread(); //������ ���� �����ϸ� �߰� �Ҵ�

//	ThreadState GetThreadState(Worker& worker) const { return std::get<0>(worker); }
//	std::thread& GetThread(Worker& worker) const { return std::get<1>(worker); }
//	void SetThreadStateRunning(Worker& worker) { std::get<0>(worker) = ThreadState::Running; }
//	void SetThreadStateIdle(Worker& worker) { std::get<0>(worker) = ThreadState::Idle; }

	const size_t m_cpuCoresCount = std::thread::hardware_concurrency();

	std::mutex m_mutex;
	std::condition_variable m_condition;
	bool m_allStop = false;
	int m_runningThreadCount = 0;
	
	std::thread* m_watchThread = nullptr;
	std::queue<std::function<void(void)> > m_jobQueue;
	std::list<WorkerThread> m_threads;
};

//Thread

#pragma region General

template<typename T_Type>
class Thread {
public:
	template<typename T_Func, typename... T_Args,
		typename std::enable_if_t<std::is_function<T_Func>::value>* = nullptr>
		Thread(T_Func&, const T_Args&...);
	Thread() = default;
	~Thread() = default;

	template<typename T_Func, typename... T_Args,
		typename std::enable_if_t<std::is_function<T_Func>::value>* = nullptr>
		void Start(T_Func&, const T_Args&...);

	bool IsWorking() const { return m_isDone; }
	void WaitResult(); // Block the process until the thread's operation is complete
	T_Type GetReturn(); // Block the process until the thread's operation is complete and get return value

private:
	bool m_isDone = false;
	T_Type m_returnValue;

};


/////////////////////////////////////////////////////////////////////////////

template<typename T_Type>
template<typename T_Func, typename... T_Args,
	typename std::enable_if_t<std::is_function<T_Func>::value>* >
	Thread<T_Type>::Thread(T_Func& func, const T_Args&... args) {
	ThreadPool::GetSingleton()->EnqueueJob([this, func, args...]()-> void {
		m_returnValue = func(args...);
		m_isDone = true;
	});
}

template<typename T_Type>
template<typename T_Func, typename... T_Args,
	typename std::enable_if_t<std::is_function<T_Func>::value>* >
	void Thread<T_Type>::Start(T_Func& func, const T_Args&... args) {
	ThreadPool::GetSingleton()->EnqueueJob([this, func, args...]()-> void {
		m_returnValue = func(args...);
		m_isDone = true;
	});
}

template<typename T_Type>
void Thread<T_Type>::WaitResult() {
	while (false == m_isDone) {}
}

template<typename T_Type>
T_Type Thread<T_Type>::GetReturn() {
	this->WaitResult();
	return m_returnValue;
}
#pragma endregion 

#pragma region Handle void specialized
//��ȯ���� void�� ��� ó��
//Handle if return type is 'void'
template<>
class Thread<void> {
public:
	template<typename T_Func, typename... T_Args,
		typename std::enable_if_t<std::is_function<T_Func>::value>* = nullptr>
		Thread(T_Func&, const T_Args&...);
	Thread() = default;
	~Thread() = default;

	template<typename T_Func, typename... T_Args,
		typename std::enable_if_t<std::is_function<T_Func>::value>* = nullptr>
		void Start(T_Func&, const T_Args&...);

	bool IsWorking() const { return m_isDone; }
	void WaitResult(); // Block the process until the thread's operation is complete

private:
	bool m_isDone = false;
};

/////////////////////////////////////////////////////////////////////////////

template<typename T_Func, typename... T_Args,
	typename std::enable_if_t<std::is_function<T_Func>::value>* >
	Thread<void>::Thread(T_Func& func, const T_Args&... args) {
	ThreadPool::GetSingleton()->EnqueueJob([this, func, args...]()-> void {
		func(args...);
		m_isDone = true;
	});
}

template<typename T_Func, typename... T_Args,
	typename std::enable_if_t<std::is_function<T_Func>::value>* >
	void Thread<void>::Start(T_Func& func, const T_Args&... args) {
	ThreadPool::GetSingleton()->EnqueueJob([this, func, args...]()-> void {
		func(args...);
		m_isDone = true;
	});
}


#pragma endregion 