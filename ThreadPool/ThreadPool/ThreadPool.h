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

	using Worker = std::tuple<ThreadState, std::thread>;

public:
	void EnqueueJob(std::function<void(void)> _job);

private:
	DECLARE_SINGLETON(ThreadPool)

	void Initialize();
	void WorkerThread(); //run in the thread
	void AllocationThread(); //������ �߰� �Ҵ�
	void DeallocateThread();
	void WatchThread(); //������ ���� �����ϸ� �߰� �Ҵ�

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