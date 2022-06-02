#include "ThreadPool.h"
//std::this_thread::get_id();

ThreadPool::Worker::Worker() {
	m_state = ThreadState::Idle;
}

void ThreadPool::Worker::operator()() {
	auto threadPool = ThreadPool::GetSingleton();
	if (nullptr == threadPool)
		return;

	while (true) {
		std::unique_lock<std::mutex> lock(threadPool->m_mutex);
		threadPool->m_condition.wait(lock, [&threadPool]()-> bool { // ture 반환시 대기를 멈추고 진행 //If it returns a value of true, stop waiting and continue
			return false == threadPool->m_jobQueue.empty() || true == threadPool->m_allStop;
			});
		if (true == threadPool->m_jobQueue.empty() && true == threadPool->m_allStop) return; //모든 스레드 중지

		std::function<void(void)> job = std::move(threadPool->m_jobQueue.front());
		threadPool->m_jobQueue.pop();
		lock.unlock();

		//Do work
		m_allocTime = std::chrono::system_clock::now();
		m_state = ThreadState::Running;
		job();
		m_state = ThreadState::Idle;
		lock.lock();
	}
}

ThreadPool::ThreadPool() { //Create thread
}

ThreadPool::~ThreadPool() {
	//waiting for thread what is unfinished         
	m_allStop = true;
	m_condition.notify_all();

	for (auto& iter : m_threads) {
		iter.second.join();
	}
	m_watchThread->join();
}
void ThreadPool::EnqueueJob(std::function<void(void)> _job) {
	std::unique_lock<std::mutex> lock(m_mutex);

	m_jobQueue.push(_job);
	m_condition.notify_one();
}

//private
void ThreadPool::Initialize(uint32_t _threadCnt) {
	this->AllocationThread(_threadCnt); // threadPool 초기 생성
	this->WatchThread(); // 감시 스레드 생성
}

void ThreadPool::AllocationThread(uint32_t _threadCnt) {
	//Create threads as many as cpu's thread
	if (0 == _threadCnt)
		_threadCnt = std::thread::hardware_concurrency();;

	for (uint32_t count = 0; count < _threadCnt; ++count) {
		Worker worker;
		m_threads.emplace_back(worker, worker);
	}
}

void ThreadPool::DeallocateThread() {
}

void ThreadPool::WatchThread() {
	if (nullptr != m_watchThread)
		return;

	m_watchThread = new std::thread();
}
