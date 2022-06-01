#include "ThreadPool.h"
//std::this_thread::get_id();

ThreadPool::ThreadPool() { //Create thread
//    this->Initialize();
}

ThreadPool::~ThreadPool() {
    //waiting for thread what is unfinished         
    m_allStop = true;
    m_condition.notify_all();
    m_runningThreadCount = 0;
    for (Worker& worker : m_threads) {
        this->GetThread(worker).join();
    }

}
void ThreadPool::EnqueueJob(std::function<void(void)> _job) {
    std::unique_lock<std::mutex> lock(m_mutex);

    m_jobQueue.push(_job);
    m_condition.notify_one();
}

//private
void ThreadPool::Initialize() {
    this->AllocationThread(); // threadPool �ʱ� ����
    this->WatchThread(); // ���� ������ ����
}

void ThreadPool::WorkerThread() {
    while (true) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock, [this]()-> bool { // ture ��ȯ�� ��⸦ ���߰� ���� //If it returns a value of true, stop waiting and continue
            return false == m_jobQueue.empty() || true == m_allStop;
            });
        if (true == m_jobQueue.empty() && true == m_allStop) return; //��� ������ ����

        std::function<void(void)> job = std::move(m_jobQueue.front());
        m_jobQueue.pop();
        ++m_runningThreadCount;
        lock.unlock();

        //Do work
        job();

        lock.lock();
        --m_runningThreadCount;
    }
}

void ThreadPool::AllocationThread() {
    //Create threads as many as cpu's thread
    for (int count = 0; count < m_cpuCoresCount; ++count) {
        m_threads.emplace_back(std::make_tuple(ThreadState::Idle, [this]()->void { this->WorkerThread(); }));
    }
}

void ThreadPool::DeallocateThread() {
}

void ThreadPool::WatchThread() {
    if (nullptr != m_watchThread)
        return;
    
    m_watchThread = new std::thread();
    auto watchDog = [this]()-> void {
        int idleThreadCount = 0;
        while (true) {
            if (0 <= m_runningThreadCount) //�������� �����尡 ���� ��� ����
                break;

            idleThreadCount = m_runningThreadCount - m_threads.size();



        }
    };

}
