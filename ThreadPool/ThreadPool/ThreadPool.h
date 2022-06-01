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
*	Singleton으로 관리되는 스레드 관리 객체
*	void InitializeBaseUtil()에서 관리 객체를 로드하며 실행 됨
*   WatchThreadPool()을 통해 스레드 추가 할당 및 장시간 사용되지 않는 스레드 해제
*	EnqueueJob()			스레드에 작업을 할당하기 위해 큐에 넣어 놓는다.
*	WorkerThread()			작업을 하는 스레드
*	AllocationThread()		스레드 할당
*	DeallocateThread()		스레드 해제
*	WatchThreadPool()		스레드 풀 감시 (미완성)
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
	void AllocationThread(uint32_t _threadCnt); //스레드 추가 할당
	void DeallocateThread();
	void WatchThread(); //스레드 수가 부족하면 추가 할당

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
//반환형이 void일 경우 처리
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