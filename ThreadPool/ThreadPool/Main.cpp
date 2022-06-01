#include <iostream>
#include <thread>
#include <list>
#include "ThreadPool.h"
#include "ThreadLocalSingleton.h"
class Test {
public:
	Test() {
		std::cout << "make Test class" << std::endl;
	}
};

class CheckTime {
public:
	CheckTime() {
		start = clock();
		std::cout << "Begin" << std::endl;
	}
	~CheckTime() {

		std::cout << "Finish Time: " << clock() - start << std::endl;
	}

	clock_t start;
};

static thread_local int i = 0;

void foo() {
	++i;
	std::cout << i << std::endl;
}

void TestThread() {
	CheckTime t;

	std::list<Thread<void>> testList;
	for (int j = 0; j < 100000; ++j) {
		for (int i = 0; i < 100; ++i) {
			testList.emplace_back(foo);
		}
	}
}

void TestThread2() {
	CheckTime t;
	std::list<std::thread> testList;
	for (int j = 0; j < 100000; ++j) {
		for (int i = 0; i < 100; ++i) {
			testList.emplace_back(foo);
		}
	}
}

class Test1 : public ThreadLocalSingleton<Test1> {
public:

private:
	DECLARE_THREAD_LOCAL_SINGLETON(Test1)
};

int main(void) {
	ThreadPool::CreateSingleton();
	ThreadPool::GetSingleton()->Initialize(200);

//	TestThread();
//	TestThread2();

	foo();
	foo();
	foo();
	
	std::thread t1(foo);
	std::thread t2(foo);
	std::thread t3(foo);


	t1.join();
	t2.join();
	t3.join();

	return 0;
}