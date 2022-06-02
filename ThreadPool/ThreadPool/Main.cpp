#include <iostream>
#include <thread>
#include <list>
#include <windows.h>
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
int foo() {
	for (int i = 0; i < 20; ++i) {
		std::cout << i;
		Sleep(500);
	}
	return 1;
}

int fooo(int& _i) {
	for (int i = 0; i < 20; ++i) {
		std::cout << i;
		Sleep(500);
	}
	--_i;
	return _i * 100;
}



void foo1() {
	int i = 9;
	Thread<int> th1(foo);
	Thread<int> th2(fooo, i);

	std::cout << std::endl << "asdf: " << i << std::endl;;
	std::cout << " --result: " << th1.GetReturn();
	std::cout << " --result: " << th2.GetReturn();
	std::cout << std::endl << "qwerty: " << i << std::endl;;
}


int main(void) {
	ThreadPool::CreateSingleton();
	ThreadPool::GetSingleton()->Initialize(100);
	
	foo1();



	return 0;
}