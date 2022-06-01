#pragma once

#include <thread>
#include <queue>
#include <list>
#include <mutex>
#include <functional>
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

	using Worker = std::tuple<ThreadState, std::thread>;

public:
	void EnqueueJob(std::function<void(void)> _job);

private:
	DECLARE_SINGLETON(ThreadPool)

	void Initialize();
	void WorkerThread(); //run in the thread
	void AllocationThread(); //스레드 추가 할당
	void DeallocateThread();
	void WatchThread(); //스레드 수가 부족하면 추가 할당

	ThreadState GetThreadState(Worker& worker) const { return std::get<0>(worker); }
	std::thread& GetThread(Worker& worker) const { return std::get<1>(worker); }
	void SetThreadStateRunning(Worker& worker) { std::get<0>(worker) = ThreadState::Running; }
	void SetThreadStateIdle(Worker& worker) { std::get<0>(worker) = ThreadState::Idle; }

	const size_t m_cpuCoresCount = std::thread::hardware_concurrency();

	std::mutex m_mutex;
	std::condition_variable m_condition;
	bool m_allStop = false;
	int m_runningThreadCount = 0;
	
	std::thread* m_watchThread = nullptr;
	std::queue<std::function<void(void)> > m_jobQueue;
	std::list<Worker> m_threads;
	//std::list<std::thread> m_threads;
};