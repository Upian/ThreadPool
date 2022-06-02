#include <iostream>
#include <thread>
#include <list>
#include "ThreadPool.h"
#include "ThreadLocalSingleton.h"
class Test {
public:
	Test() {
		std::cout << "Make Test class" << std::endl;
	}
	~Test() {
		std::cout << "Remove Test class" << std::endl;
	}

	void operator()() {
		std::cout << "run thread" << std::endl;
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

class Test1 : public ThreadLocalSingleton<Test1> {
public:

private:
	DECLARE_THREAD_LOCAL_SINGLETON(Test1)
};

thread_local int i = 0;
/*
void foo() {
	std::cout << i++ << std::endl;
}
*/
void foo(int* _i) {
	std::cout << *_i++ << std::endl;
}



int main(void) {
	ThreadPool::CreateSingleton();
	ThreadPool::GetSingleton()->Initialize(100);
	

	return 0;
}